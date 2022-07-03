#ifndef HASHSET_H_
#define HASHSET_H_

// Linked list for set
typedef struct set_node {
    struct set_node* next;
    int* vals;

} SetNode;

typedef struct set {
    int capacity;
    int* size;
    struct set_node** buckets;
} Set;

Set set_create(int buckers);
    /*
    Create the Set with the specified number of buckets.
    */

void set_destroy(Set s);
    /*
    Destroy the set and free any heap allocated memory.
    */

void set_add(Set s, int x, int y);
    /*
    Add the x, y pair to the set.
    */

void set_remove(Set s, int x, int y);
    /*
    Remove the x, y pair from the set.
    */

int *set_random(Set s);
    /*
    Get a random element from the set.
    */

#endif