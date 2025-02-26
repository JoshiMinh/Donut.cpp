#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 22
#define SCALE_X 30
#define SCALE_Y 15
#define SLEEP_TIME 8000  
#define END_TIME 10       

int main() {
    float A = 0;
    char b[SCREEN_WIDTH * SCREEN_HEIGHT];
    time_t start_time = time(NULL);
    
    printf("\x1b[2J");

    while (time(NULL) - start_time <= END_TIME) {
        memset(b, ' ', sizeof(b));

        float points[5][3] = {
            { 0, 1, 0 }, { -1, -1, -1 }, { 1, -1, -1 },  
            { 1, -1, 1 }, { -1, -1, 1 }
        };

        int edges[8][2] = {
            {0, 1}, {0, 2}, {0, 3}, {0, 4},
            {1, 2}, {2, 3}, {3, 4}, {4, 1}
        };

        for (int e = 0; e < 8; e++) {
            int p1 = edges[e][0], p2 = edges[e][1];

            float x1 = points[p1][0] * cos(A) - points[p1][2] * sin(A);
            float y1 = points[p1][1];
            float z1 = points[p1][0] * sin(A) + points[p1][2] * cos(A) + 3;

            float x2 = points[p2][0] * cos(A) - points[p2][2] * sin(A);
            float y2 = points[p2][1];
            float z2 = points[p2][0] * sin(A) + points[p2][2] * cos(A) + 3;

            int sx1 = SCREEN_WIDTH / 2 + SCALE_X * x1 / z1;
            int sy1 = SCREEN_HEIGHT / 2 - SCALE_Y * y1 / z1;
            int sx2 = SCREEN_WIDTH / 2 + SCALE_X * x2 / z2;
            int sy2 = SCREEN_HEIGHT / 2 - SCALE_Y * y2 / z2;

            int dx = abs(sx2 - sx1), dy = abs(sy2 - sy1);
            int steps = dx > dy ? dx : dy;

            for (int i = 0; i <= steps; i++) {
                int x = sx1 + i * (sx2 - sx1) / steps;
                int y = sy1 + i * (sy2 - sy1) / steps;
                if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                    b[y * SCREEN_WIDTH + x] = '#';
                }
            }
        }

        printf("\x1b[38;2;0;255;0m\x1b[H");
        for (int k = 0; k < SCREEN_WIDTH * SCREEN_HEIGHT; k++) {
            putchar(k % SCREEN_WIDTH ? b[k] : '\n');
        }

        usleep(SLEEP_TIME);
        A += 0.06;
    }

    return 0;
}