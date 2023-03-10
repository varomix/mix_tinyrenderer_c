#ifndef MIX_TINYRENDERER_MODEL_H
#define MIX_TINYRENDERER_MODEL_H

#include "cglm/cglm.h"
#include "cglm/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Model {
    vec3 *verts;
    vec3 *faces;
    vec3 *tex_verts;
    vec3 *nor_verts;
    int num_verts;
    int num_faces;
} Model;

void parse_obj_file(const char *filename, Model *model);

#endif