# Cell-Counter-C
This is a solution to the Cell Counter assignment 1 in the course Computer Systems (02132) made by Sebastian F. Taylor (@Sebastian-Francis-Taylor) and Victor Reynolds (@slayervictor).

# Instructions
## Installation
To install the program simply clone this repository:
```bash
git clone https://github.com/Sebastian-Francis-Taylor/Cell-Counter-C
```
compile the program
```bash
make
```
run the program
```
bin/cell-counter
```

### Compilation
```bash
make              # Regular optimised build
make debug        # Debug build with symbols (creates cell-counter-debug)
make valgrind     # Build debug version and run valgrind on it
make timing       # Builds version with execution time print statements (creates cell-counter-timing)
make clean        # Removes old builds 
```

# Assignment Checklist
## Tasks
- [x] **T1**: Read carefully the entire document to acquire a clear and complete understanding of the algorithm to be implemented.  
- [x] **T2**: Design and structure the code (e.g., divide functionality into functions, decide function prototypes, choose buffers, etc.).  
- [x] **T3**: Implement the design in C and perform unit tests to ensure new code sections work as expected.  
- [x] **T4**: Test functionality and perform execution time and memory usage analysis.  
- [x] **T5**: Perform optimizations and enhancements to improve detection rate, execution time, and memory usage.  
- [x] **T6**: Test functionality again and analyze execution time and memory usage to demonstrate the impact of optimizations.  
- [x] **T7**: Prepare a short report according to the provided instructions.  

## Achievements for points
- [x] (5pt) Convert to grayscale
- [x] (5pt) Apply binary threshold
- [x] (10pt) Erode Image
- [x] (10pt) Detect Spots (cells)
- [x] (10pt) Generate output image
- [x] (5pt) Print results
- [x] (15pt) Functional test
- [x] (5pt) Execution time analysis
- [x] (5pt) Memory use analysis
- [x] (25pt) Optimizations and enhancements (including testing)
- [x] (5pt) Report 

## Base Optimizations and enhancement
- [x] Dynamic calculation of the threshold for the generation of the binary image. As previously mentioned, understanding and investigating the Otsu’s method is a good starting point.
- [x] Efficient and optimized erosion and spot detection implementation.
- [x] Using different size and placements of ones of the structuring element for the erosion.
- [x] Adapting the algorithm to improve performance or detection ratio.
- [x] Minor optimizations: More efficient encoding of the binary image, optimized conversion to gray-scale to avoid division, re-use of the arrays where the images are stored.
