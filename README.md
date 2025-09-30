# Cell-Counter-C
This is the solution to the Cell Counter assignment 1 in the course Computer Systems (02132) made by Sebastian Taylor (@Sebastian-Francis-Taylor) and Victor Reynolds (@slayervictor).

# Instructions
Instructions here, when we are ready

# Assignment Checklist
## Tasks
- [x] **T1**: Read carefully the entire document to acquire a clear and complete understanding of the algorithm to be implemented.  
- [ ] **T2**: Design and structure the code (e.g., divide functionality into functions, decide function prototypes, choose buffers, etc.).  
- [ ] **T3**: Implement the design in C and perform unit tests to ensure new code sections work as expected.  
- [ ] **T4**: Test functionality and perform execution time and memory usage analysis.  
- [ ] **T5**: Perform optimizations and enhancements to improve detection rate, execution time, and memory usage.  
- [ ] **T6**: Test functionality again and analyze execution time and memory usage to demonstrate the impact of optimizations.  
- [ ] **T7**: Prepare a short report according to the provided instructions.  

## Achievements for points
- [x] (5pt) Convert to grayscale
- [x] (5pt) Apply binary threshold
- [x] (10pt) Erode Image
- [x] (10pt) Detect Spots (cells)
- [x] (10pt) Generate output image
- [ ] (5pt) Print results
- [x] (15pt) Functional test
- [x] (5pt) Execution time analysis
- [ ] (5pt) Memory use analysis
- [ ] (25pt) Optimizations and enhancements (including testing)
- [ ] (5pt) Report 

## Optimizations and enhancement
- [x] Dynamic calculation of the threshold for the generation of the binary image. As previously mentioned, understanding and investigating the Otsu’s method is a good starting point.
- [x] Efficient and optimized erosion and spot detection implementation.
- [x] Using different size and placements of ones of the structuring element for the erosion.
- [ ] Using different size and shape for the detection area and exclusion frame for the spot detection.
- [ ] Adapting the algorithm to improve performance or detection ratio.
- [ ] Minor optimizations: More efficient encoding of the binary image, opti- mized conversion to gray-scale to avoid division, re-use of the arrays where the images are stored.
