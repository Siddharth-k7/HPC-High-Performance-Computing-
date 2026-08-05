/*
 * HPC26 / T0 - Performance Profiling and Optimization using GPROF and PERF
 *
 * Baseline algorithm : Bubble Sort  - O(n^2)
 * Optimised algorithm: Quick Sort   - O(n log n) average
 *
 * Usage:
 *     ./mysort                 -> both sorts, N = 18250
 *     ./mysort <n>             -> both sorts on n elements
 *     ./mysort <n> quick       -> quick sort only (use for n = 1000000)
 *     ./mysort <n> bubble      -> bubble sort only
 *     ./mysort scaling         -> input-size study
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEFAULT_SIZE 18250   /* ~50 years of daily temperature readings */
#define TEMP_MIN    -10.0f
#define TEMP_MAX     45.0f

void generate_data(float *arr, int n)
{
    for (int i = 0; i < n; i++) {
        float r = (float)rand() / (float)RAND_MAX;
        arr[i] = TEMP_MIN + r * (TEMP_MAX - TEMP_MIN);
    }
}

void swap(float *a, float *b)
{
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

int is_sorted(const float *arr, int n)
{
    for (int i = 1; i < n; i++)
        if (arr[i - 1] > arr[i])
            return 0;
    return 1;
}

/* ---------------- Baseline: Bubble Sort ---------------- */
void bubbleSort(float *arr, int n)
{
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

/* ---------------- Optimised: Quick Sort ---------------- */
int partition(float *arr, int low, int high)
{
    float pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quickSort(float *arr, int low, int high)
{
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

/* clock() is too coarse for Quick Sort at small N;
   clock_gettime(CLOCK_MONOTONIC) gives nanosecond resolution. */
static double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

static double run_bubble(const float *master, int n)
{
    float *a = (float *)malloc((size_t)n * sizeof(float));
    if (!a) { fprintf(stderr, "malloc failed\n"); exit(1); }
    memcpy(a, master, (size_t)n * sizeof(float));

    double start = now_sec();
    bubbleSort(a, n);
    double end = now_sec();

    double t = end - start;
    printf("  Bubble Sort : %10.6f s   (sorted = %s)\n",
           t, is_sorted(a, n) ? "yes" : "NO");
    free(a);
    return t;
}

static double run_quick(const float *master, int n)
{
    float *a = (float *)malloc((size_t)n * sizeof(float));
    if (!a) { fprintf(stderr, "malloc failed\n"); exit(1); }
    memcpy(a, master, (size_t)n * sizeof(float));

    double start = now_sec();
    quickSort(a, 0, n - 1);
    double end = now_sec();

    double t = end - start;
    printf("  Quick Sort  : %10.6f s   (sorted = %s)\n",
           t, is_sorted(a, n) ? "yes" : "NO");
    free(a);
    return t;
}

static void run_size(int n, int do_bubble, int do_quick)
{
    float *master = (float *)malloc((size_t)n * sizeof(float));
    if (!master) { fprintf(stderr, "malloc failed for n=%d\n", n); exit(1); }

    generate_data(master, n);
    printf("N = %d\n", n);
    if (do_bubble) run_bubble(master, n);
    if (do_quick)  run_quick(master, n);
    printf("\n");

    free(master);
}

int main(int argc, char **argv)
{
    srand(42);   /* fixed seed -> identical data across all builds */

    if (argc > 1 && strcmp(argv[1], "scaling") == 0) {
        int sizes[] = {1000, 2000, 4000, 8000, 16000, 18250, 32000, 64000};
        int k = (int)(sizeof(sizes) / sizeof(sizes[0]));
        printf("=== Input-size scaling study ===\n\n");
        for (int i = 0; i < k; i++)
            run_size(sizes[i], 1, 1);
        return 0;
    }

    int n = (argc > 1) ? atoi(argv[1]) : DEFAULT_SIZE;
    if (n <= 1) { fprintf(stderr, "Invalid size\n"); return 1; }

    int do_bubble = 1, do_quick = 1;
    if (argc > 2) {
        if (strcmp(argv[2], "quick") == 0)  do_bubble = 0;
        if (strcmp(argv[2], "bubble") == 0) do_quick  = 0;
    }

    printf("=== Sorting simulated daily temperature data ===\n\n");
    run_size(n, do_bubble, do_quick);
    return 0;
}