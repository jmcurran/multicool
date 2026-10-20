#include <vector>
#include <Rcpp.h>

using namespace Rcpp;
using namespace std;

// generate all integer partitions of n, as ascending compositions
// [[Rcpp::export(rng = false)]]
List generateCompositions(int n){
	// Adapted from Python code and algorithm by
	// Jerome Kelleher (c) 2009
	// https://jeromekelleher.net/category/combinatorics.html
	if(n < 1)
	  Rcpp::stop("n must be a positive integer");

	// collect in a std::vector and convert once at the end: Rcpp::List::push_back
	// copies the whole list each time, which made this quadratic
	vector< vector<int> > A;
	vector<int> a(n+1, 0);

	int k = 1;
	int y = n - 1;

	while(k != 0){
		int x = a[k - 1] + 1;
		k -= 1;
		while(2*x <= y){
			a[k] = x;
			y -= x;
			k += 1;
		}
		int l = k + 1;
		while(x <= y){
			a[k] = x;
			a[l] = y;
			A.push_back(vector<int>(a.begin(), a.begin() + (k + 2)));
			x += 1;
			y -= 1;
		}
		a[k] = x + y;
		y = x + y - 1;
		A.push_back(vector<int>(a.begin(), a.begin() + k + 1));

		if(A.size() % 10000 == 0)
		  Rcpp::checkUserInterrupt();
	}

	return wrap(A);
}
