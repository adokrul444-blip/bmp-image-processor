#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "transformations.h"

static void print_usage(const char* prog_name) {
    fprintf(stderr,
            "Usage: %s [-i input.bmp] [-o output.bmp] [OPTIONS]\n"
            "Options:\n"
            "  -r                  Rotate 90 degrees clockwise\n"
            "  -l                  Rotate 90 degrees counter-clockwise\n"
            "  -fh                 Flip horizontally\n"
            "  -fv                 Flip vertically\n"
            "  -s <factor>         Scale image by positive float factor\n"
            "  -c <y> <x> <h> <w>  Crop rectangular region\n"
            "  -e <rgb>            Extract color channels (combination of r, g, b)\n"
            "  -h, --help          Show this help message\n",
            prog_name);
}

static bool replace_image(struct bmp_image** current, struct bmp_image* next) {
    if (!next) {
        fprintf(stderr, "Error: Image transformation failed (invalid parameters or out of memory).\n");
        free_bmp_image(*current);
        *current = NULL;
        return false;
    }
    free_bmp_image(*current);
    *current = next;
    return true;
}

int main(int argc, char* argv[]) {
    FILE* input = stdin;
    FILE* output = stdout;
    int exit_code = EXIT_SUCCESS;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            if (input != stdin) fclose(input);
            input = fopen(argv[++i], "rb");
            if (!input) {
                perror("Error opening input file");
                if (output != stdout) fclose(output);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            if (output != stdout) fclose(output);
            output = fopen(argv[++i], "wb");
            if (!output) {
                perror("Error opening output file");
                if (input != stdin) fclose(input);
                return EXIT_FAILURE;
            }
        }
    }

    struct bmp_image* image = read_bmp(input);
    if (!image) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "-o") == 0) {
            i++;
        } else if (strcmp(argv[i], "-r") == 0) {
            if (!replace_image(&image, rotate_right(image))) { exit_code = EXIT_FAILURE; goto cleanup; }
        } else if (strcmp(argv[i], "-l") == 0) {
            if (!replace_image(&image, rotate_left(image))) { exit_code = EXIT_FAILURE; goto cleanup; }
        } else if (strcmp(argv[i], "-fh") == 0) {
            if (!replace_image(&image, flip_horizontally(image))) { exit_code = EXIT_FAILURE; goto cleanup; }
        } else if (strcmp(argv[i], "-fv") == 0) {
            if (!replace_image(&image, flip_vertically(image))) { exit_code = EXIT_FAILURE; goto cleanup; }
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            float factor = strtof(argv[++i], NULL);
            if (!replace_image(&image, scale(image, factor))) { exit_code = EXIT_FAILURE; goto cleanup; }
        } else if (strcmp(argv[i], "-c") == 0 && i + 4 < argc) {
            uint32_t sy = (uint32_t)strtoul(argv[++i], NULL, 10);
            uint32_t sx = (uint32_t)strtoul(argv[++i], NULL, 10);
            uint32_t h  = (uint32_t)strtoul(argv[++i], NULL, 10);
            uint32_t w  = (uint32_t)strtoul(argv[++i], NULL, 10);
            if (!replace_image(&image, crop(image, sy, sx, h, w))) { exit_code = EXIT_FAILURE; goto cleanup; }
        } else if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            if (!replace_image(&image, extract(image, argv[++i]))) { exit_code = EXIT_FAILURE; goto cleanup; }
        } else {
            fprintf(stderr, "Unknown or incomplete option: %s\n", argv[i]);
            print_usage(argv[0]);
            exit_code = EXIT_FAILURE;
            goto cleanup;
        }
    }

    if (!write_bmp(output, image)) {
        fprintf(stderr, "Error: Failed to write output BMP file.\n");
        exit_code = EXIT_FAILURE;
    }

cleanup:
    free_bmp_image(image);
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);
    return exit_code;
}
