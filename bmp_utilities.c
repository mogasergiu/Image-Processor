#include "bmp_utilities.h"

imageData* readImage(char *const imageName) {
    FILE *imageFile = fopen(imageName, "rb");

    imageData *image= (imageData*)calloc(1, sizeof(imageData));
    if (image == NULL) {
        return NULL;
    }

    image -> bf = (bmp_fileheader*)calloc(1, sizeof(bmp_fileheader));
    if (image -> bf == NULL) {
        return NULL;
    }
    fread(image -> bf, sizeof(bmp_fileheader), 1, imageFile);

    image -> bi = (bmp_infoheader*)calloc(1, sizeof(bmp_infoheader));
    if (image -> bi == NULL) {
        return NULL;
    }
    fread(image -> bi, sizeof(bmp_infoheader), 1, imageFile);

    image -> padding = image -> bi -> width % 4;

    // allocating and storing the bitmap
    fseek(imageFile, image -> bf -> imageDataOffset, SEEK_SET);
    image -> bitmap = (pixel**)calloc(image -> bi -> height, sizeof(pixel*));
    if (image -> bitmap == NULL) {
        return NULL;
    }
    int i, j, k;
    unsigned char dumpster;
    for (i = 0; i < image -> bi -> height; i++) {
        image -> bitmap[i] = (pixel*)calloc(image -> bi -> width,
                            sizeof(pixel));
        if (image -> bitmap[i] == NULL) {
            return NULL;
        }
        for (j = 0; j < image -> bi -> width; j++) {
            image -> bitmap[i][j].bgr = (unsigned char*)calloc(PIXEL_LEN,
                                            sizeof(unsigned char));
            if (image -> bitmap[i][j].bgr == NULL) {
                return NULL;
            }
            for (k = 0; k < PIXEL_LEN; k++) {
                fread(&image -> bitmap[i][j].bgr[k], sizeof(unsigned char), 1,
                    imageFile);
            }
        }
        for (j = 0; j < image -> padding; j++) {
            fread(&dumpster, sizeof(unsigned char), 1, imageFile);
        }
    }

    fclose(imageFile);
    return image;
}

void writeImage(imageData *const image, char *const imageName) {
    FILE *imageFile = fopen(imageName, "wb");
    const unsigned char byteZero = 0;

    fwrite(image -> bf, sizeof(bmp_fileheader), 1, imageFile);
    fwrite(image -> bi, sizeof(bmp_infoheader), 1, imageFile);
    fseek(imageFile, image -> bf -> imageDataOffset, SEEK_SET);

    int i, j, k;
    for (i = 0; i < image -> bi -> height; i++) {
        for (j = 0; j < image -> bi -> width; j++) {
            for (k = 0; k < PIXEL_LEN; k++) {
                fwrite(&image -> bitmap[i][j].bgr[k], sizeof(unsigned char), 1,
                        imageFile);
            }
        }
        for (j = 0; j < image -> padding; j++) {
            fwrite(&byteZero, sizeof(unsigned char), 1, imageFile);
        }
    }

    fclose(imageFile);
}

int inBounds(imageData *image, int x, int y) {
    if (x >= image -> bi -> width || x < 0) {  // if it goes beyond width
        return 0;
    }

    if (y >= image -> bi -> height || y < 0) {  // if it goes beyond height
        return 0;
    }

    return 1;
}

void freeImage(imageData *image) {
    if (image != NULL) {
        if (image -> bitmap != NULL) {
            int i, j;
            for (i = 0; i < image -> bi -> height; i++) {
                if (image -> bitmap[i] != NULL) {
                    for (j = 0; j < image -> bi -> width; j++) {
                        if (image -> bitmap[i][j].bgr != NULL) {
                            free(image -> bitmap[i][j].bgr);
                            image -> bitmap[i][j].bgr = NULL;
                        }
                    }
                    free(image -> bitmap[i]);
                    image -> bitmap[i] = NULL;
                }
            }
            free(image -> bitmap);
            image -> bitmap = NULL;
        }

        if (image -> bf != NULL) {
            free(image -> bf);
            image -> bf = NULL;
        }

        if (image -> bi != NULL) {
            free(image -> bi);
            image -> bi = NULL;
        }

        free(image);
        image = NULL;
    }
}

