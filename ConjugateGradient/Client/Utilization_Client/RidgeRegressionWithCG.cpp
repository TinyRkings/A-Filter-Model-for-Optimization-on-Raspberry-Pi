# include <cmath>
# include <cstdlib>
# include <cstring>
# include <iomanip>
# include <iostream>
# include <sys/time.h>
# include "cpu_util.h"

struct stats_cpu cur,prev1;
using namespace std;
# include "cg.hpp"

int main ( );
//Implicit Feedback Collaborative Filtering

//Store the matrix to Coo Format
double*MatrixToCoo(int n, int nz_num, int row[], int col[], double** sparseMatrix)
{
	double *a;
  	a = new double[nz_num];
  	for ( int k = 0; k < nz_num; k++ )
  	{
  		row[k] = 0;
    	col[k] = 0;
    	a[k] = 0.0;
  	}
  	int storenum=0;
  	for(int i=0; i< n; i++)
  	{
  		for(int j=0;j<n; j++)
  		{
  			if(sparseMatrix[i][j]!=0)
  			{
  				row[storenum]=i;
  				col[storenum]=j;
  				a[storenum]=sparseMatrix[i][j];
  				storenum++;
  			}
  		}
  	}
  	return a;
}


//matrix size: n*n 500*500,  sparsity: sparsity
int SparseMatrix(int n, double sparsity, double **sparseMatrix)
{
	int nonzero= (int)n*n*sparsity;
	int remaining = nonzero - n;
	
	//diagonal initialization
	for(int i=0; i<n; i++)
	{
		sparseMatrix[i][i]=2.0;
	}
	
	//remaining initialization
	if(remaining%2!=0)
		remaining++;
	for(int i=0; i<remaining; i=i+2)
	{
		int randomrow = rand() % n;
		if(randomrow!=0)
		{
			int randomcolumn = rand()%randomrow;
			if(sparseMatrix[randomrow][randomcolumn]<-0.5)
				i= i-2;
			else
			{
				sparseMatrix[randomrow][randomcolumn] = -1.0;
				sparseMatrix[randomcolumn][randomrow] = -1.0;
			}
		}
		else
			i = i-2;
	}
	return remaining+n;
}


int main ( )
{
  double utilization1=0;
  double utilization2=0;
  double utilization3=0;
  
  int cpu_id =-1;
  double*a;
  int *col;
  int *row;
  double* x1;
  double* x2;
  double* b;
  double *r;
  double r_norm;
  int seed;
  int n=500;
  double sparsity=0.06;//5000 nonzeros	
  while(1)
  {
  for(int ww=0; ww<100; ww++)
  {
  	//generate sparseMatrix
  	double **sparseMatrix = new double*[n];
  	for(int i=0; i< n; i++)
  		sparseMatrix[i]=new double[n];
  	for (int i=0;i<n; i++)
  	{
  		for(int j=0;j<n;j++)
  			sparseMatrix[i][j]=0.0;
  	}
  	int nz_num = SparseMatrix(n, sparsity, sparseMatrix);
  	//printf("nz-num: %d\n", nz_num);
  
  
  	//generate "COO format"
  	read_stat_cpu(&prev1, cpu_id);
  	row = ( int * ) malloc ( nz_num * sizeof ( int ) );
  	col = ( int * ) malloc ( nz_num * sizeof ( int ) );
  	a= MatrixToCoo(n, nz_num, row, col, sparseMatrix);
  	read_stat_cpu(&cur, cpu_id);
	utilization1 = (utilization1 * ww + calc_util_perc(prev1, cur))/(ww+1);
  	//free sparseMatrix
  	for(int i = 0; i < n; ++i)
   		delete [] sparseMatrix[i];
  	delete [] sparseMatrix;
  
  
  	//right hand b
  	//Choose a random solution.
  	seed = 123456789;
  	x1 = r8vec_uniform_01_new ( n, seed );
  	//Compute the corresponding right hand side.
  	b = r8sp_mv ( n, n, nz_num, row, col, a, x1 );
  
  
  	//Call the CG routine.
  	read_stat_cpu(&prev1, cpu_id);
  	x2 = new double[n];
  	for (int i = 0; i < n; i++ )
  	{
    	x2[i] = 1.0;
  	}
  	r8sp_cg ( n, nz_num, row, col, a, b, x2 );
  	read_stat_cpu(&cur, cpu_id);
	utilization2 = (utilization2 * ww + calc_util_perc(prev1, cur))/(ww+1);
	copy_struct(cur, &prev1);
  
  
  	//Compute the residual.
  	r = r8sp_res ( n, n, nz_num, row, col, a, x2, b );
  	r_norm = r8vec_norm ( n, r );
  	read_stat_cpu(&cur, cpu_id);
    utilization3 = (utilization3 * ww + calc_util_perc(prev1, cur))/(ww+1);
  
  }
  printf("100 images t1 utilization:%f us\n", utilization1);
  printf("100 images t2 utilization:%f us\n", utilization2);
  printf("100 images t3 utilization:%f us\n", utilization3);
  cout << "\n";
  utilization1=0;
  utilization2=0;
  utilization3=0; 
  }
  
  //Free memory.
  delete [] a;
  delete [] b;
  delete [] col;
  delete [] r;
  delete [] row;
  delete [] x1;
  delete [] x2;
  return 0;
}
