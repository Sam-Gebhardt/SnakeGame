#ifndef HASHSET_H_
#define HASHSET_H_

// Linked list for set
typedef struct set_node {
    struct set_node* next;
    int* vals;

} SetNode;

typedef struct set {
    int size;
    struct set_node** buckets;
} Set;

Set set_create(int buckers);

void set_destroy(Set s);

void set_add(Set s, int x, int y);

void set_remove(Set s, int x, int y);

int *set_random(Set s);

#endif