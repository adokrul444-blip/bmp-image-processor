#include "bmp.h"
#include <stdlib.h>

#define BMP_MAGIC 0x4D42
#define BMP_SUPPORTED_BPP 24

static uint32_t calc_row_size(uint32_t width) {
    return (width * sizeof(struct pixel) + 3) & ~3U;
}

struct bmp_header* read_bmp_header(FILE* stream) {
    if (!stream) return NULL;

    struct bmp_header* header = malloc(sizeof(struct bmp_header));
    if (!header) return NULL;

    if (fread(header, sizeof(struct bmp_header), 1, stream) != 1) {
        free(header);
        return NULL;
    }

    if (header->type != BMP_MAGIC ||
        header->bpp != BMP_SUPPORTED_BPP ||
        header->compression != 0 ||
        header->width == 0 ||
        header->height == 0) {
        free(header);
        return NULL;
    }

    return header;
}

struct pixel* read_data(FILE* stream, const struct bmp_header* header) {
    if (!stream || !header) return NULL;

    uint32_t width = header->width;
    uint32_t height = header->height;
    uint32_t row_size = calc_row_size(width);
    uint32_t padding = row_size - width * sizeof(struct pixel);

    struct pixel* data = malloc((size_t)width * height * sizeof(struct pixel));
    if (!data) return NULL;

    if (fseek(stream, header->offset, SEEK_SET) != 0) {
        free(data);
        return NULL;
    }

    for (uint32_t y = 0; y < height; y++) {
        if (fread(data + (size_t)y * width, sizeof(struct pixel), width, stream) != width) {
            free(data);
            return NULL;
        }
        if (padding > 0 && fseek(stream, padding, SEEK_CUR) != 0) {
            free(data);
            return NULL;
        }
    }

    return data;
}

struct bmp_image* read_bmp(FILE* stream) {
    if (!stream) return NULL;

    struct bmp_header* header = read_bmp_header(stream);
    if (!header) {
        fprintf(stderr, "Error: Invalid or unsupported BMP file (requires 24-bit uncompressed BMP).\n");
        return NULL;
    }

    struct bmp_image* image = malloc(sizeof(struct bmp_image));
    if (!image) {
        free(header);
        return NULL;
    }

    image->header = header;
    image->data = read_data(stream, header);

    if (!image->data) {
        fprintf(stderr, "Error: Corrupted BMP pixel data.\n");
        free(header);
        free(image);
        return NULL;
    }

    return image;
}

bool write_bmp(FILE* stream, const struct bmp_image* image) {
    if (!stream || !image || !image->header || !image->data) return false;

    uint32_t width = image->header->width;
    uint32_t height = image->header->height;
    uint32_t row_size = calc_row_size(width);
    uint32_t padding = row_size - width * sizeof(struct pixel);

    if (fwrite(image->header, sizeof(struct bmp_header), 1, stream) != 1) {
        return false;
    }

    const uint8_t pad[3] = {0, 0, 0};

    for (uint32_t y = 0; y < height; y++) {
        if (fwrite(image->data + (size_t)y * width, sizeof(struct pixel), width, stream) != width) {
            return false;
        }
        if (padding > 0 && fwrite(pad, 1, padding, stream) != padding) {
            return false;
        }
    }

    return true;
}

void free_bmp_image(struct bmp_image* image) {
    if (!image) return;
    free(image->data);
    free(image->header);
    free(image);
}
