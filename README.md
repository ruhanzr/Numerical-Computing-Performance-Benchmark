# Numerical Computing Performance Benchmark in C
gcc main.c -o matrix.exe
.\matrix.exe
## Project Overview
This project was to learn about optimization and algorithmic processes through performance benchmarking simple matrix multiplication. The goal was to implement matrix multiplication, verify correctness, benchmark different loop orderings, test whether cache-aware blocking/tiling could improve runtime, and determine any other external factors which could impact performance.

The project focuses on:
* Implementing matrix multiplication in C
* Generating aand filling square matrices with random values
* Benchmarking runtime across different matrix sizes
* Comparing all six loop orderings for matrix multiplication
* Understanding how C memory layout and CPU cache behavior affect performance
* Implementing blocked/tiled matrix multiplication
* Verifying correctness using checksum comparisons
* Understanding data comparisons
* Understanding the role of environment in performance benchmarking

## Basic Matrix Multiplication
I began with a basic algorithm that included code for accepting inputs, filling matrices with random values, and printing matrices. The first working version used a fixed 3×3 matrix size so that I could manually check the output and confirm that the multiplication logic was correct.

The core of the algorithm is a triple loop. One loop controls the output column, one loop controls the output row, and the innermost loop performs the row-column multiplication needed for each element of the final matrix.

For each position in the output matrix, the program multiplies a row from the first matrix by a column from the second matrix. Each pair of values is multiplied and added to a running total, and that total becomes the value stored in the product matrix.

The first matrix depends on the row being calculated and the inner loop variable. The second matrix depends on the inner loop variable and the output column, because the correct column of the second matrix has to be held while the row of the first matrix is being traversed. The product matrix depends on the output row and output column.

After verifying the 3×3 version, I replaced the fixed matrix size with a defined constant. This allowed the same algorithm to work for larger square matrices without rewriting the loop logic.

## Benchmarking Setup
Once the multiplication was working, I added runtime benchmarking using the time.h library. This allowed me to measure how long each matrix multiplication took.

After increasing the matrix size, I needed to stop printing the full matrices because it does not provide much benefit and creates several issues such as:
* Large matrices cannot realistically be checked by hand.
* Printing hundreds of rows and columns is difficult to read.
* Terminal output can affect program execution time.
* Printing does not help with performance benchmarking.

Instead, I added a checksum function that calculates the sum of all elements in the output matrix. This allowed me to confirm that different algorithms produced the same result without printing the entire matrix. With this new function, I added in logic where any matrix under the size of 5x5 would print out, and anything above that size would use the checkSum function. 

I benchmarked matrix sizes of: 
* 50x50
* 100x100
* 200x200
* 300x300
* 400x400

For each size, I ran three trials, recorded the runtime, and calculated average times in Microsoft Excel.

## Loop Order Optimization

After getting baseline timing data, I tried to improve performance by changing the loop order. The original implementation used KIJ loop ordering, and I first compared it against an IJK-style implementation.

I created a duplicate function where I could test this, it was the same function but with the variables reordered. Below are my results for the original KIJ loop, and my reordered IJK loop.
![KIJ Loop Order Results](images/old_BASELINE_KIJ.png)
![IJK Loop Order Results](images/old_optimized_IJK.png)

The checksums for both versions were the same, meaning both algorithms produced the same output. However, the runtime changed slightly. As seen in the table, the IJK version showed a small speedup, with the performance gains becoming more noticeable as matrix size increased.

This led me to look deeper into why changing the loop order affected runtime.

## Cache Locality and Row-Major Memory Layout

After experimenting and researching how CPU cache works, I realized that the speed difference was related to how C stores 2D arrays in memory.

C stores 2D arrays in row-major order, meaning elements in the same row are stored next to each other in memory. Accessing values across a row is usually faster than accessing values down a column because row elements are closer together in memory.

I thought about it like this, in a 4×4 matrix, moving across a row accesses memory positions like:
    0, 1, 2, 3

But moving down a column accesses positions more like:
    3, 7, 11, 15

Those column values are farther apart in memory, so the CPU has to jump around more.

The CPU also does not usually grab just one integer from memory at a time. It grabs a small chunk of nearby memory called a cache line. Because of this, accessing values that are next to each other in memory is faster because nearby values may already be loaded into cache.

This performance gain is not very noticeable for small matrices, but as matrix size increases, memory access patterns start to matter more.

The key question for predicting loop-order performance became:

    Which index changes in the innermost loop, and does that make the program move across rows or down columns?

Based on this, I predicted that loop orders with more row-wise memory access would perform better. I initially expected an IKJ-style loop to perform well because the innermost loop can access matrixB[k][j] and matrixC[i][j] row-wise, while reusing a value from matrixA. 

