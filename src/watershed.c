#include "cbmp.h"
#include "priority_queue.h"
#include <stdio.h>
#include <string.h>

void distance_transform(unsigned char input[BMP_WIDTH][BMP_HEIGHT], int distance[BMP_WIDTH][BMP_HEIGHT]) {
    const int INF = BMP_WIDTH + BMP_HEIGHT;

    // Initialize: background=0, foreground=INF
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            distance[x][y] = (input[x][y] == WHITE) ? INF : 0;
        }
    }

    // Forward pass
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            if (distance[x][y] == 0) continue;

            int min_dist = distance[x][y];

            if (x > 0) {
                min_dist = (distance[x - 1][y] + 1 < min_dist) ? distance[x - 1][y] + 1 : min_dist;
            }
            if (y > 0) {
                min_dist = (distance[x][y - 1] + 1 < min_dist) ? distance[x][y - 1] + 1 : min_dist;
            }
            if (x > 0 && y > 0) {
                min_dist = (distance[x - 1][y - 1] + 1 < min_dist) ? distance[x - 1][y - 1] + 1 : min_dist;
            }
            if (x < BMP_WIDTH - 1 && y > 0) {
                min_dist = (distance[x + 1][y - 1] + 1 < min_dist) ? distance[x + 1][y - 1] + 1 : min_dist;
            }

            distance[x][y] = min_dist;
        }
    }

    // Backward pass
    for (int x = BMP_WIDTH - 1; x >= 0; --x) {
        for (int y = BMP_HEIGHT - 1; y >= 0; --y) {
            if (distance[x][y] == 0) continue;

            int min_dist = distance[x][y];

            if (x < BMP_WIDTH - 1) {
                min_dist = (distance[x + 1][y] + 1 < min_dist) ? distance[x + 1][y] + 1 : min_dist;
            }
            if (y < BMP_HEIGHT - 1) {
                min_dist = (distance[x][y + 1] + 1 < min_dist) ? distance[x][y + 1] + 1 : min_dist;
            }
            if (x < BMP_WIDTH - 1 && y < BMP_HEIGHT - 1) {
                min_dist = (distance[x + 1][y + 1] + 1 < min_dist) ? distance[x + 1][y + 1] + 1 : min_dist;
            }
            if (x > 0 && y < BMP_HEIGHT - 1) {
                min_dist = (distance[x - 1][y + 1] + 1 < min_dist) ? distance[x - 1][y + 1] + 1 : min_dist;
            }

            distance[x][y] = min_dist;
        }
    }
}

void find_local_maxima(int distance[BMP_WIDTH][BMP_HEIGHT], int labels[BMP_WIDTH][BMP_HEIGHT], int *labels_amount) {
    int current_label = 0;

    // Initialize labels
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            labels[x][y] = 0;
        }
    }

    // Find local maxima (centers of cells)
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            if (distance[x][y] <= 1) continue; // Skip background and edges

            int is_maximum = 1;
            int dx[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
            int dy[8] = {0, 0, -1, 1, -1, 1, -1, 1};

            for (int i = 0; i < 8; ++i) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (nx >= 0 && nx < BMP_WIDTH && ny >= 0 && ny < BMP_HEIGHT) {
                    if (distance[nx][ny] > distance[x][y]) {
                        is_maximum = 0;
                        break;
                    }
                }
            }

            if (is_maximum && distance[x][y] >= 7) { // Minimum distance threshold
                current_label++;
                labels[x][y] = current_label;
            }
        }
    }

    *labels_amount = current_label;
    printf("[ DEBUG ] Local maxima found: %d\n", current_label);
}

void watershed_segmentation(int distance[BMP_WIDTH][BMP_HEIGHT], int labels[BMP_WIDTH][BMP_HEIGHT]) {
    int num_labels;
    find_local_maxima(distance, labels, &num_labels);

    if (num_labels == 0) {
        printf("[ DEBUG ] No seeds found\n");
        return;
    }

    PriorityQueue *pq = priority_queue_create(BMP_WIDTH * BMP_HEIGHT);
    if (!pq) return;

    // Add all seed pixels to queue with NEGATIVE priority (for max-heap behavior)
    for (int x = 0; x < BMP_WIDTH; x++) {
        for (int y = 0; y < BMP_HEIGHT; y++) {
            if (labels[x][y] > 0) {
                priority_queue_push(pq, x, y, -distance[x][y]);
            }
        }
    }

    int dx[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dy[8] = {0, 0, -1, 1, -1, 1, -1, 1};

    // Process queue from high distance to low distance
    while (!priority_queue_is_empty(pq)) {
        QueueNode node = priority_queue_pop(pq);
        int x = node.x;
        int y = node.y;
        int current_label = labels[x][y];

        // Process neighbors
        for (int i = 0; i < 8; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (nx >= 0 && nx < BMP_WIDTH && ny >= 0 && ny < BMP_HEIGHT) {
                // Only process unlabeled foreground pixels
                if (labels[nx][ny] == 0 && distance[nx][ny] > 0) {
                    // Check for watershed line
                    int has_different_neighbor = 0;
                    for (int j = 0; j < 8; j++) {
                        int nnx = nx + dx[j];
                        int nny = ny + dy[j];
                        if (nnx >= 0 && nnx < BMP_WIDTH && nny >= 0 && nny < BMP_HEIGHT) {
                            if (labels[nnx][nny] > 0 && labels[nnx][nny] != WATERSHED_LINE && labels[nnx][nny] != current_label) {
                                has_different_neighbor = 1;
                                break;
                            }
                        }
                    }

                    if (has_different_neighbor) {
                        labels[nx][ny] = WATERSHED_LINE;
                    } else {
                        labels[nx][ny] = current_label;
                        priority_queue_push(pq, nx, ny, -distance[nx][ny]);
                    }
                }
            }
        }
    }

    priority_queue_free(pq);
}

int watershed_erode(unsigned char input[BMP_WIDTH][BMP_HEIGHT], unsigned char output[BMP_WIDTH][BMP_HEIGHT]) {
    static int distance[BMP_WIDTH][BMP_HEIGHT];
    static int labels[BMP_WIDTH][BMP_HEIGHT];
    int eroded_any = 0;

    // Copy input to output initially
    memcpy(output, input, BMP_WIDTH * BMP_HEIGHT);

    distance_transform(input, distance);
    watershed_segmentation(distance, labels);

    // Apply watershed lines as erosion
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            if (labels[x][y] == WATERSHED_LINE) {
                output[x][y] = 128; // Erode watershed lines
                eroded_any = 1;
            }
        }
    }

    // Debug info
    int watershed_count = 0;
    for (int x = 0; x < BMP_WIDTH; ++x) {
        for (int y = 0; y < BMP_HEIGHT; ++y) {
            if (labels[x][y] == WATERSHED_LINE) watershed_count++;
        }
    }
    printf("[ DEBUG ] Watershed lines: %d pixels\n", watershed_count);

    return eroded_any;
}
