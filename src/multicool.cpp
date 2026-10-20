#include <vector>
#include <cstring>
#include <climits>
#include <cmath>
#include <memory>
#include <thread>
#include <algorithm>
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
public:
  struct item {
    int v;
    item *n;
  };

private:
  // storage for the linked list. It is never resized after construction, so
  // pointers into it stay valid. Copying is disabled because a copy's pointers
  // would still point into this object's storage.
  vector<item> m_vNodes;
  item *h; // head of the list
  item *i; // Williams' i pointer

  vector<int> m_vInitialState;
  bool m_bFirst;

  // moves the list on to the next permutation. Returns false when there are
  // no more permutations, and keeps returning false after that.
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

public:
  Multicool(const vector<int> &x) :
    m_vNodes(x.size()), h(NULL), i(NULL),
    m_vInitialState(x), m_bFirst(true){
    int nx = (int)x.size();

    // advance() dereferences i, which is only set when there are at least two elements
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

  // the current list order and the position of i in it, which is all that
  // is needed for another object to carry on from this permutation
  struct Checkpoint {
    vector<int> order;
    int iPos;
  };

  Checkpoint checkpoint(void) const {
    Checkpoint cp;
    int pos = 0;

    for(const item *y = h; y; y = y->n, pos++){
      cp.order.push_back(y->v);
      if(y == i)
        cp.iPos = pos;
    }

    return cp;
  };

  // carries on from a checkpoint: the first step() gives the checkpointed
  // permutation. This makes no R API calls, so it is safe to use when
  // filling a result from several threads.
  Multicool(const Checkpoint &cp, const vector<int> &initial) :
    m_vNodes(cp.order.size()), h(NULL), i(NULL),
    m_vInitialState(initial), m_bFirst(true){
    // node k holds order[k] and links to node k + 1
    for(int k = (int)cp.order.size() - 1; k >= 0; k--){
      item *t = &m_vNodes[k];
      t->v = cp.order[k];
      t->n = h;
      h = t;
    }

    i = &m_vNodes[cp.iPos];
  };

  // moves on to the next permutation, which is then available from head().
  // The first call leaves the list at the initial permutation.
  bool step(void){
    if(m_bFirst){
      m_bFirst = false;
      return true;
    }

    return advance();
  };

  const item *head(void) const {
    return h;
  };

  int getLength(void) const {
    return (int)m_vInitialState.size();
  };

  // the largest code, i.e. the number of unique elements
  int getMaxCode(void) const {
    return m_vInitialState.back();
  };

  const vector<int> &getInitialState(void) const {
    return m_vInitialState;
  };
};

// Writes up to nRows permutations from it into the rows of a matrix, as
// elements[code] for each code 1..k. If fewer than nRows are left, the matrix
// is shrunk to fit, or R_NilValue is returned if there were none left.
template <int RTYPE>
SEXP fillRows(Multicool &it, int nRows, SEXP elements){
  Vector<RTYPE> vElements(elements);
  int nLength = it.getLength();
  Matrix<RTYPE> result = no_init(nRows, nLength);
  R_xlen_t nr = nRows;
  int row = 0;

  while(row < nRows && it.step()){
    if(row % 10000 == 0)
      Rcpp::checkUserInterrupt();

    // the matrix is column-major, so element (row, col) is at row + col * nr
    R_xlen_t pos = row;
    for(const Multicool::item *y = it.head(); y; y = y->n, pos += nr)
      result[pos] = vElements[y->v - 1];

    row++;
  }

  if(row == nRows)
    return result;

  if(row == 0)
    return R_NilValue;

  Matrix<RTYPE> shrunk = no_init(row, nLength);
  for(int col = 0; col < nLength; col++){
    for(int r = 0; r < row; r++)
      shrunk(r, col) = result(r, col);
  }

  return shrunk;
}

