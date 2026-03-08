# Grayscale-ImageProcessor

A lightweight, dependency-free C++ library and command-line application for manipulating Portable Gray Map (PGM) images. 

This project is built from scratch to process binary PGM (P5) files and perform various mathematical, spatial, and geometric image operations. By modifying the `app/main.cpp` file, you can easily chain these methods together to achieve your desired image processing pipeline.

## Features

The core `Image` class currently supports the following operations:
* **File I/O:** Robust loading and saving of binary P5 PGM format images.
* **Pixel Adjustments:** Adjust image brightness (with proper value clipping) and scale contrast.
* **Gaussian Smoothing:** Apply 1D (X or Y axis) or full 2D Gaussian blurring using dynamically generated kernels.
* **Image Derivatives:** Calculate X and Y spatial derivatives using `[-1, 0, 1]` kernels, featuring optional pre-smoothing to reduce noise.
* **Edge Detection:** Extract structural edges based on the gradient magnitude (calculating the square root of Ix² + Iy²) against a specified threshold.
* **Geometric Transformations:** Rotate the image by a specific angle (`theta`) around a targeted `(x, y)` pivot point.

## Prerequisites

* **CMake:** Version 3.12 or higher.
* **Compiler:** Must support the C++17 standard.

## Compilation and Build

1. Clone or navigate to the project directory:
   ```bash
   cd LOCATION_OF_PROJECT/Grayscale-ImageProcessor/
   ```
2. Create a build directory and navigate into it:
   ```bash
   mkdir build
   cd build/
   ```
3. Compile the project using CMake:
   ```bash
   cmake ..
   cmake --build .
   ```

## Running the Application

After building, an executable named `GrayscaleImage` will be generated in your `build` directory. 

You must pass the path to the target PGM image as a command-line argument. The default execution applies edge detection and saves the output to `/tmp/ModifiedImage.pgm`.

```bash
# Example usage using the provided sample image
./GrayscaleImage ../data/example.pgm
```

 ---

## 📜 License

This project is licensed under the MIT License. Copyright (c) 2026 Buğrahan İmal. You are free to use, copy, modify, merge, publish, and distribute this software as per the license conditions.
