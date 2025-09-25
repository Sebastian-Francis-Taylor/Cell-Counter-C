#include "cbmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define THRESHOLD 127
const int PATTERN[3][3] = {{0, 1, 0}, {1, 1, 1}, {0, 1, 0}};

#define PATTERN_SIZE 3 // needs to be odd
#define PATTERN_CENTER ((PATTERN_SIZE - 1) / 2) + 1

#define WHITE 255
#define BLACK 0

#define TRUE 1
#define FALSE 0
#define SEARCH_WINDOW 14
#define BINARY_THRESHOLD 127

// change to theoretical max for cells in a 950 x 950 image
#define MAX_COORDINATES 4700

typedef struct {
    int x;
    int y;
} Coordinates;

static Coordinates coordinates[MAX_COORDINATES];
static int coordinates_amount = 0;

void add_coordinate(int x, int y) {
    if (coordinates_amount < MAX_COORDINATES) {
        coordinates[coordinates_amount].x = x;
        coordinates[coordinates_amount].y = y;
        coordinates_amount += 1;
        printf("White pixel detected at position: (%d, %d)\n", x, y);
    }
}

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
static unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
static unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
static unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT];
static unsigned char binary_image[BMP_WIDTH][BMP_HEIGHT];

void save_greyscale_image(unsigned char image[BMP_WIDTH][BMP_HEIGHT], char *save_path) {
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            for (int z = 0; z < BMP_CHANNELS; ++z) {
                output_image[x][y][z] = image[x][y];
            }
        }
    }

    write_bitmap(output_image, save_path);
}

// Applying the threshold on all pixels
static void apply_threshold(unsigned int threshold, unsigned char input_image[BMP_WIDTH][BMP_HEIGHT],
                            unsigned char output_image[BMP_WIDTH][BMP_HEIGHT]) {
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            output_image[x][y] = (input_image[x][y] <= threshold) ? 0 : 255;
        }
    }
}

int erode_image(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT]) {
    int eroded_any = 0; // Track if any pixel was eroded

    // Copy input image to output as default
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            output_image[x][y] = input_image[x][y];
        }
    }

    // Perform erosion and avoid borders
    for (int x = 1; x < BMP_WIDTH - 1; ++x) {
        for (int y = 1; y < BMP_HEIGHT - 1; ++y) {
            if (input_image[x][y] == WHITE) {
                int survives = 1;

                // Check pattern
                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        if (PATTERN[i + 1][j + 1] == 1) {
                            if (input_image[x + i][y + j] != WHITE) {
                                survives = 0;
                                break;
                            }
                        }
                    }
                    if (!survives) break;
                }

                if (!survives) {
                    output_image[x][y] = BLACK;
                    eroded_any = 1;
                }
            }
        }
    }
    // return 1 if any pixel was eroded, 0 otherwise
    return eroded_any;
}

void greyscale_bitmap(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]) {
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            unsigned int channel_sum = 0;
            for (int c = 0; c < BMP_CHANNELS; ++c) {
                channel_sum += (unsigned int)input_image[x][y][c];
            }
            greyscale_image[x][y] = channel_sum / BMP_CHANNELS;
        }
    }
}

int has_white_pixel(unsigned char image[BMP_WIDTH][BMP_HEIGHT], int start_x, int start_y) {
    int has_pixel_in_interior = 0;
    int has_pixel_on_perimeter = 0;

    for (int x = 0; x < SEARCH_WINDOW; x++) {
        for (int y = 0; y < SEARCH_WINDOW; y++) {
            int current_x = start_x + x;
            int current_y = start_y + y;

            // Check if current position is on the perimeter
            int on_perimeter = (x == 0 || x == SEARCH_WINDOW - 1 || y == 0 || y == SEARCH_WINDOW - 1);

            if (image[current_x][current_y] == 255) {
                if (on_perimeter) {
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

void remove_spot(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], int start_x, int start_y) {
    for (int x = 0; x < SEARCH_WINDOW; ++x) {
        for (int y = 0; y < SEARCH_WINDOW; ++y) {
            input_image[start_x + x][start_y + y] = 0;
        }
    }
}

int detect_spots(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT]) {
    int cells_found = 0;
    // Stops the search before the array is out of bounds
    for (int x = 0; x <= BMP_WIDTH - SEARCH_WINDOW; x++) {
        for (int y = 0; y <= BMP_HEIGHT - SEARCH_WINDOW; y++) {
            if (has_white_pixel(input_image, x, y)) {
                add_coordinate(x, y);
                remove_spot(input_image, x, y);
                cells_found += 1;
            }
        }
    }
    return cells_found;
}

void generate_output_image(unsigned char image[BMP_WIDTH][BMP_HEIGHT]) {
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            for (int z = 0; z < BMP_CHANNELS; ++z) {
                output_image[x][y][z] = image[x][y];
            }
        }
    }

    for (int i = 0; i < coordinates_amount; ++i) {
        int cx = coordinates[i].x;
        int cy = coordinates[i].y;

        // draws the horizontal line of the +
        for (int j = -5; j <= 5; j++) {
            output_image[cx + j][cy][0] = 255; // Red
        }

        // draws the vertical line of the +
        for (int j = -5; j <= 5; j++) {
            output_image[cx][cy + j][0] = 255; // Red
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
        fprintf(stderr, "Usage: %s <input file path> <output file path>\n", argv[0]);
        exit(1);
    }

    printf("Cell Counter - Bateman Boys\n");

    // Load image from file
    read_bitmap(argv[1], input_image);
    write_bitmap(input_image, "step_0.bmp");

    greyscale_bitmap(input_image);

    // copy to output image
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            binary_image[x][y] = greyscale_image[x][y];
        }
    }

    apply_threshold(BINARY_THRESHOLD, greyscale_image, binary_image);
    save_greyscale_image(binary_image, "output/stage_0.bmp");

    unsigned char binary_image_2[BMP_WIDTH][BMP_HEIGHT];
    // copy to binary 2 image
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            binary_image_2[x][y] = binary_image[x][y];
        }
    }
    int total_cells = 0;
    for (int i = 1; i <= 10; ++i) {
        erode_image((i % 2 == 0 ? binary_image : binary_image_2), (i % 2 == 0 ? binary_image_2 : binary_image));
        int cells_found = detect_spots((i % 2 == 0 ? binary_image_2 : binary_image));
        total_cells += cells_found;
        char save_path[256];
        snprintf(save_path, sizeof(save_path), "output/stage_%d.bmp", i);
        save_greyscale_image((i % 2 == 0 ? binary_image_2 : binary_image), save_path);
    }

    printf("%d cells found in sample image '%s'\n", total_cells, argv[1]);

    generate_output_image(greyscale_image);

    // Save image to file
    // save_greyscale_image(output_image, argv[2]);

    printf("Done!\n");
    return 0;
}