char** getNewImagesNames(char *const imageName) {
    char **newImagesNames = (char**)calloc(TOTAL_TASKS, sizeof(char*));
    if (newImagesNames == NULL) {
        return NULL;
    }
    int i;
    for (i = 0; i < TOTAL_TASKS; i++) {
        newImagesNames[i] = (char*)calloc(MAX_IMAGE_NAME_LEN, sizeof(char));
        if (newImagesNames[i] == NULL) {
            return NULL;
        }
    }

    char *dotBmpPosition = strstr(imageName, ".bmp");  // ".bmp" position
    short int testNo = *(dotBmpPosition - 1) - '0';  // index of requested image
    snprintf(newImagesNames[0], MAX_IMAGE_NAME_LEN,
            "test%hu_black_white.bmp", testNo);
    snprintf(newImagesNames[1], MAX_IMAGE_NAME_LEN,
            "test%hu_nocrop.bmp", testNo);
    snprintf(newImagesNames[2], MAX_IMAGE_NAME_LEN,
            "test%hu_filter.bmp", testNo);
    snprintf(newImagesNames[3], MAX_IMAGE_NAME_LEN,
            "test%hu_pooling.bmp", testNo);
    snprintf(newImagesNames[4], MAX_IMAGE_NAME_LEN,
            "test%hu_clustered.bmp", testNo);

    return newImagesNames;
}

void freeNewImagesNames(char **newImagesNames) {
    if (newImagesNames != NULL) {
        int i;
        for (i = 0; i < TOTAL_TASKS; i++) {
            if (newImagesNames[i] != NULL) {
                free(newImagesNames[i]);
                newImagesNames[i] = NULL;
            }
        }
        free(newImagesNames);
        newImagesNames = NULL;
    }
}

void blackWhite(imageData *image, char *const imageName) {
    FILE *imageFile = fopen(imageName, "wb");
    const unsigned char byteZero = 0;

    fwrite(image -> bf, sizeof(bmp_fileheader), 1, imageFile);
    fwrite(image -> bi, sizeof(bmp_infoheader), 1, imageFile);
    fseek(imageFile, image -> bf -> imageDataOffset, SEEK_SET);

    int i, j, k, s;
    for (i = 0; i < image -> bi -> height; i++) {
        for (j = 0; j < image -> bi -> width; j++) {
            for (k = 0, s = 0; k < PIXEL_LEN; k++) {
                s += image -> bitmap[i][j].bgr[k];
            }
            for (k = 0, s /= PIXEL_LEN; k < PIXEL_LEN; k++) {
                fwrite(&s, sizeof(unsigned char), 1, imageFile);
            }
        }
        for (j = 0; j < image -> padding; j++) {
            fwrite(&byteZero, sizeof(unsigned char), 1, imageFile);
        }
    }

    fclose(imageFile);
}

void noCropHorizontal(imageData *image, char *const imageName) {
    FILE *imageFile = fopen(imageName, "wb");
    signed int difference = image -> bi -> width - image -> bi -> height;
    signed int firstHalf, secondHalf;
    const unsigned char byteZero = 0;
    const unsigned char byteBorder = 255;
    signed int oldHeight =  image -> bi -> height;

    if (difference & 1) {
        firstHalf = (difference >> 1) + 1;
    } else {
        firstHalf = difference >> 1;
    }
    secondHalf = firstHalf + image -> bi -> height;

    image -> bi -> height = image -> bi -> width;

    fwrite(image -> bf, sizeof(bmp_fileheader), 1, imageFile);
    fwrite(image -> bi, sizeof(bmp_infoheader), 1, imageFile);
    fseek(imageFile, image -> bf -> imageDataOffset, SEEK_SET);

    int i, j, k;
    for (i = 0; i < firstHalf; i++) {  // upper white part
        for (j = 0; j < image -> bi -> width; j++) {
            for (k = 0; k < PIXEL_LEN; k++) {
                fwrite(&byteBorder, sizeof(unsigned char), 1, imageFile);
            }
        }
        for (j = 0; j < image -> padding; j++) {
            fwrite(&byteZero, sizeof(unsigned char), 1, imageFile);
        }
    }
    // placing the image between the upper and lower white portions
    for (i = firstHalf; i < secondHalf; i++) {
        for (j = 0; j < image -> bi -> width; j++) {
            for (k = 0; k < PIXEL_LEN; k++) {
                fwrite(&image -> bitmap[i - firstHalf][j].bgr[k],
                        sizeof(unsigned char), 1, imageFile);
            }
        }
        for (j = 0; j < image -> padding; j++) {
            fwrite(&byteZero, sizeof(unsigned char), 1, imageFile);
        }
    }

    // lower white area
    for (i = secondHalf; i < image -> bi -> height; i++) {
        for (j = 0; j < image -> bi -> width; j++) {
            for (k = 0; k < PIXEL_LEN; k++) {
                fwrite(&byteBorder, sizeof(unsigned char), 1, imageFile);
            }
        }
        for (j = 0; j < image -> padding; j++) {
            fwrite(&byteZero, sizeof(unsigned char), 1, imageFile);
        }
    }

    image -> bi -> height = oldHeight;  // preserving the initial image's size

    fclose(imageFile);
}

