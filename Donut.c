#include <stdio.h>
#include <string.h>
#include <math.h>

int main() {
    // --- Setup Variables ---
    float A = 0, B = 0;    // A and B are the rotation angles around the x and z axes.
    float i, j;            // i and j are the angles used to trace the surface of the torus.
    int k;
    float z[1760];         // The z-buffer, stores the depth of each character on screen.
    char b[1760];          // The character buffer, holds the ASCII character for each position.

    // --- Start of Animation ---
    printf("\x1b[2J");     // ANSI escape code to clear the screen at the beginning.
    
    // Infinite loop to continuously draw frames.
    for(;;) {
        // --- Prepare for New Frame ---
        // Clear the buffers for the new frame.
        memset(b, 32, 1760);       // Fill the character buffer with spaces (ASCII 32).
        memset(z, 0, 7040);        // Fill the z-buffer with 0s. 7040 = 1760 * sizeof(float).

        // --- Calculation Loops (The Core Math) ---
        // These nested loops iterate through every point on the surface of the torus.
        // j corresponds to the large circle of the torus (theta).
        for(j = 0; j < 6.28; j += 0.07) {
            // i corresponds to the small, cross-sectional circle (phi).
            for(i = 0; i < 6.28; i += 0.02) {
                // Pre-calculate sines and cosines for the current angles.
                float c = sin(i);
                float d = cos(j);
                float e = sin(A);
                float f = sin(j);
                float g = cos(A);
                float l = cos(i);
                float m = cos(B);
                float n = sin(B);

                // --- 3D Rotation and Projection Math ---
                float h = d + 2; // Part of the torus equation, related to R1 + R2.
                
                // D is the inverse of the point's z-coordinate (1/z). This is the key to perspective.
                // The +5 pushes the donut away from the viewer.
                float D = 1 / (c * h * e + f * g + 5); 

                // t is an intermediate term from the rotation matrix multiplication.
                float t = c * h * g - f * e;

                // --- Calculate Screen Coordinates ---
                // x and y are the final 2D coordinates on the screen.
                // The formulas combine rotation matrices for angles A and B, and projection using D.
                int x = 40 + 30 * D * (l * h * m - t * n);
                int y = 12 + 15 * D * (l * h * n + t * m);

                // --- Calculate Buffer Index and Luminance ---
                // o is the 1D index into our 1D buffers (b and z) from the 2D (x, y) coords.
                int o = x + 80 * y;
                
                // N is the luminance index. It's the dot product of the surface normal and the light vector, scaled by 8.
                int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);

                // --- Z-Buffering and Drawing ---
                // Check if the point is within the screen bounds AND if it's closer than what's already in the z-buffer.
                if(22 > y && y > 0 && 80 > x && x > 0 && D > z[o]) {
                    z[o] = D; // Update z-buffer with the new, closer depth.
                    // Select the character based on the luminance index N.
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
            }
        }

        // --- Render the Frame ---z
        printf("\x1b[H"); // ANSI escape code to move cursor to the top-left (home).
        
        // Loop through the character buffer and print it to the screen.
        for(k = 0; k < 1761; k++) {
            // A trick to print newlines: if k is a multiple of 80, print newline (ASCII 10), otherwise print the character.
            putchar(k % 80 ? b[k] : 10);
            
            // Increment rotation angles inside the print loop for the next frame.
            A += 0.00001;
            B += 0.000005;
        }
    }
    return 0;
}