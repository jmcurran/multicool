#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <vector>


/* Adapted from Aaron Williams multicool.c */
/* Author: Aaron Williams 

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

extern "C" {
	class Multicool{
		struct list_el {
			int v;
			struct list_el * n;
		};
		typedef struct list_el item;

		item *h;
		item *t;
		item *i;

		int *currState;
		int nLength;
		bool bFirst;

	public:
		Multicool(int *x, int nx){
			i = (item *)NULL;
			h = NULL;
			currState = new int[nx];
			nLength = nx;
			bFirst = true;

			for(int ctr = 0; ctr < nx; ctr++){
				t = new item;
				t->v = INT_MAX;
				t->v = x[ctr];
				currState[ctr] = x[ctr];
				t->n = h;
				h = t;

				if(h->n && h->n->v > h->v){
					std::cout << "Error" << std::endl;
				}

				if(ctr+1 == 2){
					i = h;
				}
			}
		};

		~Multicool(void){
			delete [] currState;

			while(t){
				item *next = t->n;
				delete t;
				t = next;
			}
		};

		void setState(item *b){
			item *y;
			y = b;
			int ctr = 0;

			while(y) {
				currState[ctr++] =  y->v;
				y = y->n ;
			}
		}

		bool hasNext(void){
			item *j;
			item *t;
			item *s;

			if(bFirst){
				setState(h);
				bFirst = false;
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
		}

		void getState(int *state){
			for(int ctr = 0; ctr < nLength; ctr++)
				state[ctr] = currState[ctr];
		};
	};



	int test(void){
		int *i  = new int[4];
		i[0] = 1;
		i[1] = 1;
		i[2] = 2;
		i[3] = 2;

		Multicool m(i, 4);

		while(m.hasNext()){
			m.getState(i);

			for(int ctr = 0; ctr < 4; ctr++){
				std::cout << i[ctr];
			}
			std::cout << std::endl;
		}

		return 0;
	}

	std::vector<Multicool*> vpm;


	void MC(int *set, int *nx, int *id){
		vpm.push_back(new Multicool(set, *nx));
		*id = (int)vpm.size() - 1;
	}

	void next(int *set, int *bnext, int *id){
		Multicool *pm = vpm[*id];

		if(pm->hasNext()){
			pm->getState(set);
			*bnext = 1;
		}else{
			pm->getState(set);
			*bnext = 0;
		}
	}
}