void noCropVertical(imageData *image, char *const imageName) {
    FILE *imageFile = fopen(imageName, "wb");
    signed int difference = image -> bi -> height - image -> bi -> width;
    signed int firstHalf, secondHalf;
    firstHalf = difference >> 1;
    secondHalf = firstHalf + image -> bi -> width;
    const unsigned char byteZero = 0;
    const unsigned char byteBorder = 255;
    signed int oldWidth =  image -> bi -> width;

    image -> bi -> width = image -> bi -> height;

    fwrite(image -> bf, sizeof(bmp_fileheader), 1, imageFile);
    fwrite(image -> bi, sizeof(bmp_infoheader), 1, imageFile);
    fseek(imageFile, image -> bf -> imageDataOffset, SEEK_SET);

    int i, j, k;
    for (i = 0; i < image -> bi -> height; i++) {
        for (j = 0; j < firstHalf; j++) {  // the left white area
            for (k = 0; k < PIXEL_LEN; k++) {
                fwrite(&byteBorder, sizeof(unsigned char), 1, imageFile);
            }
        }

        for (; j < secondHalf; j++) {  // placing the image between the white areas
            for (k = 0; k < PIXEL_LEN; k++) {
                fwrite(&image -> bitmap[i][j - firstHalf].bgr[k],
                        sizeof(unsigned char), 1, imageFile);
            }
        }

        for (j = 0; j < image -> padding; j++) {
            fwrite(&byteZero, sizeof(unsigned char), 1, imageFile);
        }

        // the right white area of the image
        for (j = secondHalf; j < image -> bi -> width; j++) {
            for (k = 0; k < PIXEL_LEN; k++) {
                fwrite(&byteBorder, sizeof(unsigned char), 1, imageFile);
            }
        }
    }

    image -> bi -> width = oldWidth;  // preserving initial image's size

    fclose(imageFile);
}

void applyFilter(imageData *image, char *const imageName,
                char *const filterFileName) {
    FILE *filterFile = fopen(filterFileName, "r");
    FILE *imageFile = fopen(imageName, "wb");
    const unsigned char byteZero = 0;
    int filterSize, i, j, m, n, k, s;
    unsigned char filteredByte;

    fscanf(filterFile, "%d", &filterSize);
    double **filterMatrix = (double**)calloc(filterSize, sizeof(double*));
    if (filterMatrix == NULL) {
        return;
    }
    for (i = 0; i < filterSize; i++) {
        filterMatrix[i] = (double*)calloc(filterSize, sizeof(double));
        if (filterMatrix[i] == NULL) {
            return;
        }
    }

    for (i = filterSize - 1; i >= 0; i--) {  // reading the desired filter upside-down
        for (j = 0; j < filterSize; j++) {
            fscanf(filterFile, "%le", &filterMatrix[i][j]);
        }
    }

    fwrite(image -> bf, sizeof(bmp_fileheader), 1, imageFile);
    fwrite(image -> bi, sizeof(bmp_infoheader), 1, imageFile);
    fseek(imageFile, image -> bf -> imageDataOffset, SEEK_SET);

    for (i = 0; i < image -> bi -> height; i++) {
        for (j = 0; j < image -> bi -> width; j++) {
            /*
             * filter matrix's coordinates, relative to the pixel's coordinates
             * (upper left in the filter matrix)
             */
            const int upperLeftY = i - filterSize / 2;
            const int upperLeftX = j - filterSize / 2;
            for (k = 0; k < PIXEL_LEN; k++) {
                s = 0;
                for (m = upperLeftY; m < upperLeftY + filterSize; m++) {
                    for (n = upperLeftX; n < upperLeftX + filterSize; n++) {
                        if (inBounds(image, n, m) == 1) {
                            s += image -> bitmap[m][n].bgr[k] *
                                filterMatrix[m - upperLeftY][n - upperLeftX];
                        }
                    }
                }

                if (s > 255) {
                    filteredByte = 255;
                } else if (s < 0) {
                    filteredByte = 0;
                } else {
                    filteredByte = s;
                }
                fwrite(&filteredByte, sizeof(unsigned char), 1, imageFile);
            }
        }
        for (j = 0; j < image -> padding; j++) {
            fwrite(&byteZero, sizeof(unsigned char), 1, imageFile);
        }
    }

    for (i = 0; i < filterSize; i++) {
        free(filterMatrix[i]);
        filterMatrix[i] = NULL;
    }
    free(filterMatrix);
    filterMatrix = NULL;

    fclose(filterFile);
    fclose(imageFile);
}

