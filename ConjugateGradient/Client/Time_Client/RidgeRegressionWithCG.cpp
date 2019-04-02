# include <cmath>
# include <cstdlib>
# include <cstring>
# include <iomanip>
# include <iostream>
#include <sys/time.h>

using namespace std;

# include "cg.hpp"

int main ( );
//Implicit Feedback Collaborative Filtering

//double R8SP_DIF2[NZ_NUM]
void r8sp_cg_test ( );//'Coo format'

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
  double t1, t2,t3;
  struct timeval t1_start, t1_end, t2_start, t2_end, t3_start,t3_end;
  t1=0;
  t2=0;
  t3=0;
  
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
  	gettimeofday(&t1_start,NULL);
  	row = ( int * ) malloc ( nz_num * sizeof ( int ) );
  	col = ( int * ) malloc ( nz_num * sizeof ( int ) );
  	a= MatrixToCoo(n, nz_num, row, col, sparseMatrix);
  	gettimeofday(&t1_end,NULL);
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
  	gettimeofday(&t2_start,NULL);
  	x2 = new double[n];
  	for (int i = 0; i < n; i++ )
  	{
    	x2[i] = 1.0;
  	}
  	r8sp_cg ( n, nz_num, row, col, a, b, x2 );
  	gettimeofday(&t2_end,NULL);
  
  
  	//Compute the residual.
  	r = r8sp_res ( n, n, nz_num, row, col, a, x2, b );
  	r_norm = r8vec_norm ( n, r );
  	gettimeofday(&t3_end,NULL);
  
  	//  Report.
  	t1 = t1 + 1000000*(t1_end.tv_sec-t1_start.tv_sec)+t1_end.tv_usec-t1_start.tv_usec; //us
  	t2 = t2 + 1000000*(t2_end.tv_sec-t2_start.tv_sec)+t2_end.tv_usec-t2_start.tv_usec; //us
  	t3 = t3 + 1000000*(t3_end.tv_sec-t2_end.tv_sec)+t3_end.tv_usec-t2_end.tv_usec; //us
  }
  printf("100 images t1 used time:%f us\n", t1);
  printf("100 images t2 used time:%f us\n", t2);
  printf("100 images t3 used time:%f us\n", t3);
  cout << "\n";
  t1=0;
  t2=0;
  t3=0; 
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



void r8sp_cg_test ( )
{
  double t1, t2,t3;
  struct timeval t1_start, t1_end, t2_start, t2_end, t3_start,t3_end;
  t1=0;
  t2=0;
  t3=0;

  double *a;
  double *b;
  int *col;
  double e_norm;
  int i;
  int n;
  int nz_num;
  double *r;
  double r_norm;
  int *row;
  int seed;
  double *x1;
  double *x2;

  cout << "\n";
  cout << "R8SP_CG_TEST\n";
  cout << "  R8SP_CG applies CG to an R8SP matrix.\n";

  n = 500;

  nz_num = 9 * n;
  row = ( int * ) malloc ( nz_num * sizeof ( int ) );
  col = ( int * ) malloc ( nz_num * sizeof ( int ) );
  
  while(1)
  {
  for(int ww=0; ww<100; ww++)
  {
  //Let A be the -1 2 -1 matrix.
  gettimeofday(&t1_start,NULL);
	
    a = r8sp_9nznum ( n, n, nz_num, row, col );
  //a = r8sp_dif2 ( n, n, nz_num, row, col );
  //a = r8sp_2nznum ( n, n, nz_num, row, col );
  //a = r8sp_1nznum ( n, n, nz_num, row, col );
  gettimeofday(&t1_end,NULL);


  //Choose a random solution.
  seed = 123456789;
  x1 = r8vec_uniform_01_new ( n, seed );

  //Compute the corresponding right hand side.
  b = r8sp_mv ( n, n, nz_num, row, col, a, x1 );

  //Call the CG routine.
  gettimeofday(&t2_start,NULL);
  x2 = new double[n];
  for ( i = 0; i < n; i++ )
  {
    x2[i] = 1.0;
  }
  r8sp_cg ( n, nz_num, row, col, a, b, x2 );
  gettimeofday(&t2_end,NULL);
  
  
  //Compute the residual.
  r = r8sp_res ( n, n, nz_num, row, col, a, x2, b );
  r_norm = r8vec_norm ( n, r );
  gettimeofday(&t3_end,NULL);
  
  //Compute the error.
  e_norm = r8vec_norm_affine ( n, x1, x2 );
  
  
  //  Report.
  t1 = t1 + 1000000*(t1_end.tv_sec-t1_start.tv_sec)+t1_end.tv_usec-t1_start.tv_usec; //us
  t2 = t2 + 1000000*(t2_end.tv_sec-t2_start.tv_sec)+t2_end.tv_usec-t2_start.tv_usec; //us
  t3 = t3 + 1000000*(t3_end.tv_sec-t2_end.tv_sec)+t3_end.tv_usec-t2_end.tv_usec; //us
  }
  printf("100 images t1 used time:%f us\n", t1);
  printf("100 images t2 used time:%f us\n", t2);
  printf("100 images t3 used time:%f us\n", t3);
  cout << "\n";
  t1=0;
  t2=0;
  t3=0; 
  }
  /*
  cout << "\n";
  cout << "  Number of variables N = " << n << "\n";
  cout << "  Norm of residual ||Ax-b|| = " << r_norm << "\n";
  cout << "  Norm of error ||x1-x2|| = " << e_norm << "\n";
  */



  //Free memory.
  delete [] a;
  delete [] b;
  delete [] col;
  delete [] r;
  delete [] row;
  delete [] x1;
  delete [] x2;

  return;
}