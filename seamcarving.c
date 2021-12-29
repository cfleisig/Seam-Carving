#include "seamcarving.h"
#include <stdio.h>
#include <math.h>

static
void print_rgb(struct rgb_img *im){
    int height = im -> height;
    int width = im -> width;
    for(int i = 0; i < height * width * 3; i++){
        printf("%d ", im -> raster[i]);

        if((i + 1) % 3 == 0){
            printf("\t");
        }
        if((i + 1) % (3 * width) == 0){
            printf("\n");
        }
    }
}

void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    int del_r, del_g, del_b, below, above, del_x, del_y;
    int height = im -> height;
    int width = im -> width;

    create_img(grad, height, width);
    (*grad) -> height = height;
    (*grad) -> width = width;

    for(int i = 0; i < height * width; i++){
        above = ((i + 1) % width == 0) ? (i - (width-1))*3 : (i + 1)*3; //check if im in last column
        below = (i % width == 0) ? (i + (width-1))*3 : ((i - 1)*3); //check if im in first column
        del_r = im -> raster[above] - im -> raster[below];
        del_g = im -> raster[above + 1] - im -> raster[below + 1];
        del_b = im -> raster[above + 2] - im -> raster[below + 2];
        del_x = del_r * del_r + del_g * del_g + del_b * del_b;

        below = (i + width > width * height - 1) ? i*3 - (height-1)*width*3 : i*3 + width*3; //check if im in last row
        above = (i < width) ? i*3 + (height-1)*width*3 : i*3 - width*3; //check if im in first row
        del_r = im -> raster[below] - im -> raster[above];
        del_g = im -> raster[below + 1] - im -> raster[above + 1];
        del_b = im -> raster[below + 2] - im -> raster[above + 2];
        del_y = del_r * del_r + del_g * del_g + del_b * del_b;
        
        (*grad) -> raster[i*3] = (uint8_t)(sqrt((double)(del_x + del_y))/10);
        (*grad) -> raster[i*3 + 1] = (uint8_t)(sqrt((double)(del_x + del_y))/10);
        (*grad) -> raster[i*3 + 2] = (uint8_t)(sqrt((double)(del_x + del_y))/10);
    }
}

static
int find_min_above(int i, double *best_arr, int width){

    int mid = i - width;
    int right = i - width + 1;
    int left = i - width - 1;

    if (i % width == 0){ //check if in leftmost column
        return best_arr[mid] > best_arr[right] ? right : mid;
    } else if((i + 1) % width == 0){ //check if in rightmost column
        return best_arr[mid] >= best_arr[left] ? left : mid;
    } else { //if not an edge case
        if (best_arr[right] >= best_arr[left] && best_arr[mid] >= best_arr[left]){
            return left;
        } else if (best_arr[left] >= best_arr[right] && best_arr[mid] >= best_arr[right]){
            return right;
        } else {
            return mid;
        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr){
    int height = grad -> height;
    int width = grad -> width;

    *best_arr = (double *)malloc(height*width*sizeof(double));

    //fill out first row of best_arr - will be identical to first row of best_arr
    for (int i = 0; i < width; i++) {
        (*best_arr)[i] = (double)(grad->raster[i*3]);
    }

    //fill out remaining rows of best_arr
    for (int i = width; i < width * height; i++){
        (*best_arr)[i] = (*best_arr)[find_min_above(i, *best_arr, width)] + (double)(grad->raster[i*3]);
    }

    //print best_arr
    /*for (int i = 0; i < 500; i++){
        printf("%d\t", (int)(*best_arr)[i]);
        if ((i+1) % width == 0){printf("\n\n");}
    }
    printf("\n"); */
}

void recover_path(double *best, int height, int width, int **path){    
    int low_i = width * height - 1;
    int index = height - 1;
    *path = (int *)malloc((height) * sizeof(int));
    
    //find minimum in bottom rows
    for(int i = width * height - 2; i >= width * (height - 1) ;i--){
        if(best[i] <= best[low_i]){low_i = i;}
    }
    (*path)[index] = low_i - width * index;

    //find lowest in remaining rows
    while(index > 0){
        index--;
        low_i = find_min_above(low_i, best, width);
        (*path)[index] = low_i - width * index;
    }
}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){
    int dest_col_i, cord_dest, cord_src;
    
    //initialize destination image
    create_img(dest, src -> height, src -> width - 1);

    //copy over all values from src row by row to dest except those corresponding to ones in path
    for(int row_i = 0; row_i < src -> height; row_i++){
        dest_col_i = 0;
        for(int col_i = 0; col_i < src -> width; col_i++){

            if(col_i != path[row_i]){
                cord_dest = (row_i * ((*dest) -> width) + dest_col_i)*3;
                cord_src = (row_i * (src -> width) + col_i)*3;

                (*dest) -> raster[cord_dest] = src -> raster[cord_src];
                (*dest) -> raster[cord_dest + 1] = src -> raster[cord_src + 1]; 
                (*dest) -> raster[cord_dest + 2] = src -> raster[cord_src + 2];
                
                dest_col_i++;
            }
        }
    }
}

int main(){
    /* struct rgb_img *im;
    struct rgb_img *cur_im;
    struct rgb_img *grad;
    double *best;
    int *path;

    read_in_img(&im, "HJoceanSmall.bin");
    
    for(int i = 0; i < 200; i++){
        printf("i = %d\n", i);
        calc_energy(im,  &grad);
        dynamic_seam(grad, &best);
        recover_path(best, grad->height, grad->width, &path);
        remove_seam(im, &cur_im, path);

        char filename[200];
        sprintf(filename, "img%d.bin", i);
        write_img(cur_im, filename);


        destroy_image(im);
        destroy_image(grad);
        free(best);
        free(path);
        im = cur_im;
    }
    destroy_image(im); */
    
    struct rgb_img *im;
    struct rgb_img *new_im;
    struct rgb_img *dest_im;
    double *mem;
    char filename[200];
    int *path;

    read_in_img(&im, "HJoceanSmall.bin"); //meric_gerler.bin -> struct rgb_img *img and puts in memory stack
    printf("height: %zu\n", im -> height);
    printf("width: %zu\n", im -> width);
    //print_rgb(im);
    calc_energy(im, &new_im);
    printf("\n");
    //print_grad(new_im);
    //printf("\n");
    dynamic_seam(new_im, &mem);
    printf("%d, %d\n\n", (int)mem[im -> width + 497], (int)mem[im -> width + 498]);
    recover_path(mem, 5, 6, &path);
    //for(int i = 0; i < im -> height; i++){ printf("%d\t", path[i]);}
    //printf("\n");
    remove_seam(im, &dest_im, path);
    //printf("\n");
    //print_rgb(dest_im);
    //printf("\n");
    
    /*float factor[5] = {0.4, 0.6, 0.8, 1.2, 1.4}; //factors by which to brighten or darken rgb (>1 brightens, <1 darkens)

    //loop to create 5 different images
    for(int i = 0; i < 5; i++){
        sprintf(filename, "img%d.bin", i); //makes the filename with number
        brighten_im(im, &new_im, filename, factor[i]);
    }*/

    return 0;
}