#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int generate_maze(int size, char passage, char wall)
{
    int width = 2 * size + 1;
    size_t rooms = (size_t)size * size;
    char *grid = malloc((size_t)width * width);
    unsigned char *visited = calloc(rooms, sizeof(*visited));
    int *stack = malloc(rooms * sizeof(*stack));
    size_t depth = 0;
    int result = EXIT_FAILURE;
    const int dr[] = {-1, 0, 1, 0};
    const int dc[] = {0, 1, 0, -1};

    if (!grid || !visited || !stack) {
        perror("malloc");
        goto cleanup;
    }
    memset(grid, wall, (size_t)width * width);
    visited[0] = 1;
    grid[width + 1] = passage;
    stack[depth++] = 0;

    /* Randomized depth-first search: each new room gets exactly one edge. */
    while (depth > 0) {
        int cell = stack[depth - 1];
        int row = cell / size, col = cell % size;
        int directions[4], count = 0;

        for (int d = 0; d < 4; ++d) {
            int nr = row + dr[d], nc = col + dc[d];
            if (nr >= 0 && nr < size && nc >= 0 && nc < size &&
                !visited[nr * size + nc])
                directions[count++] = d;
        }
        if (count == 0) {
            --depth;
            continue;
        }

        int d = directions[rand() % count];
        int nr = row + dr[d], nc = col + dc[d];
        grid[(2 * row + 1 + dr[d]) * width + 2 * col + 1 + dc[d]] = passage;
        grid[(2 * nr + 1) * width + 2 * nc + 1] = passage;
        visited[nr * size + nc] = 1;
        stack[depth++] = nr * size + nc;
    }

    for (int row = 0; row < width; ++row) {
        if (fwrite(grid + (size_t)row * width, 1, width, stdout) != (size_t)width ||
            putchar('\n') == EOF) {
            perror("stdout");
            goto cleanup;
        }
    }
    if (fflush(stdout) == EOF) {
        perror("stdout");
        goto cleanup;
    }
    result = EXIT_SUCCESS;

cleanup:
    free(stack);
    free(visited);
    free(grid);
    return result;
}

int main(int argc, char *argv[])
{
    int size = 6;
    const char passage = '.', wall = '#';

    if (argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        return EXIT_FAILURE;
    }
    srand((unsigned int)time(NULL));
    return generate_maze(size, passage, wall);
}
