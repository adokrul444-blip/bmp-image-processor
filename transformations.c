#include "transformations.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static uint32_t calc_row_size(uint32_t width) {
    return (width * sizeof(struct pixel) + 3) & ~3U;
}

static void update_header_metrics(struct bmp_header* h, uint32_t new_width, uint32_t new_height) {
    h->width = new_width;
    h->height = new_height;
    h->image_size = calc_row_size(new_width) * new_height;
    h->size = h->offset + h->image_size;
}

static struct bmp_image* create_blank_copy(const struct bmp_image* src, uint32_t new_width, uint32_t new_height) {
    if (!src || !src->header || !src->data || new_width == 0 || new_height == 0) {
        return NULL;
    }

    struct bmp_image* dst = malloc(sizeof(struct bmp_image));
    if (!dst) return NULL;

    dst->header = malloc(sizeof(struct bmp_header));
    if (!dst->header) {
        free(dst);
        return NULL;
    }

    memcpy(dst->header, src->header, sizeof(struct bmp_header));
    update_header_metrics(dst->header, new_width, new_height);

    dst->data = malloc((size_t)new_width * new_height * sizeof(struct pixel));
    if (!dst->data) {
        free(dst->header);
        free(dst);
        return NULL;
    }

    return dst;
}

struct bmp_image* extract(const struct bmp_image* image, const char* colors_to_keep) {
    if (!image || !image->header || !image->data || !colors_to_keep || colors_to_keep[0] == '\0') {
        return NULL;
    }

    bool keep_r = false, keep_g = false, keep_b = false;

    for (size_t i = 0; colors_to_keep[i] != '\0'; i++) {
        switch (colors_to_keep[i]) {
            case 'r': keep_r = true; break;
            case 'g': keep_g = true; break;
            case 'b': keep_b = true; break;
            default: return NULL;
        }
    }

    uint32_t w = image->header->width;
    uint32_t h = image->header->height;
    struct bmp_image* dst = create_blank_copy(image, w, h);
    if (!dst) return NULL;

    size_t total_pixels = (size_t)w * h;
    for (size_t i = 0; i < total_pixels; i++) {
        dst->data[i].red = keep_r ? image->data[i].red : 0;
        dst->data[i].green = keep_g ? image->data[i].green : 0;
        dst->data[i].blue = keep_b ? image->data[i].blue : 0;
    }

    return dst;
}

struct bmp_image* flip_horizontally(const struct bmp_image* image) {
    if (!image || !image->header || !image->data) return NULL;

    uint32_t w = image->header->width;
    uint32_t h = image->header->height;
    struct bmp_image* dst = create_blank_copy(image, w, h);
    if (!dst) return NULL;

    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            dst->data[(size_t)y * w + x] = image->data[(size_t)y * w + (w - 1 - x)];
        }
    }

    return dst;
}

struct bmp_image* flip_vertically(const struct bmp_image* image) {
    if (!image || !image->header || !image->data) return NULL;

    uint32_t w = image->header->width;
    uint32_t h = image->header->height;
    struct bmp_image* dst = create_blank_copy(image, w, h);
    if (!dst) return NULL;

    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            dst->data[(size_t)y * w + x] = image->data[(size_t)(h - 1 - y) * w + x];
        }
    }

    return dst;
}

struct bmp_image* rotate_right(const struct bmp_image* image) {
    if (!image || !image->header || !image->data) return NULL;

    uint32_t w = image->header->width;
    uint32_t h = image->header->height;
    struct bmp_image* dst = create_blank_copy(image, h, w);
    if (!dst) return NULL;

    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            dst->data[(size_t)(w - 1 - x) * h + y] = image->data[(size_t)y * w + x];
        }
    }

    return dst;
}

struct bmp_image* rotate_left(const struct bmp_image* image) {
    if (!image || !image->header || !image->data) return NULL;

    uint32_t w = image->header->width;
    uint32_t h = image->header->height;
    struct bmp_image* dst = create_blank_copy(image, h, w);
    if (!dst) return NULL;

    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            dst->data[(size_t)x * h + (h - 1 - y)] = image->data[(size_t)y * w + x];
        }
    }

    return dst;
}

struct bmp_image* crop(
    const struct bmp_image* image,
    uint32_t start_y,
    uint32_t start_x,
    uint32_t height,
    uint32_t width
) {
    if (!image || !image->header || !image->data || height == 0 || width == 0) {
        return NULL;
    }

    uint32_t orig_w = image->header->width;
    uint32_t orig_h = image->header->height;

    if (start_y >= orig_h || start_x >= orig_w ||
        start_y + height > orig_h || start_x + width > orig_w) {
        return NULL;
    }

    struct bmp_image* dst = create_blank_copy(image, width, height);
    if (!dst) return NULL;

    for (uint32_t y = 0; y < height; y++) {
        uint32_t src_y = orig_h - start_y - height + y;
        for (uint32_t x = 0; x < width; x++) {
            dst->data[(size_t)y * width + x] = image->data[(size_t)src_y * orig_w + (start_x + x)];
        }
    }

    return dst;
}

struct bmp_image* scale(const struct bmp_image* image, float factor) {
    if (!image || !image->header || !image->data || factor <= 0.0f) {
        return NULL;
    }

    uint32_t orig_w = image->header->width;
    uint32_t orig_h = image->header->height;
    uint32_t new_w = (uint32_t)lroundf((float)orig_w * factor);
    uint32_t new_h = (uint32_t)lroundf((float)orig_h * factor);

    struct bmp_image* dst = create_blank_copy(image, new_w, new_h);
    if (!dst) return NULL;

    for (uint32_t y = 0; y < new_h; y++) {
        uint32_t src_y = (y * orig_h) / new_h;
        for (uint32_t x = 0; x < new_w; x++) {
            uint32_t src_x = (x * orig_w) / new_w;
            dst->data[(size_t)y * new_w + x] = image->data[(size_t)src_y * orig_w + src_x];
        }
    }

    return dst;
}
