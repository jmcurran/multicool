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
  // is the multinomial coefficient, which the R code computes exactly. If
  // elements is an integer, double, logical or character vector, the matrix
  // holds elements[code] for each code 1..k, otherwise it holds the codes.
  SEXP allPerm(int nPerms, SEXP elements){
    switch(TYPEOF(elements)){
      case INTSXP:  return fillPerms<INTSXP>(nPerms, elements);
      case REALSXP: return fillPerms<REALSXP>(nPerms, elements);
      case LGLSXP:  return fillPerms<LGLSXP>(nPerms, elements);
      case STRSXP:  return fillPerms<STRSXP>(nPerms, elements);
      default:      return fillPerms<INTSXP>(nPerms, R_NilValue);
    }
  };

  int getLength(void){
    return (int)m_vInitialState.size();
  };

  vector<int> getInitialState(void){
    return m_vInitialState;
  };

  bool hasNext(void){
    if(m_bFirst){
      m_bFirst = false;
    }else if(!advance()){
      return false;
    }

    setState();
    return true;
  };

  List nextPerm(void){
    bool b = hasNext();

    return List::create(_["b"] = b,
                        _["set"] = IntegerVector(m_vCurrState.begin(), m_vCurrState.end()));
  };

private:
  // writes every permutation into a matrix, walking the list straight into
  // the output rather than copying each state first. With no elements the
  // codes themselves are written.
  template <int RTYPE>
  SEXP fillPerms(int nPerms, SEXP elements){
    // iterate over a fresh copy so the state used by nextPerm() is not disturbed
    Multicool iter(m_vInitialState);
    bool useCodes = Rf_isNull(elements);
    Vector<RTYPE> vElements = useCodes ? Vector<RTYPE>(0) : Vector<RTYPE>(elements);
    Matrix<RTYPE> result = no_init(nPerms, getLength());
    R_xlen_t nRows = nPerms;
    int row = 0;

    do{
      if(row >= nPerms)
        Rcpp::stop("more permutations were generated than expected");

      if(row % 10000 == 0)
        Rcpp::checkUserInterrupt();

      // the matrix is column-major, so element (row, col) is at row + col * nRows
      R_xlen_t pos = row;
      for(item *y = iter.h; y; y = y->n, pos += nRows){
        if(useCodes)
          result[pos] = y->v;
        else
          result[pos] = vElements[y->v - 1];
      }

      row++;
    }while(iter.advance());

    if(row != nPerms)
      Rcpp::stop("fewer permutations were generated than expected");

    return result;
  };

  // moves the list on to the next permutation without copying the state.
  // Returns false when there are no more permutations.
  bool advance(void){
    item *j;
    item *t;
    item *s;

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
      return true;
    }

    return false;
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
