#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <vector>
#include "multinomial.h"

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

// [[Rcpp::plugins("cpp11")]]

class Multicool{
  struct list_el {
    int v;
    struct list_el * n;
  };
  typedef struct list_el item;
  
  item *h;
  item *t;
  item *i;
  
  int *m_pnInitialState; // stored so that the current state can be reset to the initial state
  int *m_pnCurrState;
  
  int m_nLength;
  bool m_bFirst;

public:
	// constructor
  Multicool(IntegerVector x){
		int nx = (int)x.size();
		
    i = (item *)NULL;
    h = NULL;
    m_pnInitialState = new int[nx];
    m_pnCurrState = new int[nx];
    m_nLength = nx;
    m_bFirst = true;
    
    for(int ctr = 0; ctr < nx; ctr++){
      t = new item;
      t->v = INT_MAX;
      t->v = x[ctr];
      m_pnCurrState[ctr] = x[ctr];
      m_pnInitialState[ctr] = x[ctr];
      t->n = h;
      h = t;
      
      if(h->n && h->n->v > h->v){
        // std::cout << "Error" << std::endl;
        // should programme some sensible action here but I don't know what
      }
      
      if(ctr+1 == 2){
        i = h;
      }
      
      /*Rprintf("Step %d\n--------\n", ctr + 1);
      debugPrint();
      Rprintf("--------\n");*/
    }
  };
	
	//destructor
  ~Multicool(void){
    // Rprintf("Destructor called\n");
    delete [] m_pnInitialState;
    delete [] m_pnCurrState;
    
    while(t){
      item *next = t->n;
      delete t;
      t = next;
    }
  };
  
private:
  
  void debugPrint(void){
    char strPtr[] = {'h', 't', 'i'};
    
    for(auto ctr = 0; ctr  < 3; ctr++){
      item *p; 
      switch(ctr){
        case 0:
          p = h;
          break;
        case 1:
          p = t;
          break;
        case 2:
          p = i;
          break;
      }
      
      if(p != NULL){
        Rprintf("item %c->v: %d\n",strPtr[ctr] , p->v);
        if(p->n != NULL){
          Rprintf("item %c->n: %p\n",strPtr[ctr] , (void*)(p->n));
        }else{
          Rprintf("item %c->n: NULL\n",strPtr[ctr]);
        }
      }else{
        Rprintf("%c = NULL\n",strPtr[ctr] );
      }
    }
    
    Rprintf("m_nLength %d\n", m_nLength);
    Rprintf("m_bFirst %d\n", (int)m_bFirst);
    
    Rprintf("m_pnInitialState: ");
    for(auto ctr = 0; ctr < m_nLength; ctr++){
      Rprintf("%d ", m_pnInitialState[ctr]);
    }
    Rprintf("\n");
    
    Rprintf("m_pnCurrState: ");
    for(auto ctr = 0; ctr < m_nLength; ctr++){
      Rprintf("%d ", m_pnCurrState[ctr]);
    }
    Rprintf("\n");
  }
  
  void print(void){
    Rprintf("item h->v: %d\n", h->v);
    Rprintf("item t->v: %d\n", t->v);
    Rprintf("item i->v: %d\n", i->v);
    Rprintf("m_nLength %d\n", m_nLength);
    Rprintf("m_bFirst %d\n", (int)m_bFirst);
    
    int i;
    
    Rprintf("m_pnInitialState: ");
    for(i = 0; i < m_nLength; i++){
      Rprintf("%d ", m_pnInitialState[i]);
    }
    Rprintf("\n");
    
    Rprintf("m_pnCurrState: ");
    for(i = 0; i < m_nLength; i++){
      Rprintf("%d ", m_pnCurrState[i]);
    }
    Rprintf("\n");
  }
  
   void reset(void){
    delete [] m_pnCurrState;
    
    while(t){
      item *next = t->n;
      delete t;
      t = next;
    }
    
    i = (item *)NULL;
    h = NULL;
    m_pnCurrState = new int[m_nLength];
    m_bFirst = true;
    
    for(int ctr = 0; ctr < m_nLength; ctr++){
      t = new item;
      t->v = INT_MAX;
      t->v = m_pnInitialState[ctr];
      m_pnCurrState[ctr] = m_pnInitialState[ctr];
      t->n = h;
      h = t;
      
      if(h->n && h->n->v > h->v){
        //std::cout << "Error" << std::endl;
        // should programme some sensible action here but I don't know what
      }
      
      if(ctr+1 == 2){
        i = h;
      }
    }
    
  }
  
  void setState(item *b){
    item *y;
    y = b;
    int ctr = 0;
    
    while(y) {
      m_pnCurrState[ctr++] =  y->v;
      y = y->n ;
    }
  }

 vector<int> getState(void){
		vector<int> vState;
    
		for(int ctr = 0; ctr < m_nLength; ctr++)
			vState.push_back( m_pnCurrState[ctr] );

		return vState;
	};
  
public:
  
  List allPerm(void){
    this->reset();
    vector<int> set = this->getInitialState();
    
    auto lResult = vector<vector<int>>();
    
    while( this->hasNext()){
      if (lResult.size() % 1000 == 0) Rcpp::checkUserInterrupt();
      lResult.push_back( this->getState() );
    }
    
    return wrap(lResult);
  };
  
  int getLength(void){
    return m_nLength;
  };
  
  vector<int> getInitialState(void){
    vector<int> vSet(m_pnInitialState, m_pnInitialState + m_nLength);
    return vSet;
  };
  
  bool hasNext(void){
    item *j;
    item *t;
    item *s;
    
    // Rprintf("\n--------\n");
    // debugPrint();
    // Rprintf("--------\n");
    
    if(m_bFirst){
      setState(h);
      m_bFirst = false;
      return true;
    }else{
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
        j = i->n;
        h = t;
        setState(h);
        return true;
      }else{
        return false;
      }
    }
  };
  
  List nextPerm(void){
    List lhs;

    lhs["b"] = hasNext() ? 1 : 0;
    lhs["set"] = as<IntegerVector>(wrap(getState()));
    
    return lhs;
  };
  
  
};

RCPP_MODULE(Multicool) {
  using namespace Rcpp;
  
  class_<Multicool>( "Multicool")
    .constructor<IntegerVector>("Standard constructor")
    .method("allPerm", &Multicool::allPerm)
    .method("set", &Multicool::getInitialState)
    .method("length", &Multicool::getLength)
    .method("hasNext", &Multicool::hasNext)
    .method("nextPerm", &Multicool::nextPerm)
  ;
}


//	int test(void){
//		int *i  = new int[4];
//		i[0] = 1;
//		i[1] = 1;
//		i[2] = 2;
//		i[3] = 2;
//
//		Multicool m(i, 4);
//
//		while(m.hasNext()){
//			m.getState(i);
//
//			for(int ctr = 0; ctr < 4; ctr++){
//				std::cout << i[ctr];
//			}
//			std::cout << std::endl;
//		}
//
//		return 0;
//	}

// multinomial coefficient
// [[Rcpp::export]]
NumericVector multinomCoeff(NumericVector x, bool useDouble){
  int nx = x.size();
  multinomial::SVI v(nx);
  int i;
  
  for(i = 0; i < nx; i++){
    v.at(i) = x[i];
  }
  
  if(useDouble){
    double u = multinomial::multi<double>(v);
    NumericVector r = NumericVector::create(u);
  
    return r;
  }//else
  unsigned long u = multinomial::multi<unsigned long>(v);
  NumericVector r = NumericVector::create(u);
    
  return r;
}