// Calls fillRows for the type of elements. Anything other than an integer,
// double, logical or character vector gets the codes 1..k, which the R code
// then maps to the elements itself.
SEXP fillDispatch(Multicool &it, int nRows, SEXP elements){
  switch(TYPEOF(elements)){
    case INTSXP:  return fillRows<INTSXP>(it, nRows, elements);
    case REALSXP: return fillRows<REALSXP>(it, nRows, elements);
    case LGLSXP:  return fillRows<LGLSXP>(it, nRows, elements);
    case STRSXP:  return fillRows<STRSXP>(it, nRows, elements);
    default:{
      IntegerVector codes = seq_len(it.getMaxCode());
      return fillRows<INTSXP>(it, nRows, codes);
    }
  }
}

// The R code holds the permutation object through an external pointer, and
// calls these functions directly. That is much cheaper per call than the
// Rcpp Module these replace.

// an external pointer is NULL after the object holding it is saved and
// reloaded, so check for that and give a useful message
Multicool &getMC(SEXP ptr){
  if(TYPEOF(ptr) != EXTPTRSXP || R_ExternalPtrAddr(ptr) == NULL)
    Rcpp::stop("this mc object is no longer valid, probably because it was saved and reloaded. Call initMC again");

  return *XPtr<Multicool>(ptr);
}

// [[Rcpp::export(rng = false)]]
SEXP mcCreate(std::vector<int> x){
  return XPtr<Multicool>(new Multicool(x), true);
}

// the element of an mc object (the list made by initMC) called name
SEXP mcPart(SEXP mcObj, const char *name){
  SEXP names = Rf_getAttrib(mcObj, R_NamesSymbol);

  for(R_xlen_t k = 0; k < Rf_xlength(names); k++){
    if(strcmp(CHAR(STRING_ELT(names, k)), name) == 0)
      return VECTOR_ELT(mcObj, k);
  }

  return R_NilValue;
}

// the next n permutations as the rows of a matrix (fewer at the end), or NULL
// when there are none left. With n = 1 a vector is returned instead. The
// argument checks are done here because they are much cheaper than in R,
// which matters when nextPerm is called once per permutation. If the mc
// object's elements are not plain (see isPlainElements in R) the codes are
// returned, and nextPerm maps them.
// [[Rcpp::export(rng = false)]]
SEXP mcNextPerm(SEXP mcObj, SEXP nSEXP){
  if(TYPEOF(mcObj) != VECSXP || !Rf_inherits(mcObj, "mc"))
    Rcpp::stop("mcObj must be of class mc");

  double d = (Rf_xlength(nSEXP) == 1 && (TYPEOF(nSEXP) == INTSXP || TYPEOF(nSEXP) == REALSXP)) ?
    Rf_asReal(nSEXP) : NA_REAL;
  if(ISNAN(d) || d < 1 || d != floor(d) || d > INT_MAX)
    Rcpp::stop("n must be a single positive integer");
  int n = (int)d;

  Multicool &mc = getMC(mcPart(mcObj, "mc"));
  SEXP elements = Rf_asLogical(mcPart(mcObj, "plain")) == TRUE ? mcPart(mcObj, "elements") : R_NilValue;

  SEXP result = PROTECT(fillDispatch(mc, n, elements));

  if(n == 1 && !Rf_isNull(result))
    Rf_setAttrib(result, R_DimSymbol, R_NilValue);

  UNPROTECT(1);
  return result;
}

