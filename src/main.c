#include "cbmp.h"
#include <stdio.h>
#include <stdlib.h>

#define THRESHOLD 127
const int PATTERN[3][3] = {{0, 1, 0}, {1, 1, 1}, {0, 1, 0}};

#define PATTERN_SIZE 3 // needs to be odd
#define PATTERN_CENTER ((PATTERN_SIZE - 1) << 1)

#define TRUE 1
#define FALSE 0
#define SEARCH_WINDOW 14

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
static unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH];

void invert(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
    for (int x = 0; x < BMP_WIDTH; x++) {
        for (int y = 0; y < BMP_HEIGTH; y++) {
            for (int c = 0; c < BMP_CHANNELS; c++) {
                output_image[x][y][c] = 255 - input_image[x][y][c];
            }
        }
    }
}

// Applying the threshold on all pixels
static void apply_threshold(unsigned int threshold, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]) {
    unsigned char output_image[BMP_WIDTH][BMP_HEIGTH];
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGTH; ++y) {
            output_image[x][y] = input_image[x][y];
        }
    }
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGTH; ++y) {
            output_image[x][y] = (input_image[x][y] <= threshold) ? 0 : 255;
        }
    }
}

// Eroding the image once
void erode_image(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]) {
    unsigned char output_image[BMP_WIDTH][BMP_HEIGTH];
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGTH; ++y) {
            output_image[x][y] = input_image[x][y];
        }
    }
    for (int x = 0; x < BMP_WIDTH; ++x) { // ooof et 4x nested loop, not good :(((
        for (int y = 0; y < BMP_HEIGTH; ++y) {

            if (input_image[x][y] == 0) {
                for (int _x = 0; x < PATTERN_SIZE; ++x) {
                    for (int _y = 0; y < PATTERN_SIZE; ++y) {
                        output_image[x][y] = (PATTERN[_x][_y] == 1 && input_image[_x][_y] >= 1) * 255;
                    }
                }
            }
        }
    }
}

void greyscale_bitmap(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGTH; ++y) {
            unsigned int channel_sum = 0;
            for (int c = 0; c < BMP_CHANNELS; ++c) {
                channel_sum += (unsigned int)input_image[x][y][c];
            }
            greyscale_image[x][y] = channel_sum / BMP_CHANNELS;
        }
    }
}

int has_white_pixel(unsigned char image[BMP_WIDTH][BMP_HEIGTH], int start_x, int start_y) {
    int has_pixel_in_interior = 0;
    int has_pixel_on_perimeter = 0;

    for (int i = 0; i < SEARCH_WINDOW; i++) {
        for (int j = 0; j < SEARCH_WINDOW; j++) {
            int current_x = start_x + i;
            int current_y = start_y + j;

            // Check if current position is on the perimeter
            int is_perimeter = (i == 0 || i == SEARCH_WINDOW - 1 || j == 0 || j == SEARCH_WINDOW - 1);

            if (image[current_x][current_y] == 1) {
                if (is_perimeter) {
                    has_pixel_on_perimeter = 1;
                } else {
                    has_pixel_in_interior = 1;
                }
            }
        }
    }

    // Return TRUE only if there's a white pixel in interior AND no white pixels on perimeter
    return (has_pixel_in_interior && !has_pixel_on_perimeter) ? TRUE : FALSE;
}

void detect_spots(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]) {
    int cells_found = 0;
    // Stops the search before the array is out of bounds
    for (int x = 0; x <= BMP_WIDTH - SEARCH_WINDOW; x++) {
        for (int y = 0; y <= BMP_HEIGTH - SEARCH_WINDOW; y++) {
            if (has_white_pixel(input_image, x, y)) {
                printf("White pixel detected at window: (%d, %d)\n", x, y);
                cells_found += 1;
            }
        }
    }
    printf("Total cells found: %d\n", cells_found);
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
    write_bitmap(input_image, "step_0.bmp");

    greyscale_bitmap(input_image);
    apply_threshold(1, greyscale_image);

    // Temp loop just for showcase
    while (1) {
        erode_image(greyscale_image);
        detect_spots(greyscale_image);
    }

    // Save image to file
    write_bitmap(output_image, argv[2]);

    printf("Done!\n");
    return 0;
}
