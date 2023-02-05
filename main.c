#include "cglm/ivec3.h"
#include "cglm/types.h"
#include "cglm/vec3.h"
#include "model.h"
#include <cglm/cglm.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image_write.h"

const int width = 512;
const int height = 512;
const int num_channels = 3;
const int depth = 255;

int *zbuffer = NULL;
//int zbuffer[2048576];
vec3 light_dir = {0, 0, -1};

typedef struct {
    int width;
    int height;
    int num_channels;
    uint8_t *pixels;
} Image;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} ColorRGBA;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ColorRGB;

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

void swap_ivec3(ivec3 v1, ivec3 v2) {
    ivec3 temp;
    glm_ivec3_copy(v1, temp);
    glm_ivec3_copy(v2, v1);
    glm_ivec3_copy(temp, v2);
}

typedef struct {
    vec3 verts;
    vec3 faces;
    int num_faces;
} Mesh;

/* Set the pixel at the specified (x,y) position to the specified color */
void set_pixel(Image img, int x, int y, ColorRGB color) {
    int index = y * img.width * img.num_channels + x * img.num_channels;
    img.pixels[index + 0] = color.r;
    img.pixels[index + 1] = color.g;
    img.pixels[index + 2] = color.b;
}

void line(int x0, int y0, int x1, int y1, Image image, ColorRGB color) {
    bool steep = false;
    if (abs(x0 - x1) < abs(y0 - y1)) {
        swap(&x0, &y0);
        swap(&x1, &y1);
        steep = true;
    }
    if (x0 > x1) { // make it left to right
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep)
            set_pixel(image, y, x, color);
        else
            set_pixel(image, x, y, color);
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

void triangle(ivec3 t0, ivec3 t1, ivec3 t2, Image image, ColorRGB color, int *zbuf) {
    if (t0[1] == t1[1] && t0[1] == t2[1]) // don't care about degenerated triangles
        return;

    if (t0[1] > t1[1]) swap_ivec3(t0, t1);
    if (t0[1] > t2[1]) swap_ivec3(t0, t2);
    if (t1[1] > t2[1]) swap_ivec3(t1, t2);

    int total_height = t2[1] - t0[1];
    for (int i = 0; i < total_height; i++) {
        bool second_half = i > t1[1] - t0[1] || t1[1] == t0[1];
        int segment_height = second_half ? t2[1] - t1[1] : t1[1] - t0[1];
        float alpha = (float) i / (float) total_height;
        float beta = (float) (i - (second_half ? t1[1] - t0[1] : 0)) / (float) segment_height;

        ivec3 A1, A2, A = GLM_IVEC3_ZERO_INIT;
        glm_ivec3_sub(t2, t0, A1);
        glm_ivec3_scale(A1, alpha, A2);
        glm_ivec3_add(A2, t0, A);

        ivec3 B, B1, B2, BOP1, B11, B22, BOP2 = GLM_IVEC3_ZERO_INIT;
        // Vec3i B = second_half ? t1 + (t2-t1)*beta
        glm_ivec3_sub(t2, t1, B1);
        glm_ivec3_scale(B1, beta, B2);
        glm_ivec3_add(t1, B2, BOP1);

        //: t0 + (t1-t0)*beta;
        glm_ivec3_sub(t1, t0, B11);
        glm_ivec3_scale(B11, beta, B22);
        glm_ivec3_add(t0, B22, BOP2);

        if (second_half) {
            glm_ivec3_copy(BOP1, B);
        } else {
            glm_ivec3_copy(BOP2, B);
        }

        if (A[0] > B[0]) swap_ivec3(A, B);
        for (int j = A[0]; j <= B[0]; j++) {
            float phi = B[0] == A[0] ? 1.0 : (float) (j - A[0]) / (float) (B[0] - A[0]);

            // C ++ version Vec3i P = A + (B-A)*phi;
            ivec3 P, P1, P2 = GLM_IVEC3_ZERO_INIT;
            glm_ivec3_sub(B, A, P1);
            glm_ivec3_scale(P1, phi, P2);
            glm_ivec3_add(P2, A, P);
            P[0] = j;
            P[1] = t0[1] + i;
            int idx = j + (t0[1] + i) * width;
            if (zbuf[idx] < P[2]) {
                zbuf[idx] = P[2];
                set_pixel(image, P[0], P[1], color);
            }
        }
    }
}

Image create_image(int width, int height, int num_channels) {
    uint8_t *pixels = (uint8_t *) malloc(width * height * num_channels * sizeof(uint8_t));
    for (int i = 0; i < width * height * num_channels; ++i) {
        pixels[i] = 0;
    }

    Image image = {.width = width, .height = height, .num_channels = num_channels, .pixels = pixels};

    return image;
}

int main(void) {

    ColorRGB red = {.r = 255, .g = 0, .b = 0};
    ColorRGB white = {.r = 255, .g = 255, .b = 255};

    // create image
    Image image = create_image(width, height, num_channels);

    // line(13, 20, 80, 40, image, white);
    // line(20, 13, 40, 80, image, red);
    // line(80, 40, 13, 20, image, red);

    /* Set the pixel at the center of the image to red */
    //    set_pixel(image, 98, 98, red);
    const char *mesh_path = "../obj/african_head.obj";
//    const char *mesh_path = "../obj/cube.obj";
//    const char *mesh_path = "../obj/susanne.obj";

    Model model;
    parse_obj_file(mesh_path, &model);
    printf("Number of vertices: %d\n", model.num_verts);
    printf("Number of faces: %d\n", model.num_faces);

//    zbuffer = (size_t *) malloc(width * height * sizeof(size_t));
    zbuffer = (int *) malloc(width * height * sizeof(int));
    for (int i = 0; i < width * height; i++) {
        zbuffer[i] = INT_MIN;
//        printf("zbuffer %d\n", zbuffer[i]);
    }

    // for (int i = 0; i < model.num_faces; i++) {
    //     vec3 face;
    //     glm_vec3_copy(model.faces[i], face);
    //     for (int j = 0; j < 3; j++) {
    //         vec3 v0, v1;
    //         glm_vec3_copy(model.verts[(int)face[j]], v0);
    //         glm_vec3_copy(model.verts[(int)face[(j + 1) % 3]], v1);
    //         int x0 = (v0[0] + 1.0) * (width - 1) / 2.0;
    //         int y0 = (v0[1] + 1.0) * (height - 1) / 2.0;
    //         int x1 = (v1[0] + 1.0) * (width - 1) / 2.0;
    //         int y1 = (v1[1] + 1.0) * (height - 1) / 2.0;

    //         line(x0, y0, x1, y1, image, white);
    //     }
    // }

    for (int i = 0; i < model.num_faces - 1; i++) {
        vec3 face = GLM_VEC3_ZERO_INIT;
        glm_vec3_copy(model.faces[i], face);
        ivec3 screen_coords[3] = GLM_IVEC3_ZERO_INIT;
        vec3 world_coords[3] = GLM_VEC3_ZERO_INIT;
        for (int j = 0; j < 3; j++) {
            vec3 v = GLM_VEC3_ZERO_INIT;
            glm_vec3_copy(model.verts[(int) face[j]], v);
            ivec3 temp_SC = {(v[0] + 1.0) * width / 2.0, (v[1] + 1.0) * height / 2.0, (v[2] + 1.0) * depth / 2.0};
            glm_ivec3_copy(temp_SC, screen_coords[j]);
            glm_vec3_copy(v, world_coords[j]);
        }

        // vec3 n = {(world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0])};
        vec3 n, n1, n2 = GLM_VEC3_ZERO_INIT;
        vec3 w0, w1, w2 = GLM_VEC3_ZERO_INIT;
        glm_vec3_copy(world_coords[0], w0);
        glm_vec3_copy(world_coords[1], w1);
        glm_vec3_copy(world_coords[2], w2);
        glm_vec3_sub(w2, w0, n1);
        glm_vec3_sub(w1, w0, n2);
        glm_cross(n1, n2, n);
        glm_vec3_normalize(n);
        float intensity = glm_vec3_dot(n, light_dir);

        if (intensity > 0) {
            ColorRGB intensity_color = {intensity * 255, intensity * 255, intensity * 255};
            triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, intensity_color, zbuffer);
        }
    }

    stbi__flip_vertically_on_write = 1;
    stbi_write_png("output_chapter_03.png", width, height, num_channels, image.pixels, width * num_channels);

    // dump z-buffer (debugging purposes only)

    Image zbimage = create_image(width, height, num_channels);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            ColorRGB z_color = {zbuffer[i + j * width], zbuffer[i + j * width], zbuffer[i + j * width]};
            set_pixel(zbimage, i, j, z_color);
        }
    }

    stbi__flip_vertically_on_write = 1;
    stbi_write_png("zdepth_chapter_03.png", width, height, num_channels, zbimage.pixels, width * num_channels);

    // Free the memory
    free(zbimage.pixels);
    free(image.pixels);
    free(model.verts);
    free(model.faces);
    return 0;
}
