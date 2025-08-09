#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main() {
    // Configuration variables
    int hexColor = 0x00FF00;       // Green color in hex
    float widthScale = 30;         // Horizontal stretch factor
    float heightScale = 15;        // Vertical stretch factor
    int sleepTime = 10000;         // Sleep time in microseconds (10000 = 0.01 seconds)
    int endTime = 10;              // Duration to run the animation (in seconds)

    // Rotation angles
    float A = 0, B = 0;

    // Buffers for depth and ASCII characters
    float z[1760];
    char b[1760];

    // Extract RGB components from hex color
    int r = (hexColor >> 16) & 0xFF;
    int g = (hexColor >> 8) & 0xFF;
    int b_color = hexColor & 0xFF;

    // Start time for animation
    time_t start_time = time(NULL);

    // Clear the screen
    printf("\x1b[2J");

    // Main animation loop
    for (;;) {
        // Initialize buffers
        memset(b, 32, 1760); // Fill character buffer with spaces
        memset(z, 0, 7040);  // Reset depth buffer

        // Generate the donut
        for (float j = 0; j < 2 * M_PI; j += 0.07) { // Outer loop for rotation
            for (float i = 0; i < 2 * M_PI; i += 0.02) { // Inner loop for rotation
                // Calculate depth (D) and 3D coordinates
                float D = 1 / (sin(i) * (cos(j) + 2) * sin(A) + sin(j) * cos(A) + 5);

                // Project 3D coordinates to 2D screen space
                int x = 40 + widthScale * D * (cos(i) * (cos(j) + 2) * cos(B) 
                        - (sin(i) * (cos(j) + 2) * cos(A) - sin(j) * sin(A)) * sin(B));
                int y = 12 + heightScale * D * (cos(i) * (cos(j) + 2) * sin(B) 
                        + (sin(i) * (cos(j) + 2) * cos(A) - sin(j) * sin(A)) * cos(B));

                // Calculate buffer index
                int o = x + 80 * y;

                // Calculate luminance
                int N = 8 * ((sin(j) * sin(A) - sin(i) * cos(j) * cos(A)) * cos(B) 
                        - sin(i) * cos(j) * sin(A) - sin(j) * cos(A) 
                        - cos(i) * cos(j) * sin(B));

                // Update buffers if within bounds and closer to the viewer
                if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                    z[o] = D; // Update depth buffer
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0]; // Update character buffer
                }
            }
        }

        // Set RGB color and move cursor to the top-left
        printf("\x1b[38;2;%d;%d;%dm\x1b[H", r, g, b_color);

        // Render the frame
        for (int k = 0; k < 1761; k++) {
            putchar(k % 80 ? b[k] : 10); // Print character or newline
            A += 0.00004; // Increment rotation angle A
            B += 0.00002; // Increment rotation angle B
        }

        // Pause for a short duration
        usleep(sleepTime);

        // Exit the loop after the specified duration
        if (time(NULL) - start_time > endTime) {
            break;
        }
    }

    return 0;
}