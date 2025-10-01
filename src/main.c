#include "cbmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define THRESHOLD 127
const int PATTERN[3][3] = {{0, 1, 0}, {1, 1, 1}, {0, 1, 0}};

#define PATTERN_SIZE 3 // needs to be odd

#define WHITE 255
#define BLACK 0

#define CROSS_HYPOTENUSE 20

#define TRUE 1
#define FALSE 0
#define SEARCH_WINDOW 14

#define MAX_COORDINATES 4700

#ifdef TIMING
#define START_TIMER() clock_t timer_start = clock()
#define END_TIMER(label)                                                                                                                             \
    do {                                                                                                                                             \
        clock_t timer_end = clock();                                                                                                                 \
        double cpu_time = ((double)(timer_end - timer_start)) / CLOCKS_PER_SEC;                                                                      \
        printf("[ %-5s ] %s took %f seconds\n", "TIME", label, cpu_time);                                                                            \
    } while (0)
#else
#define START_TIMER()
#define END_TIMER(label)
#endif

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
    }
}

void print_coordinate() {
    for (int i = 0; i < coordinates_amount; ++i) {
        int x = coordinates[i].x;
        int y = coordinates[i].y;
        printf("[ %-5s ] White pixel detected at position: (%d, %d)\n", "LOG", x, y);
    }
}

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
static unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
static unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
static unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT];
static unsigned char binary_image[BMP_WIDTH][BMP_HEIGHT];

void save_greyscale_image(unsigned char image[BMP_WIDTH][BMP_HEIGHT], char *save_path) {
    START_TIMER();
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            unsigned char pixel = image[x][y]; // stores the pixel so it doesn't need to be read 3 times in loop below
            for (int z = 0; z < BMP_CHANNELS; ++z) {
                output_image[x][y][z] = pixel; // Use stored pixel
            }
        }
    }

    write_bitmap(output_image, save_path);
    END_TIMER("save_greyscale_image");
}

void save_image(unsigned char image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], char *save_path) { write_bitmap(image, save_path); }

unsigned int otsu_threshold(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT]) {
    unsigned int histogram[256] = {0};
    // step 1, create histogram
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            histogram[input_image[x][y]]++;
        }
    }

    // step 2, sum the intensities and count pixels
    unsigned int pixels_sum = 0;
    for (int i = 0; i <= 255; ++i) {
        pixels_sum += i * histogram[i];
    }

    // step 3, initialise variables
    float max_variance = 0.0f;
    unsigned int optimal_threshold = 0;
    unsigned int background_count = 0;
    unsigned int background_sum = 0;
    unsigned int total_pixels = BMP_HEIGHT * BMP_WIDTH;

    // step 4, go through potential thresholds
    for (int i = 0; i <= 255; ++i) {
        background_count = background_count + histogram[i];
        background_sum = background_sum + i * histogram[i];

        unsigned int foreground_count = total_pixels - background_count;
        if (background_count == 0 || foreground_count == 0) {
            continue;
        }

        float background_mean = (float)background_sum / (float)background_count;
        float foreground_mean = (float)(pixels_sum - background_sum) / (float)foreground_count;

        float variance =
            (float)background_count * (float)foreground_count * ((background_mean - foreground_mean) * (background_mean - foreground_mean));
        if (variance > max_variance) {
            max_variance = variance;
            optimal_threshold = i;
        }
    }

    return optimal_threshold;
}

static void apply_threshold(unsigned int threshold, unsigned char input_image[BMP_WIDTH][BMP_HEIGHT],
                            unsigned char output_image[BMP_WIDTH][BMP_HEIGHT]) {
    START_TIMER();
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            output_image[x][y] = (input_image[x][y] <= threshold) ? 0 : 255;
        }
    }
    END_TIMER("apply_threshold");
}

int erode_image(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT]) {
    START_TIMER();
    int eroded_any = 0;

    const int R = PATTERN_SIZE / 2;

    // Finding pattern offsets
    int offsets[PATTERN_SIZE * PATTERN_SIZE][2];
    int n_offsets = 0;
    for (int i = 0; i < PATTERN_SIZE; ++i) {
        for (int j = 0; j < PATTERN_SIZE; ++j) {
            if (PATTERN[i][j]) {
                offsets[n_offsets][0] = i - R;
                offsets[n_offsets][1] = j - R;
                n_offsets++;
            }
        }
    }

    // erode image
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {

            if (input_image[x][y] == WHITE) {
                int survives = 1;

                for (int k = 0; k < n_offsets; ++k) {
                    int nx = x + offsets[k][0];
                    int ny = y + offsets[k][1];

                    // border check
                    if (nx < 0 || ny < 0 || nx >= BMP_WIDTH || ny >= BMP_HEIGHT || input_image[nx][ny] != WHITE) {
                        survives = 0;
                        break;
                    }
                }

                if (!survives) {
                    output_image[x][y] = BLACK;
                    eroded_any = 1;
                } else {
                    output_image[x][y] = WHITE;
                }
            } else {
                output_image[x][y] = BLACK;
            }
        }
    }

    END_TIMER("erode_image");
    return eroded_any;
}

