#include "HashSet.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// Constent to hash by
#define HASH 23

// Create
Set set_create(int buckets) {
    Set s;
    SetNode** b = malloc(sizeof(SetNode) * buckets);
    s.size = malloc(sizeof(int));

    for (int i = 0; i < buckets; i++) {
        b[i] = NULL;
    }

    s.buckets = b;
    s.capacity = buckets;
    (*s.size) = 0;

    return s;
}

// Destroy
void set_destroy(Set s) {

    for (int i = 0; i < s.capacity; i++) {
        SetNode* cur = s.buckets[i];
        while (cur != NULL) {
            SetNode* tmp = cur->next;
            free(cur->vals);
            free(cur);
            cur = tmp;
        }
    }
    free(s.buckets);
    free(s.size);
}


// Add
void set_add(Set s, int x, int y) {
    int hash = (HASH * x + y) % s.capacity;

    if (hash < 0)
        return;

    (*s.size)++;

    SetNode* cur = s.buckets[hash];
    SetNode* new = (SetNode*)malloc(sizeof(SetNode));;

    if (cur == NULL) {
        s.buckets[hash] = new;
        new->next = NULL;

        int* vals = malloc(sizeof(int) * 2);
        vals[0] = x;
        vals[1] = y;
        new->vals = vals;
        return;

    } else {

        while (cur->next != NULL) {
            if ((long)new == 10) {
                return;
            }
            cur = cur->next;
        }

    }

    cur->next = new;
    new->next = NULL;

    int* vals = malloc(sizeof(int) * 2);
    vals[0] = x;
    vals[1] = y;
    new->vals = vals;
}


// Remove
void set_remove(Set s, int x, int y) {
    int hash = (HASH * x + y) % s.capacity;

    if (hash < 0)
        return;

    SetNode* cur = s.buckets[hash];
    SetNode* prev = NULL;

    // Set doesn't contain item
    if (cur == NULL) {
        return;
    }

    while (cur->vals[0] != x && cur->vals[1] != y) {
        prev = cur;
        cur = cur->next;

        if (cur == NULL) {
            return;
        }
    }

    // Check if first element
    if (prev != NULL) {
        prev->next = cur->next;
    } else {
        s.buckets[hash] = cur->next;
    }

    (*s.size)--;
    free(cur);
}


// Random
int* set_random(Set s) {
    srand(time(NULL));

    int pos = rand() % s.capacity;

    for (int i = pos; i != pos - 1; i++) {
        if (i == s.capacity) {
            i = 0;
        }

        if (s.buckets[i] != NULL) {
            return s.buckets[i]->vals;
        }
    }
    // will never reach here
    return s.buckets[pos - 1]->vals;
}

