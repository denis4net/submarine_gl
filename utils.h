#ifndef UTILS_H
#define UTILS_H
#include <cstdio>

#define LOG(x, ...) fprintf(stderr, "L: "); fprintf(stderr, x, __VA_ARGS__); fprintf(stderr, "\n");
#define ERROR(x, ...) fprintf(stderr, "E: "); fprintf(stderr, x, __VA_ARGS__) fprintf(stderr, "\n");
#endif // UTILS_H
