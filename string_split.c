#include <stdio.h>
#include <string.h>

int main() {

    char str[80] = "f 1201/1249/1201 1202/1248/1202 1200/1246/1200";
    printf("complete line %s\n", str);

    // we get the 'f' alone here, we skip it
    char *token = strtok(str, " ");

    // while (token != NULL) {
    char *token_1 = strtok(NULL, " ");
    char *token_2 = strtok(NULL, " ");
    char *token_3 = strtok(NULL, " ");

    printf("%s\n", token_1);
    char *vertex_index = strtok(token_1, "/");
    char *vertex_texture = strtok(NULL, "/");
    char *vertex_normal = strtok(NULL, "/");
    printf("vertex_index %s, vertex_texture %s, vertex_normal %s\n", vertex_index, vertex_texture, vertex_normal);

    printf("%s\n", token_2);
    vertex_index = strtok(token_2, "/");
    vertex_texture = strtok(NULL, "/");
    vertex_normal = strtok(NULL, "/");
    printf("vertex_index %s, vertex_texture %s, vertex_normal %s\n", vertex_index, vertex_texture, vertex_normal);

    printf("%s\n", token_3);
    vertex_index = strtok(token_3, "/");
    vertex_texture = strtok(NULL, "/");
    vertex_normal = strtok(NULL, "/");
    printf("vertex_index %s, vertex_texture %s, vertex_normal %s\n", vertex_index, vertex_texture, vertex_normal);
    // }

    return 0;
}