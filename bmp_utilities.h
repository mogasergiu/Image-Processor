#ifndef INCLUDE_
#define INCLUDE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bmp_header.h"

#define TOTAL_TASKS 5  // total number of images to execute at a time
#define MAX_IMAGE_NAME_LEN 35  // maximum image name length
#define PIXEL_LEN 3  // number of bytes a RGB pixel has
#define MIN(x, y) x < y ? x : y;
#define MAX(x, y) x > y ? x : y;
#define ABS_DIFF(x, y) (x - y) < 0 ? (y - x) : (x - y);  // abs(x - y)
#define DIRECTIONS_NUMBER 4

#pragma pack(1)

// struct to store a pixel's coordinates in a bitmap
typedef struct {
    int x,y;
} pair;

// struct to store the data about a pixel in the bitmap
typedef struct {
    unsigned char *bgr;
} pixel;

//struct to store the image's metadata
typedef struct {
    signed int padding;
    bmp_fileheader *bf;  // bmp_fileheader
    bmp_infoheader *bi;  // bmp_infoheader
    pixel **bitmap;
} imageData;

#pragma pack()

/*
 * imageName - name/location of the image to be read
 */
imageData* readImage(char *const imageName);

/*
 * image - stores the data about the image
 * imageName - name of the image
 */
void writeImage(imageData *const image, char *const imageName);

/*
 * x si y - pixel coordinates
 * image - the current bitmap we are working on
 */
int inBounds(imageData *image, int x, int y);

/*
 * image - image to be freed
 */
void freeImage(imageData *image);

/*
 * imageName - name/location of the initial image
 */
char** getNewImagesNames(char *const imageName);

/*
 * newImagesNames - store the new names for the images
 */
void freeNewImagesNames(char **newImagesNames);

/*
 * image - the bitmap we are going to apply the black and white filter to
 * imageName - the filtered image's new name
 */
void blackWhite(imageData *image, char *const imageName);

/*
 * image - the bitmap we are going to apply the no crop filter to
 * imageName - the filtered image's new name
 */
void noCropHorizontal(imageData *image, char *const imageName);

/*
 * image - the bitmap we are going to apply the no crop filter to
 * imageName - the filtered image's new name
 */
void noCropVertical(imageData *image, char *const imageName);

/*
 * image - the bitmap we are going to apply the filter to
 * imageName - the filtered image's new name
 * filterFileName - name/location of the filter matrix (for pooling and clustering only)
 */
void applyFilter(imageData *image, char *const imageName,
                char *const filterFileName);

/*
 * image - the bitmap we are going to apply the filter to
 * imageName - the filtered image's new name
 * filterSize - filter matrix's size
 */
void minPooling(imageData *image, char *const imageName,
                const unsigned char filterSize);

/*
 * image - the bitmap we are going to apply the filter to
 * imageName - the filtered image's new name
 * filterSize - filter matrix's size
 */
void maxPooling(imageData *image, char *const imageName,
                const unsigned char filterSize);

int getDiff(pixel *x, pixel *y);

/*
 * image - image to explore
 * storage - the current zone's maximal coordinates storage
 * init, sizeStorage - only change the pixels whose coordinates will be
 *                     found in "storage" between the two indexes
 * sum - the medial sums of the pixels of the current area
 */
void modifyBitmap(imageData **image, pair **storage, int init, int **sum,
                int *sizeStorage);

/*
 * image - image whose bitmap we explore
 * visited - keeps count of which pixels have been already operated on
 * x si y - the required pixel's coordinates
 * sum - stores the sum of all RGB bytes in the current area
 * storage - stores the coordinates of the pixels in the current area
 */
void exploreBitmap(imageData **image, int ***visited, int threshold,
                    int **directionsX, int **directionsY, int **sum,
                    pair **storage, int *sizeStorage, int x, int y);

/*
 * image - image we are to operate on
 * imageName - name of the image we need to obtain
 */
void clusterImage(imageData *image, char *const imageName, int threshold);

#endif
