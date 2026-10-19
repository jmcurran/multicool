#include <vector>
#include <Rcpp.h>

using namespace Rcpp;
using namespace std;


/* Adapted from Aaron Williams multicool.c
Author: Aaron Williams (haron@uvic.ca)
Website: http://www.math.mcgill.ca/haron/

This code is distributed without conditions

This program outputs the permutations of any multiset in cool-lex order.
The program stores a single permutation in a singly-linked list, and then
each successive permutation in O(1)-time while using O(1) additional pointers.

Input is read from stdin as follows: n e_1 e_2 ... e_n where e_i <= e_{i+1} for all i.

For example, if input.txt is a file containing the following single line
6 1 1 2 3 3 3
then multicool < input.txt will output the cool-lex order for the permutations of {1,1,2,3,3,3}:
333211
133321
313321
etc */

// C++ version: James M. Curran (j.curran@auckland.ac.nz)

class Multicool{
  struct item {
    int v;
    item *n;
  };

  // storage for the linked list. It is never resized after construction, so
  // pointers into it stay valid. Copying is disabled because a copy's pointers
  // would still point into this object's storage.
  vector<item> m_vNodes;
  item *h; // head of the list
  item *i; // Williams' i pointer

  vector<int> m_vInitialState;
  vector<int> m_vCurrState;
  bool m_bFirst;

  void setState(void){
    int ctr = 0;

    for(item *y = h; y; y = y->n)
      m_vCurrState[ctr++] = y->v;
  }

public:
  Multicool(vector<int> x) :
    m_vNodes(x.size()), h(NULL), i(NULL),
    m_vInitialState(x), m_vCurrState(x), m_bFirst(true){
    int nx = (int)x.size();

    // hasNext() dereferences i, which is only set when there are at least two elements
    if(nx < 2)
      Rcpp::stop("a multiset must have at least two elements to be permuted");

    for(int ctr = 1; ctr < nx; ctr++){
      if(x[ctr] < x[ctr - 1])
        Rcpp::stop("the elements of the multiset must be in non-decreasing order");
    }

    // each element is pushed onto the front of the list, so the list runs
    // from the largest element to the smallest
    for(int ctr = 0; ctr < nx; ctr++){
      item *t = &m_vNodes[ctr];
      t->v = x[ctr];
      t->n = h;
      h = t;

      if(ctr == 1){
        i = h;
      }
    }
  };

  Multicool(const Multicool &) = delete;
  Multicool & operator=(const Multicool &) = delete;

  // returns every permutation as a row of an nPerms x length matrix. nPerms
  // is the multinomial coefficient, which the R code computes exactly.
  IntegerMatrix allPerm(int nPerms){
    // iterate over a fresh copy so the state used by nextPerm() is not disturbed
    Multicool iter(m_vInitialState);
    int nLength = getLength();
    IntegerMatrix result(nPerms, nLength);
    int row = 0;

    while(iter.hasNext()){
      if(row >= nPerms)
        Rcpp::stop("more permutations were generated than expected");

      if(row % 1000 == 0)
        Rcpp::checkUserInterrupt();

      for(int col = 0; col < nLength; col++)
        result(row, col) = iter.m_vCurrState[col];

      row++;
    }

    if(row != nPerms)
      Rcpp::stop("fewer permutations were generated than expected");

    return result;
  };

  int getLength(void){
    return (int)m_vInitialState.size();
  };

  vector<int> getInitialState(void){
    return m_vInitialState;
  };

  bool hasNext(void){
    item *j;
    item *t;
    item *s;

    if(m_bFirst){
      setState();
      m_bFirst = false;
      return true;
    }

    j = i->n;

    if(j->n || j->v < h->v) {
      if (j->n && i->v >= j->n->v) {
        s = j;
      } else {
        s = i;
      }
      t = s->n;
      s->n = t->n;
      t->n = h;
      if(t->v < h->v) {
        i = t;
      }
      h = t;
      setState();
      return true;
    }

    return false;
  };

  List nextPerm(void){
    bool b = hasNext();

    return List::create(_["b"] = b,
                        _["set"] = IntegerVector(m_vCurrState.begin(), m_vCurrState.end()));
  };
};

RCPP_MODULE(Multicool) {
  using namespace Rcpp;

  class_<Multicool>( "Multicool")
    .constructor< vector<int> >("Standard constructor")
    .method("allPerm", &Multicool::allPerm)
    .method("set", &Multicool::getInitialState)
    .method("length", &Multicool::getLength)
    .method("hasNext", &Multicool::hasNext)
    .method("nextPerm", &Multicool::nextPerm)
  ;
}
