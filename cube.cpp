#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 22
#define BUFFER_SIZE (WIDTH * HEIGHT)
#define SLEEP_TIME 10000   // Microseconds per frame (0.01 seconds)
#define RUN_TIME 10        // Total run time in seconds

int main() {
    int hexColor = 0x00FF00;
    float widthScale = 30, heightScale = 15;
    float A = 0, B = 0;
    float z[BUFFER_SIZE];
    char b[BUFFER_SIZE];
    int r = (hexColor >> 16) & 0xFF,
        g = (hexColor >> 8)  & 0xFF,
        b_color = hexColor   & 0xFF;

    time_t start_time = time(NULL);
    printf("\x1b[2J"); // Clear screen

    while (time(NULL) - start_time < RUN_TIME) {
        memset(b, 32, BUFFER_SIZE);
        memset(z, 0, sizeof(z));

        float step = 0.05;
        for (int face = 0; face < 6; face++) {
            for (float u = -1; u <= 1; u += step) {
                for (float v = -1; v <= 1; v += step) {
                    float x, y, zc, nx, ny, nz;

                    switch (face) {
                        case 0: x = u; y = v; zc = 1; nx = 0; ny = 0; nz = 1; break;
                        case 1: x = u; y = v; zc = -1; nx = 0; ny = 0; nz = -1; break;
                        case 2: x = 1; y = u; zc = v; nx = 1; ny = 0; nz = 0; break;
                        case 3: x = -1; y = u; zc = v; nx = -1; ny = 0; nz = 0; break;
                        case 4: x = u; y = 1; zc = v; nx = 0; ny = 1; nz = 0; break;
                        case 5: x = u; y = -1; zc = v; nx = 0; ny = -1; nz = 0; break;
                    }

                    float y1 = y * cos(A) - zc * sin(A);
                    float z1 = y * sin(A) + zc * cos(A);
                    float ny1 = ny * cos(A) - nz * sin(A);
                    float nz1 = ny * sin(A) + nz * cos(A);
                    float x1 = x, nx1 = nx;

                    float X = x1 * cos(B) - y1 * sin(B);
                    float Y = x1 * sin(B) + y1 * cos(B);
                    float Z = z1;
                    float nX = nx1 * cos(B) - ny1 * sin(B);
                    float nY = nx1 * sin(B) + ny1 * cos(B);
                    float nZ = nz1;

                    float distance = 3;
                    float ooz = 1 / (Z + distance);
                    int xp = (int)(40 + widthScale * ooz * X);
                    int yp = (int)(12 + heightScale * ooz * Y);
                    int idx = xp + WIDTH * yp;

                    if (yp >= 0 && yp < HEIGHT && xp >= 0 && xp < WIDTH && ooz > z[idx]) {
                        float Lx = 0, Ly = 1, Lz = -1;
                        float Lmag = sqrt(Lx * Lx + Ly * Ly + Lz * Lz);
                        Lx /= Lmag; Ly /= Lmag; Lz /= Lmag;
                        float luminance = nX * Lx + nY * Ly + nZ * Lz;
                        if (luminance < 0) luminance = 0;

                        int luminance_index = (int)(luminance * 11);
                        if (luminance_index < 0) luminance_index = 0;
                        if (luminance_index > 11) luminance_index = 11;
                        char luminance_chars[] = ".,-~:;=!*#$@";

                        z[idx] = ooz;
                        b[idx] = luminance_chars[luminance_index];
                    }
                }
            }
        }

        printf("\x1b[38;2;%d;%d;%dm\x1b[H", r, g, b_color);
        for (int k = 0; k < BUFFER_SIZE; k++) {
            putchar(k % WIDTH ? b[k] : 10);
        }

        usleep(SLEEP_TIME);
        A += 0.03;
        B += 0.02;
    }

    return 0;
}