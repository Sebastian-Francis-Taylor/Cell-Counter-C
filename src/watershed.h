#ifndef WATERSHED_H
#define WATERSHED_H

#include "cbmp.h"
#define UNLABELED 0
#define WATERSHED_LINE 254
#define MINIMA_BUFFER_SIZE (BMP_WIDTH * BMP_HEIGHT)

void distance_transform(unsigned char input[BMP_WIDTH][BMP_HEIGHT],
                        int distance[BMP_WIDTH][BMP_HEIGHT]);

void find_regional_minima(int distance[BMP_WIDTH][BMP_HEIGHT],
                          int labels[BMP_WIDTH][BMP_HEIGHT], int *num_labels);

void watershed_segmentation(int distance[BMP_WIDTH][BMP_HEIGHT],
                            int labels[BMP_WIDTH][BMP_HEIGHT]);

int watershed_erode(unsigned char input[BMP_WIDTH][BMP_HEIGHT],
                    unsigned char output[BMP_WIDTH][BMP_HEIGHT]);

#endif
