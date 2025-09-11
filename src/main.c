#include "cbmp.h"
#include <stdio.h>
#include <stdlib.h>

#define THRESHOLD 127
const int PATTERN[3][3] = {
    {0, 1, 0},
    {1, 1, 1},
    {0, 1, 0}
};


#define PATTERN_SIZE 3 // needs to be odd
#define PATTERN_CENTER ((PATTERN_SIZE - 1) << 1)

#define TRUE 1
#define FALSE 0
#define SEARCH_WINDOW 14

void invert(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS],
            unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
    for (int x = 0; x < BMP_WIDTH; x++) {
        for (int y = 0; y < BMP_HEIGTH; y++) {
            for (int c = 0; c < BMP_CHANNELS; c++) {
                output_image[x][y][c] = 255 - input_image[x][y][c];
            }
        }
    }
}

// Applying the threshold on all pixels
unsigned char applyThreshold(unsigned int threshold, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]) {
    unsigned char output_image[BMP_WIDTH][BMP_HEIGTH] = input_image;
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGTH; ++y) {
            if (input_image[x][y] <= THRESHOLD) {
                output_image[x][y] = 0;
            } else {
                output_image[x][y] = 255;
            }
        }
    }
    return output_image;
}

// Eroding the image once
unsigned char erodeImage(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]) {
    unsigned char output_image[BMP_WIDTH][BMP_HEIGTH] = input_image;
    for (int x = 0; x < BMP_WIDTH; ++x) { // ooof et 4x nested loop, not good :(((
        for (int y = 0; y < BMP_HEIGTH; ++y) {

            if (input_image[x][y] == 0) {
                for (int _x = 0; x < PATTERN_SIZE ; ++x) {
                    for (int _y = 0; y < PATTERN_SIZE; ++y) {
                        output_image[x][y] = (PATTERN[_x][_y] == 1 && input_image[_x][_y] >= 1) * 255;
                    }
                }
            }

        }
    }
    return output_image;
}

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

unsigned char *greyscale_bitmap(char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
    static unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH];

    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGTH; ++y) {
            unsigned int channel_sum = 0;
            for (int c = 0; c < BMP_CHANNELS; ++c) {
                channel_sum += (unsigned int)input_image[x][y][c];
            }
            greyscale_image[x][y] = channel_sum / BMP_CHANNELS;
        }
    }
    return (unsigned char *)greyscale_image;
}

int has_white_pixel(unsigned char image[BMP_WIDTH][BMP_HEIGTH], int start_x, int start_y) {
    for (int i = 0; i < SEARCH_WINDOW; i++) {
        for (int j = 0; j < SEARCH_WINDOW; j++) {
            if (image[start_x + i][start_y + j] == 1) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void detect_spots(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]) {
    // Stops the search before the array is out of bounds
    for (int x = 0; x <= BMP_WIDTH - SEARCH_WINDOW; x++) {
        for (int y = 0; y <= BMP_HEIGTH - SEARCH_WINDOW; y++) {
            if (has_white_pixel(input_image, x, y)) {
                printf("White pixel detected at window: (%d, %d)\n", x, y);
            }
        }
    }
}

int main(int argc, char **argv) {
    // argc counts how may arguments are passed
    // argv[0] is a string with the name of the program
    // argv[1] is the first command line argument (input image)
    // argv[2] is the second command line argument (output image)

    // Checking that 2 arguments are passed
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
        exit(1);
    }

    printf("Example program - 02132 - A1\n");

    // Load image from file
    read_bitmap(argv[1], input_image);

    // Run inversion
    invert(input_image, output_image);

    // Save image to file
    write_bitmap(output_image, argv[2]);

    printf("Done!\n");
    return 0;
}
