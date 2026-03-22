#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Screen dimensions
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 22
#define BUFFER_SIZE   (SCREEN_WIDTH * SCREEN_HEIGHT)

// Frame delay in microseconds and animation duration in seconds
#define FRAME_DELAY 10000
#define DURATION    10

// Luminance characters ordered from dark to bright
static const char LUMINANCE_CHARS[] = ".,-~:;=!*#$@";

int main() {
    // --- Configuration ---
    int   hexColor    = 0x00FF00; // Display color as 0xRRGGBB
    float widthScale  = 30;       // Horizontal projection scale
    float heightScale = 15;       // Vertical projection scale

    // Extract RGB components from the hex color
    int colorR = (hexColor >> 16) & 0xFF;
    int colorG = (hexColor >>  8) & 0xFF;
    int colorB =  hexColor        & 0xFF;

    // Rotation angles
    float rotA = 0, rotB = 0;

    float depth[BUFFER_SIZE];  // Per-pixel inverse depth
    char  screen[BUFFER_SIZE]; // Per-pixel ASCII character

    clock_t startTime = clock();
    printf("\x1b[2J"); // Clear screen

    while ((clock() - startTime) * 1000 / CLOCKS_PER_SEC < DURATION * 1000) {
        memset(screen, ' ', BUFFER_SIZE);
        memset(depth,   0,  sizeof(depth));

        // Render each face of the cube (6 faces total)
        for (int face = 0; face < 6; face++) {
            for (float u = -1; u <= 1; u += 0.05f) {
                for (float v = -1; v <= 1; v += 0.05f) {
                    // Surface point and outward normal for this face
                    float px, py, pz, nx, ny, nz;
                    switch (face) {
                        case 0: px = u;  py = v;  pz =  1; nx =  0; ny =  0; nz =  1; break;
                        case 1: px = u;  py = v;  pz = -1; nx =  0; ny =  0; nz = -1; break;
                        case 2: px =  1; py = u;  pz = v;  nx =  1; ny =  0; nz =  0; break;
                        case 3: px = -1; py = u;  pz = v;  nx = -1; ny =  0; nz =  0; break;
                        case 4: px = u;  py =  1; pz = v;  nx =  0; ny =  1; nz =  0; break;
                        default:px = u;  py = -1; pz = v;  nx =  0; ny = -1; nz =  0; break;
                    }

                    // Rotate around A (X-axis) then B (Z-axis)
                    float py1 = py * cos(rotA) - pz * sin(rotA);
                    float pz1 = py * sin(rotA) + pz * cos(rotA);
                    float ny1 = ny * cos(rotA) - nz * sin(rotA);
                    float nz1 = ny * sin(rotA) + nz * cos(rotA);

                    float pX = px * cos(rotB) - py1 * sin(rotB);
                    float pY = px * sin(rotB) + py1 * cos(rotB);
                    float pZ = pz1;
                    float nX = nx * cos(rotB) - ny1 * sin(rotB);
                    float nY = nx * sin(rotB) + ny1 * cos(rotB);
                    float nZ = nz1;

                    // Perspective projection
                    float invZ = 1 / (pZ + 3);
                    int   xp   = (int)(40 + widthScale  * invZ * pX);
                    int   yp   = (int)(12 + heightScale * invZ * pY);
                    int   idx  = xp + SCREEN_WIDTH * yp;

                    if (yp >= 0 && yp < SCREEN_HEIGHT && xp >= 0 && xp < SCREEN_WIDTH
                            && invZ > depth[idx]) {
                        // Diffuse lighting: normalised light direction (0, 1, -1)
                        static const float lightMag = 1.41421356f; // sqrt(0^2 + 1^2 + 1^2)
                        float luminance = fmax(0,
                            (nX * 0 + nY * 1 + nZ * -1) / lightMag);

                        depth[idx]  = invZ;
                        screen[idx] = LUMINANCE_CHARS[(int)(luminance * 11)];
                    }
                }
            }
        }

        // Render frame with configured color
        printf("\x1b[38;2;%d;%d;%dm\x1b[H", colorR, colorG, colorB);
        for (int k = 0; k < BUFFER_SIZE; k++)
            putchar(k % SCREEN_WIDTH ? screen[k] : '\n');

        usleep(FRAME_DELAY);

        rotA += 0.03f;
        rotB += 0.02f;
    }

    return 0;
}