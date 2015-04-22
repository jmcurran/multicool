#include <vector>
#include <Rcpp.h>

using namespace Rcpp;
using namespace std;

// generate all combinations
// [[Rcpp::export]]
List generateCompositions(int n){
	// Adapted from Python code and algorithm by
	// Jerome Kelleher (c) 2009
	// http://jeromekelleher.net/partitions.php
	List A;
	vector<int> a(n+1);
	
	fill(a.begin(), a.end(), 0);
	
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
			/*for(int i = 0; i < k + 2; i++)
				cout << a[i] << ' '; */
			A.push_back(vector<int>(a.begin(), a.begin() + (k + 2)));
			//cout << endl;
			x += 1;
			y -= 1;
		}
		a[k] = x + y;
		y = x + y - 1;
		/*for(int i = 0; i < k + 1; i++)
			cout << a[i] << ' ';
		 cout << endl;*/
		A.push_back(vector<int>(a.begin(), a.begin() + k + 1));
	}
	
	return A;
}

// Calculate Stirling numbers of the second kind S2(n, k)
// [[Rcpp::export]]
long Stirling2C(int n, int k){
  if((n == 0 && k == 0) || (n > 0 && k == 1) || (n > 0 && n == k))
    return 1;
    
  if((n == 0 || k == 0)) // n = k = 0 should get caught by the previous if
    return 0;
    
  return k * Stirling2C(n - 1, k) + Stirling2C(n - 1, k - 1);
}

// Calculate Bell numbers
// [[Rcpp::export]]
long BellC(int n){
  long sum = 0;
  
  for(int k = 1; k <= n; k++){
    sum += Stirling2C(n, k);
  }
  
  return sum;
}