void greyscale_bitmap(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]) {
    START_TIMER();
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            unsigned int channel_sum = 0;
            for (int c = 0; c < BMP_CHANNELS; ++c) {
                channel_sum += (unsigned int)input_image[x][y][c];
            }
            greyscale_image[x][y] = channel_sum >> 2;
        }
    }
    END_TIMER("greyscale_bitmap");
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

void cross(unsigned char image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], Coordinates coordinates[MAX_COORDINATES], unsigned int hypotenuse) {
    int half_hypotenuse = hypotenuse >> 1;

    for (int z = 0; z < coordinates_amount; z++) {
        for (int x = 0; x < hypotenuse; ++x) {
            int x1 = coordinates[z].x + x - half_hypotenuse;
            int y1 = coordinates[z].y + x - half_hypotenuse;
            int x2 = coordinates[z].x - x + half_hypotenuse;
            int y2 = coordinates[z].y + x - half_hypotenuse;

            // Draws first diagonal (\)
            if (x1 >= 0 && x1 < BMP_WIDTH && y1 >= 0 && y1 < BMP_HEIGHT) {
                image[x1][y1][0] = 255; // Red
                image[x1][y1][1] = 0;
                image[x1][y1][2] = 0;
            }

            // Draws second diagonal (/)
            if (x2 >= 0 && x2 < BMP_WIDTH && y2 >= 0 && y2 < BMP_HEIGHT) {
                image[x2][y2][0] = 255; // Red
                image[x2][y2][1] = 0;
                image[x2][y2][2] = 0;
            }
        }
    }
}

void remove_spot(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT], int start_x, int start_y) {
    for (int x = 0; x < SEARCH_WINDOW; ++x) {
        for (int y = 0; y < SEARCH_WINDOW; ++y) {
            input_image[start_x + x][start_y + y] = 0;
        }
    }
}

int detect_spots(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT]) {
    START_TIMER();
    int cells_found = 0;
    // Stops the search before the array is out of bounds
    for (int x = 0; x <= BMP_WIDTH - SEARCH_WINDOW; x++) {
        for (int y = 0; y <= BMP_HEIGHT - SEARCH_WINDOW; y++) {
            if (has_white_pixel(input_image, x, y)) {
                add_coordinate(x + (SEARCH_WINDOW >> 1), y + (SEARCH_WINDOW >> 1));
                remove_spot(input_image, x, y);
                cells_found += 1;
            }
        }
    }
    END_TIMER("detect_spots");
    return cells_found;
}

void generate_output_image(unsigned char image[BMP_WIDTH][BMP_HEIGHT]) {
    // Copy greyscale to RGB
    START_TIMER();
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            for (int z = 0; z < BMP_CHANNELS; ++z) {
                input_image[x][y][z] = image[x][y];
            }
        }
    }

    cross(input_image, coordinates, CROSS_HYPOTENUSE);
    END_TIMER("generate_output_image");
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

    greyscale_bitmap(input_image);
    save_greyscale_image(greyscale_image, "greyscale_image.bmp");

    unsigned int binary_threshold = otsu_threshold(greyscale_image);
    printf("[ %-5s ] binary_threshold (otsu_threshold) = %d\n", "DEBUG", binary_threshold);
    apply_threshold(binary_threshold, greyscale_image, binary_image);
    save_greyscale_image(binary_image, "output/stage_0.bmp");

    unsigned char binary_image_2[BMP_WIDTH][BMP_HEIGHT];
    // copy to binary 2 image
    memcpy(binary_image_2, binary_image, sizeof(binary_image));

    int total_cells = 0;
    // potential optimisation: get rid of ternerary for detect_spots and save_greyscale_image and combine into one computation
    for (int i = 1; i <= 10; ++i) {
        erode_image((i % 2 == 0 ? binary_image : binary_image_2), (i % 2 == 0 ? binary_image_2 : binary_image));
        int cells_found = detect_spots((i % 2 == 0 ? binary_image_2 : binary_image));
        total_cells += cells_found;
        char save_path[256];
        snprintf(save_path, sizeof(save_path), "output/stage_%d.bmp", i);
        save_greyscale_image((i % 2 == 0 ? binary_image_2 : binary_image), save_path);
        if (cells_found <= 0 && i >= 3) { // early break if no more cells are found
            break;
        }
    }

    print_coordinate();
    printf("%d cells found in sample image '%s'\n", total_cells, argv[1]);
    generate_output_image(greyscale_image);

    // Save image to file
    write_bitmap(input_image, argv[2]);

    printf("Done!\n");
    return 0;
}
