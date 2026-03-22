#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Screen dimensions
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 22
#define BUFFER_SIZE   (SCREEN_WIDTH * SCREEN_HEIGHT)

// Projection scale factors
#define SCALE_X 30
#define SCALE_Y 15

// Frame delay in microseconds and animation duration in seconds
#define FRAME_DELAY 8000
#define DURATION    10

int main() {
    float rotA = 0;                // Rotation angle around Y-axis
    char  screen[BUFFER_SIZE];     // Screen character buffer
    time_t startTime = time(NULL);

    printf("\x1b[2J"); // Clear screen

    while (time(NULL) - startTime <= DURATION) {
        memset(screen, ' ', BUFFER_SIZE);

        // Pyramid vertices: apex at top, four base corners below
        float vertices[5][3] = {
            {  0,  1,  0 }, // Apex
            { -1, -1, -1 }, // Base front-left
            {  1, -1, -1 }, // Base front-right
            {  1, -1,  1 }, // Base back-right
            { -1, -1,  1 }, // Base back-left
        };

        // Edges: four from apex to base corners, four forming the base square
        int edges[8][2] = {
            {0, 1}, {0, 2}, {0, 3}, {0, 4}, // Lateral edges
            {1, 2}, {2, 3}, {3, 4}, {4, 1}  // Base edges
        };

        float cosA = cos(rotA), sinA = sin(rotA);

        for (int e = 0; e < 8; e++) {
            int p1 = edges[e][0], p2 = edges[e][1];

            // Rotate both endpoints around the Y-axis then translate into view
            float x1 = vertices[p1][0] * cosA - vertices[p1][2] * sinA;
            float y1 = vertices[p1][1];
            float z1 = vertices[p1][0] * sinA + vertices[p1][2] * cosA + 3;

            float x2 = vertices[p2][0] * cosA - vertices[p2][2] * sinA;
            float y2 = vertices[p2][1];
            float z2 = vertices[p2][0] * sinA + vertices[p2][2] * cosA + 3;

            // Perspective projection to screen coordinates
            int sx1 = SCREEN_WIDTH  / 2 + (int)(SCALE_X * x1 / z1);
            int sy1 = SCREEN_HEIGHT / 2 - (int)(SCALE_Y * y1 / z1);
            int sx2 = SCREEN_WIDTH  / 2 + (int)(SCALE_X * x2 / z2);
            int sy2 = SCREEN_HEIGHT / 2 - (int)(SCALE_Y * y2 / z2);

            // Rasterise the edge with Bresenham's line algorithm
            int dx    = abs(sx2 - sx1);
            int dy    = abs(sy2 - sy1);
            int steps = dx > dy ? dx : dy;

            for (int i = 0; i <= steps; i++) {
                int x = sx1 + i * (sx2 - sx1) / steps;
                int y = sy1 + i * (sy2 - sy1) / steps;
                if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
                    screen[y * SCREEN_WIDTH + x] = '#';
            }
        }

        // Render frame in green
        printf("\x1b[38;2;0;255;0m\x1b[H");
        for (int k = 0; k < BUFFER_SIZE; k++)
            putchar(k % SCREEN_WIDTH ? screen[k] : '\n');

        usleep(FRAME_DELAY);

        rotA += 0.06f;
    }

    return 0;
}