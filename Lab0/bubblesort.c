#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000000   // 1 million elements

// Swap two elements
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Bubble Sort function
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;

        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                swapped = 1;
            }
        }

        // Stop early if already sorted
        if (!swapped)
            break;
    }
}

int main() {
    int *arr = (int *)malloc(SIZE * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed\n");
        return 1;
    }

    srand(time(NULL));

    // Fill array with random integers
    for (int i = 0; i < SIZE; i++) {
        arr[i] = rand();
    }

    // Measure execution time
    clock_t start, end;
    start = clock();

    bubbleSort(arr, SIZE);

    end = clock();

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Bubble Sort of %d elements took %.2f seconds.\n", SIZE, time_taken);

    free(arr);
    return 0;
}