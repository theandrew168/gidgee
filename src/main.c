#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <linmath/linmath.h>

static void
print_usage(const char* arg0)
{
    printf("usage: %s [options]\n", arg0);
    printf("\n");
    printf("Options:\n");
    printf("  -h --help        print this help\n");
}

int
main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
    }

    printf("Hello, world!\n");
    return EXIT_SUCCESS;
}
