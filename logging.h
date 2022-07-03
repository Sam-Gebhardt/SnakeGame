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
    /*
    Write the message in log_str to the file.
    */

void logging_create(Log* l, char* log_path);
    /*
    Create the logging interface with log_path as the destination for
    messages.
    */

void logging_destroy(Log l);
    /*
    Destory the logging interface and close any open file pointers.
    */
#endif