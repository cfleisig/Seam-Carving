# Seam-Carving
## Project Description
This project employs the seam carving algorithm developed by Mitsubishi Electric Research Laboratories to resize images.

## Programming Techniques
* dynamic programing
* image processing
* multifile projects

## How to run C code
In terminal type `gcc seamcarving.c c_img.c -o seamcarving` to create executable file. Then `./seamcarving` to run the C program.

Note that as a mac user my terminal uses Z Shell (zsh), however if you use a differnet shell, these commands will likely not work for you.

## Libraries Used
For C: `<stdio.h>`, `<math.h>`

For Python: `PIL`

## Project Files
* Header and C files: This is the main part of the program, where I read in a binary file, apply the seam carving algorithm and write out a "carved" binary file.
* Python files: To conver .bin to an image file and back again
* Image files (.jpeg, .bin, .png): A variety of images are in the folder so that I could test using different images.

## Strucures Used
### rbg_img
* Purpose: to store height, width and image in a single neat structure in memory
* Defined in file: c_img.h
* Members:
    * `uint8_t *raster` --> points to place where image is stored in memory, raster is of size sizeof(uint8_t *) × height × width × 3
    * `size_t height` --> height of image in pixels
    * `size_t width'` --> width of image in pixels

## How this code works
This program can be broken down into 7 steps. 

Steps 1. & 7. are run in python, a sample of how to run them are found at the bottom of png2bin.py, in the form of a comment. A sample of how to run are in the steps 2.-6. are found in the main function in seamcarving.c.

Note that the example used in the code below only removes a vertical seam with a width of one pixel, for the naked eye to see changes in the image, MANY seams must be removed.

The steps are as follows:

1. *Create .bin file.* Write image to a .bin file
    * Function used: `write_image(image, filename)`
    * File in which function can be found: png2bin.py
    * Note
    * Example code snipet: 
    ```python:
     image = Image.open("meric_gertler.jpeg") # meric_gertler.jpeg is the file being converted to .bin format
     write_image(image, "meric_gertler.bin") # creates bin file meric_gertler.bin
     ```

2. *Read in .bin file.* Read in .bin image, and store in a struct rgb_img instance
    * Function used: `void read_in_img(struct rgb_img **im, char *filename);`
    * File in which function can be found: c_img.c
    * Example code snipet: 
    ```
     struct rgb_img *im;
     read_in_img(&im, "meric_gertler.bin");
     ```

3. *Energy Calculation.* Calculate the "energy/colour gradient" of each pixels and store these energy values in a "gradient image" of type struct rgb_img.
    * What is energy?: The energy measures the "importance" of each pixel. The higher the energy , the larger the colour gradient. The seam carving algorithm seeks to find remove seams with low energy pixels/small colour gradients, such as those in the middle of a plain blue sky. These pixels are "less interesting", so removing them is less likely to remove meaning from the image.
    * Function used: `void calc_energy(struct rgb_img *im, struct rgb_img **grad)`
    * File in which function can be found: seamcarving.c
    * Example code snipet: 
    ```
    struct rgb_img *new_im;
    calc_energy(im, &new_im);
    ```
    
4. *Seam identification.*
    * This is similar to the classic shortest path problem in an edge-weighted digraph except the vertices instead of the edges and the digraph is acyclic.
    * Function used: `void dynamic_seam(struct rgb_img *grad, double **best_arr);` and `void recover_path(double *best, int height, int width, int **path);`
    * File in which function can be found: seamcarving.c
    * Example code snipet: 
    ```
    double *mem; //contains the minimum total energy required to get to each pixel from the top of the image
    int *path; //points to an array containning indices of the lowest energy seam
    int width = im -> width;
    int height = im -> height;
    
    dynamic_seam(new_im, &mem);
    recover_path(mem, height, width, &path);
    ```

5. *Seam removal.* Removes lowest energy seam from rgb image, to create new "carved" rbg image.
    * Function used: `void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path)`
    * File in which function can be found: seamcarving.c
    * Example code snipet: 
    ```
    struct rgb_img *dest_im;
    remove_seam(im, &dest_im, path);
    ```

6. *Write "carved" image to .bin.*
    * Function used: `void write_img(struct rgb_img *im, char *filename)`
    * File in which function can be found: seamcarving.c
    * Example code snipet: 
    ```
    struct rgb_img *new_im;
    calc_energy(im, &new_im);
    ```

7. *Write carved .bin image to .png*
    * Function used: `def read_image(filename)`
    * File in which function can be found: png2bin.py
    * Example code snipet: 
    ```python:
    image = read_image("carved_meric_gertler.bin")
    image.save("carved_meric_gertler.png")
    ```
## Credits
Credit to Prof. Guerzhoy for assigning this project. 

This project was inspired by https://coursera.cs.princeton.edu/algs4/assignments/seam/specification.php.