Although I expected IKJ to perform better because of its row-wise access pattern, my benchmark showed that IJK was faster under my testing conditions. Since the implementations were structurally very similar, this result suggests that performance is affected by more than just the predicted memory access pattern. Compiler behavior, timing noise, register usage, and system load may have contributed to the difference. Because of this, I treated the benchmark results as empirical rather than assuming the theoretically more cache-friendly loop order would always be fastest.

Because of these results, I tested all six possible loop orderings:
* IJK ![IJK Results](images/old_optimized_IJK.png)
* IKJ ![IKJ Results](images/IKJ.png)
* JIK ![JIK Results](images/JIK.png)
* JKI ![JKI Results](images/JKI.png)
* KIJ ![KIJ Results](images/KIJ.png)
* KJI ![KJI Results](images/KJI.png)

Before testing, I expected loop orders with more row-wise access to perform better because C stores 2D arrays in row-major order. The results showed that IJK was the fastest for larger matrix sizes on my machine, reaching about a 1.12× speedup over the slowest/baseline ordering at 400×400 in the original benchmark data.

This suggests that cache locality matters, but performance also depends on implementation details such as local accumulation, matrixC write frequency, compiler behavior, and timing noise for small matrix sizes.

## Blocked/Tiled Matrix Multiplication

After testing loop order, I implemented blocked/tiled matrix multiplication. The idea behind blocking is to break a large matrix into smaller square blocks so that the CPU can reuse data while it is still in cache.

The blocked algorithm uses six total loops:
* The three outer loops select the current block.
* The three inner loops perform matrix multiplication inside that block.

I tested several block sizes on a 400×400 matrix:
* 2
* 8
* 16
* 20
* 25
* 50
* 100
* 200

At first, blocking produced concerning results. The blocked version was much slower than the best non-blocked version, even when using the fastest block size from that first test (50), nearly 50% slower. My conclusion was that the blocked algorithm added extra overhead because it uses more loops, and that overhead may have been large enough to outweigh the cache benefits.

When I wrote my initial basic tirple loop multiplication algorithm I used a local variable to accumulate values, but when writing my blocking algorithm I thought it may have been more efficient to just write and accumulate directly into the matrix. After seeing the results, I went back to my original use of a local variable to accumulate values. This improved performance, but the blocked version was still slower than the original triple-loop system in that round of testing.

I also noticed that the timing data was inconsistent between runs. My best guess was that this was probably caused by other applications running on my computer, connected devices, background processes, and general system load. Because of this, I changed the program so that it automatically ran a set number of trials and calculated the average time in one pass. This made the benchmarking process more consistent, easier to compare, and easier to conduct.

## Updated Benchmark Results

Now having a quick way to benchmark, I opted to redo some of the important data I had already gotten which includes the baseline test in the original algorithm, the altered IJK ordered loop, and I recalculated the speed up times using it. I noticed that the new runtimes were longer across all versions compared to the earlier results. Because of this, the earlier data should not be directly compared to the newer data. Some external factor likely increased the load on my computer and reduced overall performance.
![New KIJ Loop Order Results](images/new_BASELINE_KIJ.png)
![New IJK Loop Order Results](images/new_optimized_IJK.png)

The relative performance gains also changed slightly. In the earlier results, switching from KIJ to IJK produced about a 1.12× speedup at 400×400. In the newer data, the same comparison produced about a 1.07× speedup.

However, because the newer data was collected under the same conditions, it can still be used to fairly compare the blocked multiplication algorithm.

Using the updated benchmarks, the best blocked result came from a block size of 20. This version achieved about a 1.15× speedup over the original KIJ implementation and about a 1.07× speedup over the optimized IJK loop order at 400×400.
![Blocking Algorithm Results](images/blocking.png)

This suggests that, under the updated testing conditions, cache blocking with a block size of 20 provided the best performance improvement among the tested versions.

## Key Takeaways

One of the biggest takeaways from this project is that performance testing is very environment-dependent. Results can change depending on system load, background processes, compiler settings, and when the data is collected.

Another takeaway is that optimization is not always straightforward. Some changes that seem like they should be faster, such as certain loop orderings or blocked multiplication, may not always improve runtime unless the implementation details also support the optimization.

The project showed that:
* Loop order affects runtime because of memory access patterns.
* C stores 2D arrays in row-major order, so row-wise access is usually more cache-friendly.
* Cache locality matters more as matrix size increases.
* Checksums are useful for verifying that different implementations produce the same output.
* Blocked multiplication can improve performance, but only if the implementation and block size are effective.
* The best block size in my updated testing was 20.
* Compiler settings can significantly affect results.

At one point, I also tested the program using GCC with the -O2 optimization flag. This significantly changed the results. My IJK loop became slower than the original KIJ loop, and the blocking algorithm was still slower under that compiler setting. This showed that compiler optimization is another major factor in performance testing.

Overall, this project showed that matrix multiplication performance depends on more than just the number of arithmetic operations. Memory layout, cache behavior, loop order, local accumulation, block size, compiler behavior, and system load all affect runtime.
