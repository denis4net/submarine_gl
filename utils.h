#ifndef UTILS_H
#define UTILS_H
#include <cstdio>

#define LOG(...) fprintf(stderr, "L: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");
#define ERROR(...) fprintf(stderr, "E: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");
#endif // UTILS_H
