#ifndef BMP_H
#define BMP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Standard 54-byte Windows BMP file and DIB header.
 */
struct bmp_header {
    uint16_t type;              // Magic identifier: "BM" (0x4D42)
    uint32_t size;              // Total file size in bytes
    uint16_t reserved1;         // Reserved (0)
    uint16_t reserved2;         // Reserved (0)
    uint32_t offset;            // Byte offset to pixel array (54)
    uint32_t dib_size;          // DIB header size (40)
    uint32_t width;             // Image width in pixels
    uint32_t height;            // Image height in pixels
    uint16_t planes;            // Color planes (1)
    uint16_t bpp;               // Bits per pixel (24)
    uint32_t compression;       // Compression method (0 = uncompressed)
    uint32_t image_size;        // Size of raw bitmap data in bytes
    uint32_t x_ppm;             // Horizontal resolution (pixels per meter)
    uint32_t y_ppm;             // Vertical resolution (pixels per meter)
    uint32_t num_colors;        // Number of colors in palette (0)
    uint32_t important_colors;  // Important colors (0)
} __attribute__((__packed__));

/**
 * 24-bit BGR pixel representation.
 */
struct pixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} __attribute__((__packed__));

/**
 * In-memory representation of a 24-bit BMP image.
 */
struct bmp_image {
    struct bmp_header* header;
    struct pixel* data;
};

struct bmp_image* read_bmp(FILE* stream);
bool write_bmp(FILE* stream, const struct bmp_image* image);
struct bmp_header* read_bmp_header(FILE* stream);
struct pixel* read_data(FILE* stream, const struct bmp_header* header);
void free_bmp_image(struct bmp_image* image);

#endif
