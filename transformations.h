#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "bmp.h"

struct bmp_image* flip_horizontally(const struct bmp_image* image);
struct bmp_image* flip_vertically(const struct bmp_image* image);
struct bmp_image* rotate_right(const struct bmp_image* image);
struct bmp_image* rotate_left(const struct bmp_image* image);
struct bmp_image* scale(const struct bmp_image* image, float factor);
struct bmp_image* crop(const struct bmp_image* image, uint32_t start_y, uint32_t start_x, uint32_t height, uint32_t width);
struct bmp_image* extract(const struct bmp_image* image, const char* colors_to_keep);

#endif
