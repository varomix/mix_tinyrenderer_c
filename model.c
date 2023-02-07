//
// Created by Varo Castaneda on 1/7/23.
//

#include "model.h"
#include "cglm/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_obj_file(const char *filename, Model *model) {
    model->num_verts = 0;
    model->num_faces = 0;

    model->verts = malloc(2048 * sizeof(vec3));
    model->faces = malloc(4096 * sizeof(vec3));
    model->tex_verts = malloc(4096 * sizeof(vec3));
    model->nor_verts = malloc(4096 * sizeof(vec3));

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("File couldn't be opened\n");
        exit(666);
    }

    char line[1024];
    int vi = 0;
    int fi = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == 'v' && line[1] == ' ') {
            // vertex line
            // printf("%s", line);
            char *token = strtok(line, " "); // skip the first token which would be v, f, vt
            double x = atof(strtok(NULL, " "));
            double y = atof(strtok(NULL, " "));
            double z = atof(strtok(NULL, " "));
            model->verts[vi][0] = x;
            model->verts[vi][1] = y;
            model->verts[vi][2] = z;
            ++model->num_verts;
            vi++;
        } else if (line[0] == 'f' && line[1] == ' ') {
            // face line
//            printf("line: %s", line);
            char *token = strtok(line, " "); // skip the first token which would be v, f, vt
            char *token_1 = strtok(NULL, " ");
            char *token_2 = strtok(NULL, " ");
            char *token_3 = strtok(NULL, " ");

            // parse first vertex
            char *vertex_index = strtok(token_1, "/");
            char *vertex_texture = strtok(NULL, "/");
            char *vertex_normal = strtok(NULL, "/");

            model->faces[fi][0] = atoi(vertex_index) - 1;
            model->tex_verts[fi][0] = atoi(vertex_texture) - 1;
            model->nor_verts[fi][0] = atoi(vertex_normal) - 1;

            // parse second vertex
            vertex_index = strtok(token_2, "/");
            vertex_texture = strtok(NULL, "/");
            vertex_normal = strtok(NULL, "/");

            model->faces[fi][1] = atoi(vertex_index) - 1;
            model->tex_verts[fi][1] = atoi(vertex_texture) - 1;
            model->nor_verts[fi][1] = atoi(vertex_normal) - 1;

            // parse third vertex
            vertex_index = strtok(token_3, "/");
            vertex_texture = strtok(NULL, "/");
            vertex_normal = strtok(NULL, "/");

            model->faces[fi][2] = atoi(vertex_index) - 1;
            model->tex_verts[fi][2] = atoi(vertex_texture) - 1;
            model->nor_verts[fi][2] = atoi(vertex_normal) - 1;

            fi++;
            ++model->num_faces;
        }
    }

    fclose(file);
}
