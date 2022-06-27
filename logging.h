#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdio.h>

typedef struct log_files {
    void* opaque;
    void (*logging)(struct log_files l);
    void (*logging_destroy)(struct log_files l);
    char log_str[BUFSIZ];
} Log;

void logging(Log l);

void logging_create(Log* l, char* log_path);

void logging_destroy(Log l);

#endif