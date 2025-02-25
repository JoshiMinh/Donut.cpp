#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main() {
    int hexColor = 0x00FF00;
    float widthScale = 30;   // Stretch horizontally
    float heightScale = 15;  // Stretch vertically
    int sleepTime = 10000;   // Microseconds 10000 = 0.01 seconds
    int endTime = 10;        // Seconds

    float A = 0, B = 0, i, j;
    int k;
    float pi = 3.14;
    float z[1760];
    char b[1760];
    int r = (hexColor >> 16) & 0xFF, g = (hexColor >> 8) & 0xFF, b_color = hexColor & 0xFF;

    time_t start_time = time(NULL);
    printf("\x1b[2J");

    for (;;) {
        memset(b, 32, 1760);
        memset(z, 0, 7040);

        for (j = 0; j < pi * 2; j += 0.07) {
            for (i = 0; i < pi * 2; i += 0.02) {
                float D = 1 / (sin(i) * (cos(j) + 2) * sin(A) + sin(j) * cos(A) + 5);

                // Donut size
                int x = 40 + widthScale * D * (cos(i) * (cos(j) + 2) * cos(B) 
                        - (sin(i) * (cos(j) + 2) * cos(A) - sin(j) * sin(A)) * sin(B));
                int y = 12 + heightScale * D * (cos(i) * (cos(j) + 2) * sin(B) 
                        + (sin(i) * (cos(j) + 2) * cos(A) - sin(j) * sin(A)) * cos(B));

                int o = x + 80 * y;
                int N = 8 * ((sin(j) * sin(A) - sin(i) * cos(j) * cos(A)) * cos(B) 
                        - sin(i) * cos(j) * sin(A) - sin(j) * cos(A) 
                        - cos(i) * cos(j) * sin(B));

                if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                    z[o] = D;
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
            }
        }

        printf("\x1b[38;2;%d;%d;%dm\x1b[H", r, g, b_color);
        for (k = 0; k < 1761; k++) {
            putchar(k % 80 ? b[k] : 10);
            A += 0.00004;
            B += 0.00002;
        }
        usleep(sleepTime);

        if (time(NULL) - start_time > endTime) {
            break; 
        }
    }

    return 0;
}