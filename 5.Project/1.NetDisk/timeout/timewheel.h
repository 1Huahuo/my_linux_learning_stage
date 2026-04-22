#ifndef __TIMEWHELL_H__
#define __TIMEWHELL_H__

#include <my_header.h>

typedef struct node Node;

typedef struct entry {
    int fd;
    Node *node;
    struct entry *next;
}Entry;

typedef struct {
    Entry **table;
    int size;
    int capacity;
    uint32_t hashseed; 
}HashMap;

struct node {
    int connfd;
    int index;
    struct node *prev;
    struct node *next;
};

typedef struct {
    Node **table;
    int slots;
    int interval;
    int current;
    HashMap fd_map;
    time_t last;
}TimeWheel;

void hash_map_init(HashMap *m);
void hash_map_destroy(HashMap *m);
void hash_map_put(HashMap *m, int fd, Node *node);
Node* hash_map_get(HashMap *m, int fd);
void hash_map_delete(HashMap *m, int fd);

TimeWheel* time_wheel_create(int slots, int interval);
void time_wheel_destroy(TimeWheel *tw);
void time_wheel_add(TimeWheel *tw, int connfd);
void time_wheel_del(TimeWheel *tw, int connfd);
void time_wheel_refresh(TimeWheel *tw, int connfd);
int time_wheel_tick(TimeWheel *tw, time_t now);

#endif
