#ifndef CPOPEN_H
#define CPOPEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define ERROR(str){ printf("error: %s", str); exit(1); };
#define MAX_LINE_LENGTH 255

static int childStatus = -1;

FILE *_popen(const char*, const char*);
int _pclose(FILE*);

#endif