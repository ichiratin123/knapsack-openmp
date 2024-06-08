#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

void initialize_items(int n, int max, int *weight, int *profit) { //using to create items
    for (int i = 0; i < n; i++) {     //loop for n items
        weight[i] = rand() % max + 1; // random weight from 1 to max value 
        profit[i] = rand() % max + 1; // random profit from 1 to max value
    }
}

void display_items(int n, int *weight, int *profit) { // show the item with weights and profits
    printf("Weights:\n");
    for (int i = 0; i < n; i++) {
        printf("%2d ", weight[i]);
    }
    printf("\nProfits:\n");
    for (int i = 0; i < n; i++) {
        printf("%2d ", profit[i]);
    }
    printf("\n");
}

//display which item it will get and knapsack table
void display_table(int **table, int n, int capacity, int *weights, int *included) { 
    int w = capacity;
    int included_count = 0; // used for count n items
    for (int i = n; i > 0; i--) {// loop for the number of items included
    // If the value from the current item row and weight column isn't equal to the value from above row
        if (table[i][w] != table[i-1][w]) { 
            included[included_count++] = i; // store that item into included array
            w -= weights[i-1]; // reduce current weight by the weight of the included item
        }
    }

    printf("Table:\n");
    for (int i = 0; i <= n; i++) { // loop for number of items (row)
        for (int j = 0; j <= capacity; j++) { // loop for the capacity (column)
            printf("%2d ", table[i][j]); //print the table
        }
        printf("\n");
    }

    // using for loop to print out all the items in included array
    printf("Items: (based on index):\n");
    for (int i = included_count - 1; i >= 0; i--) { 
        printf("%d ", included[i]);
    }
    printf("\n");
}

// function to return the maximum of two integers
int max(int a, int b){
    return a > b ? a: b; // return the larger of a or b by using ternary operator
}

void knapsack(int *weights, int *profits, int n, int capacity, int **dp, int nthreads) {
    // Initialize the DP table
    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            dp[i][w] = 0; //set all values to 0
        }
    }

    double start_time = omp_get_wtime(); // time start
    // Fill the DP table using OpenMP for parallel loops
    printf("Check Threads\n");
    #pragma omp parallel for num_threads(nthreads) schedule(dynamic) // Parallelize the loop with OpenMP
    for (int i = 1; i <= n; i++) { // loop items (row)
        int tid = omp_get_thread_num(); //get threads number to debug
        printf("Thread %d is processing item %d\n", tid, i);
        for (int w = 1; w <= capacity; w++) { // loop capacity (column)
            if (weights[i - 1] <= w) { // check if that item can fit into the knapsack
                // update DP table value by taking maximum of not taking or taking the item
                dp[i][w] = max(dp[i - 1][w], profits[i - 1] + dp[i - 1][w - weights[i - 1]]);
            } else {
                // if item can't fit, inherit the value from the row above
                dp[i][w] = dp[i - 1][w];
            }
        }
    }
    double end_time = omp_get_wtime();  // time stop
    printf("Execution Time: %f seconds\n", end_time - start_time);
}

int main(int argc, char *argv[]) {
    // Default parameters (n: number of items, c: capacity, m: maximum values, nthreads: number of threads)
    int n = 0, c = 0, m = 50, nthreads = 3; 
    if (argc < 3) {
        printf("Usage: %s <number of items> <knapsack capacity> [number of threads] [max item value]\n", argv[0]);
        return 1;
    }

    n = atoi(argv[1]);
    c = atoi(argv[2]);
    if (argc > 3) nthreads = atoi(argv[3]);
    if (argc > 4) m = atoi(argv[4]);

    if (n <= 0 || c <= 0 || m <= 0 || nthreads <= 0) {
        fprintf(stderr, "Error: Invalid input values. All values must be greater than 0.\n");
        return 1;
    }

    int *weights = (int *)malloc(n * sizeof(int));
    int *profits = (int *)malloc(n * sizeof(int));
    int *included = (int *)malloc(n * sizeof(int));
    

    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = (int *)malloc((c + 1) * sizeof(int));
    }

    initialize_items(n, m, weights, profits);

    display_items(n, weights, profits);

    knapsack(weights, profits, n, c, dp, nthreads);

    display_table(dp, n, c, weights, included);

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);
    free(weights);
    free(profits);
    free(included);

    return 0;
}
