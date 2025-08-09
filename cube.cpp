#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 22
#define BUFFER_SIZE (WIDTH * HEIGHT)
#define SLEEP_TIME 10000

int main() {
    // RGB color components
    int hexColor = 0x00FF00, r = (hexColor >> 16) & 0xFF, g = (hexColor >> 8) & 0xFF, b_color = hexColor & 0xFF;
    // Scaling factors and rotation angles
    float widthScale = 30, heightScale = 15, A = 0, B = 0, z[BUFFER_SIZE];
    char b[BUFFER_SIZE];
    clock_t start_time = clock();
    printf("\x1b[2J"); // Clear screen

    while ((clock() - start_time) * 1000 / CLOCKS_PER_SEC < 10000) {
        memset(b, 32, BUFFER_SIZE); // Fill buffer with spaces
        memset(z, 0, sizeof(z));    // Reset depth buffer

        for (int face = 0; face < 6; face++) {
            for (float u = -1; u <= 1; u += 0.05) {
                for (float v = -1; v <= 1; v += 0.05) {
                    // Cube face vertices and normals
                    float x, y, zc, nx, ny, nz;
                    switch (face) {
                        case 0: x = u; y = v; zc = 1;  nx = 0; ny = 0; nz = 1;  break;
                        case 1: x = u; y = v; zc = -1; nx = 0; ny = 0; nz = -1; break;
                        case 2: x = 1; y = u; zc = v;  nx = 1; ny = 0; nz = 0;  break;
                        case 3: x = -1; y = u; zc = v; nx = -1; ny = 0; nz = 0; break;
                        case 4: x = u; y = 1; zc = v;  nx = 0; ny = 1; nz = 0;  break;
                        case 5: x = u; y = -1; zc = v; nx = 0; ny = -1; nz = 0; break;
                    }

                    // Rotate around X and Z axes
                    float y1 = y * cos(A) - zc * sin(A), z1 = y * sin(A) + zc * cos(A);
                    float ny1 = ny * cos(A) - nz * sin(A), nz1 = ny * sin(A) + nz * cos(A);
                    float x1 = x, nx1 = nx, X = x1 * cos(B) - y1 * sin(B), Y = x1 * sin(B) + y1 * cos(B);
                    float Z = z1, nX = nx1 * cos(B) - ny1 * sin(B), nY = nx1 * sin(B) + ny1 * cos(B), nZ = nz1;

                    // Perspective projection
                    float ooz = 1 / (Z + 3);
                    int xp = (int)(40 + widthScale * ooz * X), yp = (int)(12 + heightScale * ooz * Y), idx = xp + WIDTH * yp;

                    // Update buffers if within bounds and closer
                    if (yp >= 0 && yp < HEIGHT && xp >= 0 && xp < WIDTH && ooz > z[idx]) {
                        // Lighting calculations
                        float Lx = 0, Ly = 1, Lz = -1, Lmag = sqrt(Lx * Lx + Ly * Ly + Lz * Lz);
                        Lx /= Lmag; Ly /= Lmag; Lz /= Lmag;
                        float luminance = fmax(0, nX * Lx + nY * Ly + nZ * Lz);
                        char luminance_chars[] = ".,-~:;=!*#$@";
                        z[idx] = ooz;
                        b[idx] = luminance_chars[(int)(luminance * 11)];
                    }
                }
            }
        }

        // Render frame
        printf("\x1b[38;2;%d;%d;%dm\x1b[H", r, g, b_color); // Set RGB color
        for (int k = 0; k < BUFFER_SIZE; k++) putchar(k % WIDTH ? b[k] : 10);
        usleep(SLEEP_TIME); // Delay
        A += 0.03; // Increment rotation angles
        B += 0.02;
    }
    return 0;
}