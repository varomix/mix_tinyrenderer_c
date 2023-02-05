#include <stdio.h>
#include <stdint.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

/* Set the pixel at the specified (x,y) position to the specified color */
void set_pixel(uint8_t *pixels, int width, int height, int num_channels, int x,
               int y, uint8_t r, uint8_t g, uint8_t b) {
    int index = y * width * num_channels + x * num_channels;
    pixels[index + 0] = r;
    pixels[index + 1] = g;
    pixels[index + 2] = b;
}

int main(void) {
    const int width = 100;
    const int height = 100;
    const int num_channels = 3;

    /* Allocate memory for image pixels */
    uint8_t *pixels =
        (uint8_t *)malloc(width * height * num_channels * sizeof(uint8_t));

    /* Initialize all pixels to black */
    for (int i = 0; i < width * height * num_channels; ++i) {
        pixels[i] = 0;
    }

    /* Set the pixel at the center of the image to red */
    set_pixel(pixels, width, height, num_channels, width / 2, height / 2, 255,
              0, 0);

    stbi_write_png("out.png", width, height, num_channels, pixels,
                   width * num_channels);

    free(pixels);

    return 0;
}