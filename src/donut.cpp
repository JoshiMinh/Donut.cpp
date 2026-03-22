#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

// Screen dimensions (80x22 terminal characters)
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 22
#define BUFFER_SIZE   (SCREEN_WIDTH * SCREEN_HEIGHT)

// Frame delay in microseconds
#define FRAME_DELAY 30000

// Luminance characters ordered from dark to bright
static const char LUMINANCE_CHARS[] = ".,-~:;=!*#$@";

int main() {
    float rotA = 0, rotB = 0;   // Rotation angles around the two axes
    float depth[BUFFER_SIZE];    // Per-pixel depth (inverse distance from viewer)
    char  screen[BUFFER_SIZE];   // Per-pixel ASCII character

    printf("\x1b[2J"); // Clear screen once before the loop

    for (;;) {
        memset(screen, ' ', BUFFER_SIZE);
        memset(depth, 0,   sizeof(depth));

        // Precompute sines and cosines of the rotation angles
        float sinA = sin(rotA), cosA = cos(rotA);
        float sinB = sin(rotB), cosB = cos(rotB);

        // Iterate over the donut surface using two angular parameters:
        //   phi  (j) — angle around the tube cross-section
        //   theta (i) — angle around the torus centre
        for (float phi = 0; phi < 2 * M_PI; phi += 0.07f) {
            float sinPhi = sin(phi), cosPhi = cos(phi);

            for (float theta = 0; theta < 2 * M_PI; theta += 0.02f) {
                float sinTheta = sin(theta), cosTheta = cos(theta);

                // h = (R2 + R1*cos(theta)) — distance from torus centre to point
                float h = cosPhi + 2;

                // Inverse perspective depth
                float invD = 1 / (sinTheta * h * sinA + sinPhi * cosA + 5);

                // Project 3-D point onto the 2-D screen
                int x = (int)(40 + 30 * invD * (cosTheta * h * cosB
                              - (sinTheta * h * cosA - sinPhi * sinA) * sinB));
                int y = (int)(12 + 15 * invD * (cosTheta * h * sinB
                              + (sinTheta * h * cosA - sinPhi * sinA) * cosB));

                int idx = x + SCREEN_WIDTH * y;

                // Luminance index (dot product of surface normal with light direction)
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

        // Render the completed frame
        printf("\x1b[H"); // Move cursor to top-left
        for (int k = 0; k < BUFFER_SIZE; k++) {
            putchar(k % SCREEN_WIDTH ? screen[k] : '\n');
            // Advance rotation slightly each pixel for a smooth wobble
            rotA += 0.00004f;
            rotB += 0.00002f;
        }

        usleep(FRAME_DELAY);
    }

    return 0;
}