# BMP Image Processor in C

Command-line utility written in C for low-level manipulation of uncompressed 24-bit BMP images. Directly parses binary file headers and modifies pixel arrays in memory without external image libraries.

## Features
- **Rotation:** 90° clockwise (`-r`) and counter-clockwise (`-l`)
- **Flipping:** Horizontal (`-fh`) and vertical (`-fv`) mirroring
- **Scaling:** Proportional resizing by a floating-point factor (`-s <factor>`)
- **Cropping:** Rectangular region extraction (`-c <y> <x> <h> <w>`)
- **Color Extraction:** Isolating specific RGB channels (`-e <r|g|b>`)

## Build & Run
```bash
make
./bmp -i input.bmp -o output.bmp -r -e rg
```