void minPooling(imageData *image, char *const imageName,
                const unsigned char filterSize) {
    FILE *imageFile = fopen(imageName, "wb");
    const unsigned char byteZero = 0;
    int i, j, m, n, k;
    unsigned char minByte;

    fwrite(image -> bf, sizeof(bmp_fileheader), 1, imageFile);
    fwrite(image -> bi, sizeof(bmp_infoheader), 1, imageFile);
    fseek(imageFile, image -> bf -> imageDataOffset, SEEK_SET);

    for (i = 0; i < image -> bi -> height; i++) {
        for (j = 0; j < image -> bi -> width; j++) {
            /*
             * filter pseudomatrix's coordinates, relative to the pixel's coordinates
             * (upper left in the filter matrix)
             */
            const int upperLeftY = i - filterSize / 2;
            const int upperLeftX = j - filterSize / 2;
            for (k = 0; k < PIXEL_LEN; k++) {
                minByte = 255;
                for (m = upperLeftY; m < upperLeftY + filterSize; m++) {
                    for (n = upperLeftX; n < upperLeftX + filterSize; n++) {
                        if (inBounds(image, n, m) == 1) {
                            minByte = MIN(minByte,
                                    image -> bitmap[m][n].bgr[k]);
                        } else {
                            minByte = 0;
                            m = upperLeftY + filterSize;
                            n = upperLeftX + filterSize;
                        }
                    }
                }
                fwrite(&minByte, sizeof(unsigned char), 1, imageFile);
            }
        }
        for (j = 0; j < image -> padding; j++) {
            fwrite(&byteZero, sizeof(unsigned char), 1, imageFile);
        }
    }

    fclose(imageFile);
}

void maxPooling(imageData *image, char *const imageName,
                const unsigned char filterSize) {
    FILE *imageFile = fopen(imageName, "wb");
    const unsigned char byteZero = 0;
    int i, j, m, n, k;
    unsigned char maxByte;

    fwrite(image -> bf, sizeof(bmp_fileheader), 1, imageFile);
    fwrite(image -> bi, sizeof(bmp_infoheader), 1, imageFile);
    fseek(imageFile, image -> bf -> imageDataOffset, SEEK_SET);

    for (i = 0; i < image -> bi -> height; i++) {
        for (j = 0; j < image -> bi -> width; j++) {
            /*
             * filter pseudomatrix's coordinates, relative to the pixel's coordinates
             * (upper left in the filter matrix)
             */
            const int upperLeftY = i - filterSize / 2;
            const int upperLeftX = j - filterSize / 2;
            for (k = 0; k < PIXEL_LEN; k++) {
                maxByte = 0;
                for (m = upperLeftY; m < upperLeftY + filterSize; m++) {
                    for (n = upperLeftX; n < upperLeftX + filterSize; n++) {
                        if (inBounds(image, n, m) == 1) {
                            maxByte = MAX(maxByte,
                                    image -> bitmap[m][n].bgr[k]);
                        }
                    }
                }
                fwrite(&maxByte, sizeof(unsigned char), 1, imageFile);
            }
        }
        for (j = 0; j < image -> padding; j++) {
            fwrite(&byteZero, sizeof(unsigned char), 1, imageFile);
        }
    }
    fclose(imageFile);
}

int getDiff(pixel *x, pixel *y) {
    int i, s = 0;
    for (i = 0; i < PIXEL_LEN; i++) {
        s += ABS_DIFF(x -> bgr[i], y -> bgr[i]);
    }
    return s;
}

