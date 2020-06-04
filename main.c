#include "bmp_utilities.h"

// obtains an array that stores the input image's names/lcoations
char** readInputFileNames() {
    FILE *inputTextFile = fopen("input.txt", "r");

    /* 
     * inputFileNames[0] - initial image name
     * inputFileNames[1] - name/location of the filter matrix
     * inputFileNames[2] - name/location of the file containing pooling type
     * inputFileNames[3] - name/location of the file containing the threshold
     */
    char **inputFileNames = (char**)calloc(TOTAL_TASKS - 1, sizeof(char*));
    if (inputFileNames == NULL) {
        return NULL;
    }

    int i;
    for (i = 0; i < TOTAL_TASKS - 1; i++) {
        inputFileNames[i] = (char*)calloc(MAX_IMAGE_NAME_LEN, sizeof(char));
        if (inputFileNames[i] == NULL) {
            return NULL;
        }
        fscanf(inputTextFile, "%s", inputFileNames[i]);
    }
    fclose(inputTextFile);

    return inputFileNames;
}

// free the char array storing the input image's names/locations
void freeInputFileNames(char **inputFileNames) {
    if (inputFileNames != NULL) {
        int i;
        for (i = 0; i < TOTAL_TASKS - 1; i++) {
            if (inputFileNames[i] != NULL) {
                free(inputFileNames[i]);
                inputFileNames[i] = NULL;
            }
        }
        free(inputFileNames);
        inputFileNames = NULL;
    }
}

int main() {
    char **inputFileNames = readInputFileNames();
    imageData *image = readImage(inputFileNames[0]);
    char **newImagesNames = getNewImagesNames(inputFileNames[0]);

    blackWhite(image, newImagesNames[0]);

    if (image -> bi -> width > image -> bi -> height) {
        noCropHorizontal(image, newImagesNames[1]);
    } else if (image -> bi -> width < image -> bi -> height) {
        noCropVertical(image, newImagesNames[1]);
    } else {
        writeImage(image, newImagesNames[1]);
    }

    applyFilter(image, newImagesNames[2], inputFileNames[1]);

    FILE *poolingFile = fopen(inputFileNames[2], "r");
    char filterPooling;
    unsigned char filterSize;
    fscanf(poolingFile, "%c %hhu", &filterPooling, &filterSize);
    fclose(poolingFile);
    if (filterPooling == 'M') {
        maxPooling(image, newImagesNames[3], filterSize);
    } else {
        minPooling(image, newImagesNames[3], filterSize);
    }

    FILE *clusterFile = fopen(inputFileNames[3], "r");
    int threshold;
    fscanf(clusterFile, "%d", &threshold);
    fclose(clusterFile);
    clusterImage(image, newImagesNames[4], threshold);

    freeNewImagesNames(newImagesNames);
    freeImage(image);
    freeInputFileNames(inputFileNames);

    return 0;
}
