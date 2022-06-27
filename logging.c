#include "logging.h"
#include <stdio.h>

void logging(Log l) {
    fputs(l.log_str, (FILE*)l.opaque);
}

void logging_create(Log* l, char* log_path) {
    l->logging = logging;
    l->logging_destroy = logging_destroy;
    l->opaque = (void*)fopen(log_path, "w");
}

void logging_destroy(Log l) {
    fclose((FILE*)l.opaque);
}
