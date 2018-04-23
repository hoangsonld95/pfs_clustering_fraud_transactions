#include "new_online_lib.h"


double feedData(FILE *f_batch) {

	int i,j;
	clock_t start, end;

	start = clock();

	X = (double**)realloc(X, (N+P)*sizeof(double*));

	for (i = N; i < (N + P); i++) {
		X[i] = (double*)malloc(D*sizeof(double));		
		for (j = 0; j < D; j++)	{
			fscanf(f_batch, "%lf\t", &X[i][j]);
			//printf("%lf\t", X[i][j]);
		}
		fscanf(f_batch, "\n");
		//printf("\n");
	}

	// Normalize feed data 
	for (i = N; i < (N+P); i++)	{
		for (j = 0; j < D; j++)	{
			X[i][j] = (X[i][j] - minX) / (maxX - minX);
		}
	}

	end = clock();

	return ((double)(end - start)/ CLOCKS_PER_SEC);

}

double takeOptimizationU(double Q) {

	return 1.00*(Q/2.0);

}

double takeOptimizationT(double u, double Q) {

	return exp((Q/u) -(2+log(2)));

}

double takeOptimizationW(double u, double Q) {

	return (2.0 - Q/u);

}

double optimizeMembership() {

	int i,j,k,p;
	double sum, Q, sumT, A;
	double temp;

	U = (double**)realloc(U, (N+P)*sizeof(double*));
	T = (double**)realloc(T, (N+P)*sizeof(double*));
	W = (double**)realloc(W, (N+P)*sizeof(double*));


	for (k = 0; k < N; k++)	{
		
		for (i = 0; i < C; i++) {
			
			Q = U[k][i] * (2 - W[k][i]);

			U[k][i] = takeOptimizationU(Q);
			T[k][i] = takeOptimizationT(U[k][i], Q);
			W[k][i] = takeOptimizationW(U[k][i], Q);

		}

	}


	for (k = N; k < (N+P); k++)	{
		
		U[k] = (double*)malloc(C*sizeof(double));
		T[k] = (double*)malloc(C*sizeof(double));
		W[k] = (double*)malloc(C*sizeof(double));

		sum = 0.0;

		for (j = 0; j < C; j++)	{
			
			sum += calculateDistance(k,j);

		}

		for (j = 0; j < C; j++)	{
			
			temp = calculateDistance(k, j);
			Q = temp/sum;

			U[k][j] = takeOptimizationU(Q);
			T[k][j] = takeOptimizationT(U[k][j], Q);
			W[k][j] = takeOptimizationW(U[k][j], Q);

		}

	}

}


int main(int argc, char const *argv[])
{
	char fileName[100];
	char fileOutput[100];
	char fileBatch[100];

	FILE *f_batch;
	FILE *f_output;

	int batch;
	int iters;

	double ifv, entropy;
	double db;
	double silhouette;
	double exec_time;

	strcpy(fileName, argv[1]);

	input(fileName);

	strcpy(fileBatch, "batch_source.txt");
	f_batch = fopen(fileBatch, "r");

	strcpy(fileOutput, "new_online_clustering_result_2.txt");

	FEED_TIME = 5;
	P = 5;

	batch = 0;

	initMemForV();

	PFS(&iters);
	printf("IFV1: %lf\n", IFV_PFS());

	do {
		iters = 0;
		exec_time = 0;
		// sleep(5);
		exec_time += feedData(f_batch);
		exec_time += optimizeMembership();
		exec_time += PFS_Optimize(&iters);
		N = N + P;
		ifv = IFV_PFS();
		db = DB_PFS();
		

		printf("iters: %d\n", iters);
		printf("exec time: %lf\n", exec_time);
		printf("ifv: %lf\n", ifv);
		
		output(fileOutput, batch, exec_time, iters, ifv, db, entropy);
		batch++;
		
	}while(!feof(f_batch));	
	
	
	return 0;
}