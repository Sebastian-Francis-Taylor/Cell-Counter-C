#include <stdio.h>
#include <stdlib.h>

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

void swap_node(QueueNode *a, QueueNode *b) {
    QueueNode temp = *a;
    *a = *b;
    *b = temp;
}

void priority_queue_push(PriorityQueue *queue, int x, int y, int priority) {
    if (queue->size >= queue->capacity) {
        printf("[ DEBUG ] PriorityQueue is full!\n");
        return; // Queue is full
    }
    int i = queue->size;
    queue->nodes[i].x = x;
    queue->nodes[i].y = y;
    queue->nodes[i].priority = priority;
    queue->size++;

    while (i > 0) {
        int parent = (i - 1) >> 1; // /2
        if (queue->nodes[i].priority >= queue->nodes[parent].priority) {
            break;
        }

        swap_node(&queue->nodes[i], &queue->nodes[parent]);
    }
}

QueueNode priority_queue_pop(PriorityQueue *queue) {

    QueueNode result = queue->nodes[0];

    queue->size--;
    queue->nodes[0] = queue->nodes[queue->size];

    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < queue->size && queue->nodes[left].priority < queue->nodes[smallest].priority) {
            smallest = left;
        }

        if (right < queue->size && queue->nodes[right].priority < queue->nodes[smallest].priority) {

            smallest = right;
        }

        if (smallest == i) {
            break;
        }

        swap_node(&queue->nodes[i], &queue->nodes[smallest]);
        i = smallest;
    }

    return result;
}

PriorityQueue *priority_queue_create(int capacity) {
    PriorityQueue *queue = malloc(sizeof(PriorityQueue));
    if (!queue) return NULL;

    queue->nodes = malloc(capacity * sizeof(QueueNode));
    if (!queue->nodes) {
        free(queue);
        return NULL;
    }

    queue->size = 0;
    queue->capacity = capacity;
    return queue;
}

void priority_queue_free(PriorityQueue *queue) {
    if (queue) {
        free(queue->nodes);
        free(queue);
    }
}

int priority_queue_is_empty(PriorityQueue *queue) { return queue->size == 0; }
