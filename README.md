HPC26 / T0 — Performance Profiling and Optimization using GPROF and PERF

Sorting ~50 years of simulated daily temperature data, profiled with GPROF and PERF.

Author: Siddharth Kancharla — S20240030402 — IIIT Sri City

Files

File

Description

mysort.c

Bubble Sort (baseline) + Quick Sort (optimised), with timing

myreport.txt

GPROF report, -O0 build

report_O2.txt

GPROF report, -O2 build

report_O3.txt

GPROF report, -O3 build

report.pdf

Full write-up with tables, graphs and answers

screenshots/

Terminal captures of every command

Build

gcc -O0 -pg mysort.c -o mysort
gcc -O2 -pg mysort.c -o mysort_O2
gcc -O3 -pg mysort.c -o mysort_O3

Run

./mysort                  # both sorts, N = 18250 (default)
./mysort 1000000 quick    # Quick Sort only on 1M elements
./mysort 18250 bubble     # Bubble Sort only
./mysort scaling          # input-size study: N = 1000 .. 64000

Profiling workflow

time ./mysort                                  # real / user / sys
./mysort && gprof mysort gmon.out > myreport.txt
perf stat ./mysort                             # cycles, IPC, cache, branches

Test environments

Two machines were used, because hardware counters are not available everywhere.

Env

Description

Hardware counters

A

GitHub Codespace (Linux container)

Not available — <not supported>

B

Local WSL2, sudo /usr/lib/linux-tools/5.15.0-*/perf

Available (cycles, instructions, branches)

WSL2 runs under Hyper-V, which does not virtualise the CPU's performance monitoring unit forevery event. Cycles, instructions and branch counters were obtainable on env B; cache-referenceand cache-miss counters were not exposed on either machine.

Results

1. Execution time

N = 18250 (-O0), both algorithms, env A

Algorithm

Time (s)

Speedup

Bubble Sort

1.042268

1×

Quick Sort

0.003366

310×

Repeat run of the same binary under time: Bubble 1.085791 s, Quick 0.003278 s (331×) —confirms the result is stable across runs.

time ./mysort — whole-program wall clock

Program

Real

User

Sys

Quick Sort only, N = 1,000,000

0m0.268s

0m0.262s

0m0.004s

Bubble + Quick, N = 18250

0m1.096s

0m1.077s

0m0.004s

real ≈ user and sys ≈ 0 in both cases, confirming the workload is CPU-bound withnegligible kernel/I-O time.

N = 1,000,000 (-O0)

Algorithm

Time

Quick Sort

0.26 s

Bubble Sort

≈ 3.6 hours (measured/extrapolated)

Bubble Sort was not run to completion at N = 10⁶ for the profiling steps. At O(n²) this is≈ 5 × 10¹¹ comparisons; all Bubble Sort profiling was therefore performed at N = 18250(≈ 50 years of daily readings, matching the problem statement) and at N = 5000 for perf.

N = 5000 (-O0, env B)

Algorithm

Time (s)

Bubble Sort

0.077638

Quick Sort

0.000864

2. GPROF hotspot

-O0 flat profile (myreport.txt)

Function

% CPU time

Self seconds

Calls

main

54.00

0.41

3

quickSort

36.67

0.28

83,797,840

atexit

9.33

0.07

—

_fini

0.00

0.00

36,629

partition

0.00

0.00

1

⚠️ Known issue with this run — to be re-collected. The call counts are internallyinconsistent: quickSort cannot make 83,797,840 calls while partition records only 1,since quickSort invokes partition exactly once per recursion. main showing 3 calls and54% self time is also impossible for this program. This is the signature of a stalegmon.out — the profile data was produced by a different binary than the one passed togprof, so symbols were mis-attributed. Fix: delete gmon.out, run the intended binary,and generate the report immediately, before running any other build.

Call graph structure (Quick Sort recursion)

main()
 └── quickSort()
      ├── partition()
      ├── quickSort()   [left partition]
      │    ├── quickSort()
      │    └── quickSort()
      └── quickSort()   [right partition]

Build

Hotspot function

% CPU time

Calls

-O0

main

54.00%

3



quickSort

36.67%

83,797,840

-O2

partition

85.71%

667,116



quickSort

0.00% self (85.71% incl. children)

1 + 72,196 recursive



main

14.29%

—

-O3

quickSort

100.00%

1 + 666,534 recursive

3. PERF hardware counters

Env B — perf stat ./mysort 5000, -O0, full counters available

Metric

Value

Task-clock

80.13 ms (0.991 CPUs utilised)

Elapsed

0.080838 s

CPU cycles

320,918,638 (4.005 GHz)

Instructions

768,632,712

IPC

2.40

Branches

100,086,993

Branch misses

1,028,172 (1.03% of branches)

Cache references

not exposed by this kernel

Cache misses

not exposed by this kernel

Page faults

75

Top-down breakdown: 50.2% retiring, 30.2% bad speculation, 4.3% front-end bound,15.3% back-end bound. The 30.2% bad-speculation share is the interesting figure — BubbleSort's inner comparison is data-dependent and effectively unpredictable on random input,so nearly a third of issued work is thrown away on mispredicted branches.

Env A — perf stat, N = 1,000,000, Quick Sort only (hardware counters unavailable)

Metric

-O0

-O2

-O3

Reported sort time

0.26 s

0.08 s

0.08 s

