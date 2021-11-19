#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include "simd.intrinsics.cpp"

#define NUMTRIES 50

void Non_SIMD_Mult(float* a, float* b, float* c, int len)
{
	for (int i = 0; i < len; i++)
	{
		c[i] = a[i] * b[i];
	}
}

float Non_SIMD_MulSum(float* a, float* b, int len)
{
	float sum = 0.;

	for (int i = 0; i < len; i++)
	{
		sum += a[i] * b[i];
	}

	return sum;
}

int main() {
#ifndef _OPENMP
	fprintf(stderr, "No OpenMP support!\n");
	return 1;
#endif
	for (int Array_size = 1024; Array_size <= 8 * 1024 * 1024; Array_size = Array_size * 2) {
		double peakMegaMults1 = 0.;
		double peakMegaMults2 = 0.;
		double peakMegaMults3 = 0.;
		double peakMegaMults4 = 0.;
		float* a = (float*)malloc(Array_size * sizeof(float));
		float* b = (float*)malloc(Array_size * sizeof(float));
		float* c = (float*)malloc(Array_size * sizeof(float));
		printf("Array Size: %d\n", Array_size);
		for (int i = 0; i < NUMTRIES; i++) {
			double startTimer1 = omp_get_wtime();
			SimdMul(a, b, c, Array_size);
			double endTimer1 = omp_get_wtime();
			double SimdMulSpeed = (float)Array_size / (endTimer1 - startTimer1) / 1000000.0;
			if (SimdMulSpeed > peakMegaMults1)
				peakMegaMults1 = SimdMulSpeed;

			double startTimer2 = omp_get_wtime();
			Non_SIMD_Mult(a, b, c, Array_size);
			double endTimer2 = omp_get_wtime();
			double NonSimdMulSpeed = (float)Array_size / (endTimer2 - startTimer2) / 1000000.0;
			if (NonSimdMulSpeed > peakMegaMults2)
				peakMegaMults2 = NonSimdMulSpeed;

			double startTimer3 = omp_get_wtime();
			SimdMulSum(a, b, Array_size);
			double endTimer3 = omp_get_wtime();
			double SimdMulSumSpeed = (float)Array_size / (endTimer3 - startTimer3) / 1000000.0;
			if (SimdMulSumSpeed > peakMegaMults3)
				peakMegaMults3 = SimdMulSumSpeed;

			double startTimer4 = omp_get_wtime();
			Non_SIMD_MulSum(a, b, Array_size);
			double endTimer4 = omp_get_wtime();
			double NonSimdMulSumSpeed = (float)Array_size / (endTimer4 - startTimer4) / 1000000.0;
			if (NonSimdMulSumSpeed > peakMegaMults4)
				peakMegaMults4 = NonSimdMulSumSpeed;

		}
		printf("SimdMulMegaMults = %f\n", peakMegaMults1);
		printf("NonSimdMulMegaMults = %f\n", peakMegaMults2);
		printf("SimdMulSumMegaMults = %f\n", peakMegaMults3);
		printf("NonSimdMulSumMegaMults = %f\n", peakMegaMults4);
		printf("Speed-up for Mul: %f\n", peakMegaMults1 / peakMegaMults2);
		printf("Speed-up for MulSum: %f\n", peakMegaMults3 / peakMegaMults4);
		printf("----------------------------------------------\n");
		FILE* fp;
		errno_t err;
		err = fopen_s(&fp, "result.csv", "a");
		if (err == 0) {
			fprintf(fp, "%d, %f, %f, %f, %f, %f ,%f\n", Array_size, peakMegaMults1, peakMegaMults2, peakMegaMults3, peakMegaMults4, peakMegaMults1 / peakMegaMults2, peakMegaMults3 / peakMegaMults4);
		}
		fclose(fp);
		free(a);
		free(b);
		free(c);
	}
}