// Fills all nPerms permutations using up to nThreads threads. Cool-lex makes
// each permutation from the previous one, so a fast serial pass that only
// moves pointers first records a checkpoint at the start of each thread's
// block of rows (this is about 10% of the work). Each thread then carries on
// from its checkpoint and writes its own rows. The threads make no R API
// calls, which is why this is only used for types whose data can be written
// through a plain pointer: not character vectors.
template <int RTYPE>
SEXP fillParallel(const vector<int> &initial, int nPerms, SEXP elements, int nThreads){
  typedef typename traits::storage_type<RTYPE>::type CTYPE;

  int nLength = (int)initial.size();
  R_xlen_t nr = nPerms;
  R_xlen_t chunk = (nr + nThreads - 1) / nThreads;

  // the serial pass also checks that there are exactly nPerms permutations
  vector<Multicool::Checkpoint> cps;
  Multicool scan(initial);
  R_xlen_t row = 0;

  while(scan.step()){
    if(row % chunk == 0)
      cps.push_back(scan.checkpoint());

    if(row % 1000000 == 0)
      Rcpp::checkUserInterrupt();

    row++;
  }

  if(row != nr)
    Rcpp::stop("the number of permutations generated was not the number expected");

  Matrix<RTYPE> result = no_init(nPerms, nLength);
  CTYPE *out = Rcpp::internal::r_vector_start<RTYPE>(result);
  const CTYPE *el = Rcpp::internal::r_vector_start<RTYPE>(elements);

  // build every block's iterator before starting any threads, so that
  // nothing that might throw happens inside a thread
  int nBlocks = (int)cps.size();
  vector< unique_ptr<Multicool> > iters;
  for(int b = 0; b < nBlocks; b++)
    iters.emplace_back(new Multicool(cps[b], initial));

  auto fillBlock = [&](int b){
    Multicool &it = *iters[b];
    R_xlen_t first = b * chunk;
    R_xlen_t last = std::min(nr, first + chunk);

    for(R_xlen_t r = first; r < last && it.step(); r++){
      // the matrix is column-major, so element (r, col) is at r + col * nr
      R_xlen_t pos = r;
      for(const Multicool::item *y = it.head(); y; y = y->n, pos += nr)
        out[pos] = el[y->v - 1];
    }
  };

  // block 0 is filled by this thread. If a thread cannot be started, its
  // block is filled here too.
  vector<thread> threads;
  vector<int> notStarted;
  for(int b = 1; b < nBlocks; b++){
    try{
      threads.emplace_back(fillBlock, b);
    }catch(...){
      notStarted.push_back(b);
    }
  }

  fillBlock(0);
  for(size_t k = 0; k < notStarted.size(); k++)
    fillBlock(notStarted[k]);

  for(size_t k = 0; k < threads.size(); k++)
    threads[k].join();

  return result;
}

// every permutation as the rows of an nPerms x length matrix. nPerms is the
// multinomial coefficient, which the R code computes exactly. It iterates
// over a fresh object, so the position of mcNextPerm is not disturbed.
// [[Rcpp::export(rng = false)]]
SEXP mcAllPerm(SEXP ptr, int nPerms, SEXP elements, int nThreads){
  const vector<int> &initial = getMC(ptr).getInitialState();

  // starting threads costs more than it saves for small results
  unsigned int nCores = thread::hardware_concurrency();
  if(nCores > 0 && (unsigned int)nThreads > nCores)
    nThreads = (int)nCores;

  if(nThreads > 1 && nPerms >= 100000){
    switch(TYPEOF(elements)){
      case INTSXP:  return fillParallel<INTSXP>(initial, nPerms, elements, nThreads);
      case REALSXP: return fillParallel<REALSXP>(initial, nPerms, elements, nThreads);
      case LGLSXP:  return fillParallel<LGLSXP>(initial, nPerms, elements, nThreads);
      case STRSXP:  break; // R strings cannot be written from several threads
      default:{
        IntegerVector codes = seq_len(initial.back());
        return fillParallel<INTSXP>(initial, nPerms, codes, nThreads);
      }
    }
  }

  Multicool iter(initial);
  SEXP result = PROTECT(fillDispatch(iter, nPerms, elements));

  if(Rf_isNull(result) || Rf_nrows(result) != nPerms || iter.step()){
    UNPROTECT(1);
    Rcpp::stop("the number of permutations generated was not the number expected");
  }

  UNPROTECT(1);
  return result;
}
