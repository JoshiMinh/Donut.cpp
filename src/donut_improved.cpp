#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Screen dimensions
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 22
#define BUFFER_SIZE   (SCREEN_WIDTH * SCREEN_HEIGHT)

// Luminance characters ordered from dark to bright
static const char LUMINANCE_CHARS[] = ".,-~:;=!*#$@";

int main() {
    // --- Configuration ---
    int   hexColor    = 0x00FF00; // Display color as 0xRRGGBB
    float widthScale  = 30;       // Horizontal projection scale
    float heightScale = 15;       // Vertical projection scale
    int   frameDelay  = 10000;    // Frame delay in microseconds
    int   duration    = 10;       // Animation duration in seconds

    // Extract RGB components from the hex color
    int colorR = (hexColor >> 16) & 0xFF;
    int colorG = (hexColor >>  8) & 0xFF;
    int colorB =  hexColor        & 0xFF;

    // Rotation angles
    float rotA = 0, rotB = 0;

    float depth[BUFFER_SIZE];   // Per-pixel inverse depth
    char  screen[BUFFER_SIZE];  // Per-pixel ASCII character

    time_t startTime = time(NULL);
    printf("\x1b[2J"); // Clear screen

    for (;;) {
        memset(screen, ' ', BUFFER_SIZE);
        memset(depth,   0,  sizeof(depth));

        // Precompute rotation sines/cosines
        float sinA = sin(rotA), cosA = cos(rotA);
        float sinB = sin(rotB), cosB = cos(rotB);

        // Iterate over the donut surface:
        //   phi   — angle around the tube cross-section
        //   theta — angle around the torus centre
        for (float phi = 0; phi < 2 * M_PI; phi += 0.07f) {
            float sinPhi = sin(phi), cosPhi = cos(phi);

            for (float theta = 0; theta < 2 * M_PI; theta += 0.02f) {
                float sinTheta = sin(theta), cosTheta = cos(theta);

                float h    = cosPhi + 2; // Distance from torus centre to surface point
                float invD = 1 / (sinTheta * h * sinA + sinPhi * cosA + 5);

                // Project 3-D point onto 2-D screen
                int x = (int)(40 + widthScale  * invD
                              * (cosTheta * h * cosB
                                 - (sinTheta * h * cosA - sinPhi * sinA) * sinB));
                int y = (int)(12 + heightScale * invD
                              * (cosTheta * h * sinB
                                 + (sinTheta * h * cosA - sinPhi * sinA) * cosB));

                int idx = x + SCREEN_WIDTH * y;

                // Luminance from surface-normal dot light-direction
                int lum = (int)(8 * ((sinPhi * sinA - sinTheta * cosPhi * cosA) * cosB
                                - sinTheta * cosPhi * sinA
                                - sinPhi * cosA
                                - cosTheta * cosPhi * sinB));

                if (y >= 0 && y < SCREEN_HEIGHT && x >= 0 && x < SCREEN_WIDTH
                        && invD > depth[idx]) {
                    depth[idx]  = invD;
                    screen[idx] = LUMINANCE_CHARS[lum > 0 ? lum : 0];
                }
            }
        }

        // Render frame with configured color
        printf("\x1b[38;2;%d;%d;%dm\x1b[H", colorR, colorG, colorB);
        for (int k = 0; k < BUFFER_SIZE; k++) {
            putchar(k % SCREEN_WIDTH ? screen[k] : '\n');
            rotA += 0.00004f;
            rotB += 0.00002f;
        }

        usleep(frameDelay);

        if (time(NULL) - startTime > duration)
            break;
    }

    return 0;
}