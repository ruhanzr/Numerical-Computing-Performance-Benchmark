/* 
This program compares different C matrix multiplication implementations,
including loop-order variations and blocked/tiled multiplication. It uses
timing data and checksum validation to compare correctness and runtime.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define SIZE 400
#define BLOCKSIZE 20
int main() {

    int seed; // Declare the seed variable
    int matrixA[SIZE][SIZE];
    int matrixB[SIZE][SIZE];
    int matrixC[SIZE][SIZE] = {0};
    int trials = 5;
    double timeSpent = 0.0;
    int getSeed();
    void fillMatrix(int[][SIZE]);
    void basicMultiply(int[][SIZE], int[][SIZE], int[][SIZE]);
    void optimizedMultiply(int[][SIZE], int[][SIZE], int[][SIZE]);
    void blockedMultiply(int[][SIZE], int[][SIZE], int[][SIZE]);
    void printMatrix(int[][SIZE]);
    long long checkSum(int[][SIZE]);
    void clearMatrix(int [][SIZE]);

    // seed = getSeed();
    // srand(seed);
    srand(1);
    fillMatrix(matrixA);
    fillMatrix(matrixB);

    // Looping Test Trials
    for (int t = 1; t <= trials; t++) {
        clearMatrix(matrixC);
        clock_t start = clock();
        optimizedMultiply(matrixA, matrixB, matrixC);
        clock_t end = clock();
        double trialTime = (double)(end - start) / CLOCKS_PER_SEC;
        timeSpent += trialTime;
        printf("Trial %d: %f seconds\n", t, trialTime);
    }

    printf("Average time: %f seconds\n", timeSpent / trials);

    printf("Matrix size: %dx%d\n", SIZE, SIZE);
    
    // Use checksum for matrices greater than 5x5 and print those smaller
    if (SIZE > 5) {
        printf("Checksum: %lld\n", checkSum(matrixC));
    }
    if (SIZE <= 5) {
        printf("\nMatrix A: \n");
        printMatrix(matrixA);
        printf("\nMatrix B: \n");
        printMatrix(matrixB);
        printf("\nMatrix C: \n");
        printMatrix(matrixC);
    }
    // printf("Loop Order IJK");
    return 0;
}

// Getting seed value from user
int getSeed() {
    int seed;
    
    do { 
        printf("Please enter a positive integer for the seed value: ");
        scanf("%d", &seed);
        if (seed <= 0) {
            printf("Invalid input. Seed value must be a positive integer.\n");
        }
    } while (seed <= 0); // Loop until a valid seed value is entered
    return seed;
}

// Filling a matrix with random numbers
void fillMatrix(int matrix[][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
         for (int j = 0; j < SIZE; j++) {
             matrix[i][j] = rand() % 10; // Fill matrix with random values between 0 and 9
         }
    }
}

// Performing basic matrix multiplication
void basicMultiply(int matrixA[][SIZE], int matrixB[][SIZE], int matrixC[][SIZE]) {
    int sum;
    int total = 0;
    for (int k = 0; k < SIZE; k++) {
        for (int i = 0; i < SIZE; i++) {
            total = 0;
            for (int j = 0; j < SIZE; j++) {
                sum = (matrixA[i][j] * matrixB[j][k]);
                total += sum;
            }
            matrixC[i][k] = total;
        }
    }

}

// Printing matrix to user
void printMatrix(int matrix[][SIZE]) {
    int row;
    int col;

    for (row = 0; row < SIZE; row++) {
        for (col = 0; col < SIZE; col++) {
            printf("%d ", matrix[row][col]);
        }
        printf("\n");
    }
}

// Checking sum of matrix elements to determine if output is being used correctly
long long checkSum(int matrix[][SIZE]) {
    int i;
    int j;
    int value;
    long long sum = 0;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            value = matrix[i][j];
            sum += value;
        }
    }
    return(sum);
}

// Performing optimized matrix multiplication
void optimizedMultiply(int matrixA[][SIZE], int matrixB[][SIZE], int matrixC[][SIZE]) {
    int sum;
    int total = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            total = 0;
            for (int k = 0; k < SIZE; k++) {
                sum = (matrixA[j][k] * matrixB[k][i]);
                total += sum;
            }
            matrixC[j][i] = total;
        }
    }
}

// Performing blocked matrix multiplication
void blockedMultiply(int matrixA[][SIZE], int matrixB[][SIZE], int matrixC[][SIZE]) {
    int sum;
    for (int ii = 0; ii < SIZE; ii += BLOCKSIZE) {
        for (int jj = 0; jj < SIZE; jj += BLOCKSIZE) {
            for (int kk = 0; kk < SIZE; kk += BLOCKSIZE) {
                for (int i = ii; i < (ii + BLOCKSIZE); i++) {
                    for (int j = jj; j < (jj + BLOCKSIZE); j++) {
                        int total = matrixC[i][j];
                        for (int k = kk; k < (kk + BLOCKSIZE); k++) {
                        sum = (matrixA[i][k] * matrixB[k][j]);
                        total += sum;
                        }
                        matrixC[i][j] = total;
                    }
                } 
            }
        }
    }
}

// Clearing matrix contents
void clearMatrix(int matrix[][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix[i][j] = 0;
        }
    }
}