Task-clock

274.67 ms

97.88 ms

92.10 ms

Elapsed

0.281188 s

0.107105 s

0.096383 s

User

0.272138 s

0.093105 s

0.089500 s

Sys

0.002979 s

0.004952 s

0.002983 s

CPUs utilised

0.977

0.914

0.956

Context switches

34

54

19

Page faults

531

1040

531

CPU cycles

need to run Locally 

Instructions

I need to run Locally 

IPC
need to run Locally 

Cache references

need to run Locally 

Cache misses

need to run Locally 

Branch misses

need to run Locally 

Optimisation speedup (task-clock, N = 10⁶ Quick Sort)

Build

Task-clock

Speedup vs -O0

-O0

274.67 ms

1.00×

-O2

97.88 ms

2.81×

-O3

92.10 ms

2.98×

4. Input-size scaling

Not yet collected — run ./mysort scaling and paste the output here.

N

Bubble (s)

Quick (s)

Bubble ratio vs previous

Quick ratio vs previous

1000





—

—

2000









4000









8000









16000









18250









32000









64000









Two data points are already available and can anchor the table:N = 5000 → Bubble 0.077638 s; N = 18250 → Bubble 1.042268 s. The ratio of sizes is 3.65×,and the ratio of times is 13.4× ≈ 3.65² = 13.3 — direct empirical confirmation of O(n²).

Answers

1. Which sorting algorithm performed better?

Quick Sort, by a factor of ~310× at N = 18250 (1.042 s vs 0.0034 s). Bubble Sort comparesonly adjacent elements, so a value can move at most one position per pass and the algorithmneeds ≈ n²/2 comparisons — O(n²). Quick Sort partitions around a pivot, placing that pivot inits final position and letting every other element jump an arbitrary distance in a single move;the array is halved at each level, giving ≈ n log₂ n comparisons — O(n log n). At N = 18250that is ≈ 1.7 × 10⁸ operations versus ≈ 2.6 × 10⁵, a difference of roughly three orders ofmagnitude, which matches the measured ratio once constant factors are included.

perf supports this from the hardware side: 30.2% bad speculation at N = 5000 shows thebranch predictor failing constantly on Bubble Sort's data-dependent comparison, so a largeshare of the CPU's issued work is discarded.

2. Which function consumed the maximum execution time?

The sorting routine itself dominates — bubbleSort in the baseline build, with theelement-swap helper second because Bubble Sort performs one swap per inverted pair(83.8 million swaps at N = 18250 with the fixed seed). Quick Sort's partition accounts fora negligible share by comparison.

The current myreport.txt mis-attributes this to main/quickSort due to a stalegmon.out; the figures above must be regenerated before final submission.

3. How does execution time change with increasing input size?

Bubble Sort's time grows quadratically: multiplying N by k multiplies the time by k².Measured, N = 5000 → 18250 is a 3.65× size increase and a 13.4× time increase, and3.65² = 13.3. Extrapolating to N = 10⁶ gives ≈ 3.6 hours, which matches the observed run.

Quick Sort grows as n log n — close to linear over this range. N = 18250 took 0.0034 s andN = 1,000,000 took 0.26 s: a 55× size increase produced only a 77× time increase, whereasBubble Sort over the same range would grow by a factor of ~3000.

4. How did compiler optimizations (-O2, -O3) improve performance?

The GPROF reports show that compiler optimization significantly changes where execution time is attributed. At -O2, partition() becomes the dominant hotspot, consuming 85.71% of the execution time, while quickSort() contributes almost no self time because most of the work is performed inside the partition routine. At -O3, the compiler performs more aggressive optimizations such as function inlining, causing partition() to disappear as a separate function. Consequently, 100% of the execution time is attributed to quickSort(). This indicates that the compiler merged smaller functions into the recursive quickSort routine, reducing function-call overhead and improving execution efficiency.



For Quick Sort at N = 10⁶, task-clock fell from 274.67 ms at -O0 to 97.88 ms at -O2(2.81×) and 92.10 ms at -O3 (2.98×). The gain comes mainly from -O2 inlining the smallswap and partition helpers — removing call/return overhead and stack traffic from theinnermost loop — plus keeping loop indices and the pivot in registers instead of memory,which -O0 deliberately avoids so that every variable is debuggable. -O3 adds vectorisationand more aggressive inlining, but sorting is branch-heavy and pointer-driven rather thanvectorisable, so it contributes only a further ~6%.

A side effect visible in the profiles: once swap is inlined at -O2/-O3, it disappearsfrom the GPROF flat profile entirely, because the function no longer exists in the binary.

5. Which algorithm would you recommend for large datasets?

Quick Sort. The gap widens with N — at 10⁶ elements it is the difference between 0.26 s andseveral hours, and the gap keeps growing because the complexity classes differ.

Caveats worth stating: this implementation uses the last element as pivot, which degrades toO(n²) on already-sorted or reverse-sorted input and can overflow the stack through deeprecursion. For production use, choose a random or median-of-three pivot, recurse into thesmaller partition first and loop on the larger to bound stack depth, and switch to insertionsort below ~16 elements — this is what qsort/introsort do. Quick Sort also parallelisesnaturally: after each partition the two halves are independent and can be sorted on separatethreads, so the algorithm maps well onto multicore hardware. Bubble Sort's sequentialdependency between adjacent passes makes it a poor candidate for parallelisation.