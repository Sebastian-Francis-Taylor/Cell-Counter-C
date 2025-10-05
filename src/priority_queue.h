#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#define BLACK 0
#define WHITE 255
#define MINIMA_BUFFER_SIZE (BMP_WIDTH * BMP_HEIGHT)
#define WATERSHED_LINE WHITE - 1

typedef struct {
  int x;
  int y;
  int priority;
} QueueNode;

typedef struct {
  QueueNode *nodes;
  int size;
  int capacity;
} PriorityQueue;

PriorityQueue *priority_queue_create(int capacity);

void priority_queue_free(PriorityQueue *queue);

void priority_queue_push(PriorityQueue *queue, int x, int y, int priority);

QueueNode priority_queue_pop(PriorityQueue *queue);

int priority_queue_is_empty(PriorityQueue *queue);

#endif