void modifyBitmap(imageData **image, pair **storage, int init, int **sum,
                int *sizeStorage) {
    int i, k;
    for (i = init; i <= (*sizeStorage); i++) {
        for (k = 0; k < PIXEL_LEN; k++) {
            (*image) -> bitmap[(*storage)[i].y][(*storage)[i].x].bgr[k] = (*sum)[k];
        }
    }
}

void exploreBitmap(imageData **image, int ***visited, int threshold,
                    int **directionsX, int **directionsY, int **sum, pair **storage,
                    int *sizeStorage, int x, int y) {
    int i, k, dirX, dirY, difference, it;

    for (k = 0; k < PIXEL_LEN; k++) {
        (*sum)[k] += (*image) -> bitmap[y][x].bgr[k]; // reinitializing "sum"
    }

    (*storage)[++(*sizeStorage)].x = x;
    (*storage)[(*sizeStorage)].y = y;
    (*visited)[y][x] = 1;
    int pixelsNo = 1;

    int init = (*sizeStorage);
    for (it = init; it <= (*sizeStorage); it++) {
        for (i = 0; i < DIRECTIONS_NUMBER; i++) {
            dirX = (*storage)[it].x + (*directionsX)[i];
            dirY = (*storage)[it].y + (*directionsY)[i];
            if (inBounds(*image, dirX, dirY) == 1) {
                if ((*visited)[dirY][dirX] == 0) {
                    difference = getDiff(&(*image) -> bitmap[y][x],
                                &(*image) -> bitmap[dirY][dirX]);
                    if (difference <= threshold) {
                        ++(*sizeStorage);
                        (*storage)[(*sizeStorage)].x = dirX;
                        (*storage)[(*sizeStorage)].y = dirY;

                        for (k = 0; k < PIXEL_LEN; k++) {
                            (*sum)[k] += (*image) -> bitmap[dirY][dirX].bgr[k];
                        }

                        (*visited)[dirY][dirX] = 1;
                        pixelsNo++;
                    }
                }
            }
        }
    }

    // obtaining the medial sum
    for (k = 0; k < PIXEL_LEN; k++) {
        (*sum)[k] /= pixelsNo;
    }

    modifyBitmap(image, storage, init, sum, sizeStorage);
}

void clusterImage(imageData *image, char *const imageName, int threshold) {
    int *directionsX = (int*)calloc(DIRECTIONS_NUMBER, sizeof(int));
    if (directionsX == NULL) {
        return;
    }
    directionsX[0] = -1;
    directionsX[1] = 1;
    directionsX[2] = directionsX[3] = 0;

    int *directionsY = (int*)calloc(DIRECTIONS_NUMBER, sizeof(int));
    if (directionsY == NULL) {
        return;
    }
    directionsY[2] = 1;
    directionsY[3] = -1;
    directionsY[0] = directionsY[1] = 0;

    int totalPixels = image -> bi -> height * image -> bi -> width;

    int **visited = (int**)calloc(image -> bi -> height, sizeof(int*));
    if (visited == NULL) {
        return;
    }
    int i;
    for (i = 0; i < image -> bi -> height; i++) {
        visited[i] = (int*)calloc(image -> bi -> width, sizeof(int));
        if (visited[i] == NULL) {
            return;
        }
    }

    int *sum = (int*)calloc(PIXEL_LEN, sizeof(int));
    if (sum == NULL) {
        return;
    }

    pair *storage = (pair*)calloc(totalPixels + 1, sizeof(pair));
    int sizeStorage = 0;

    int j, k;
    for (i = image -> bi -> height - 1; i >= 0; i--) {
        for (j = 0; j < image -> bi -> width; j++) {
            if (visited[i][j] == 0) {
                for (k = 0; k < PIXEL_LEN; k++) {
                    sum[k] = 0;
                }
                exploreBitmap(&image, &visited, threshold, &directionsX,
                            &directionsY, &sum, &storage, &sizeStorage, j, i);
            }
        }
    }

    writeImage(image, imageName);

    free(sum);
    sum = NULL;

    for (i = 0; i < image -> bi -> height; i++) {
        free(visited[i]);
    }
    free(visited);
    visited = NULL;

    free(directionsX);
    directionsX = NULL;

    free(directionsY);
    directionsY = NULL;

    free(storage);
    storage = NULL;
}
