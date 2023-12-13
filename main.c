#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_COLORS 100

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} Color;

Color colors[MAX_COLORS];
int numColors = 0;

void parseColor(const char* line) {
    unsigned int r, g, b, a;
    if (sscanf_s(line, "%u,%u,%u,%u", &r, &g, &b, &a) == 4) {
        // Parsing rgba
        if(r < 256 && g < 256 && b < 256 && a < 256) {
            colors[numColors].red = (unsigned char)r;
            colors[numColors].green = (unsigned char)g;
            colors[numColors].blue = (unsigned char)b;
            colors[numColors].alpha = (unsigned char)a;
            numColors++;
        }
    } else {
        // Parsing hex with 3 digits
        if(strlen(line) == 3 && sscanf_s(line, "%x%x%x", &r, &g, &b) == 3)
        {
            colors[numColors].red = (unsigned char)r;
            colors[numColors].green = (unsigned char)g;
            colors[numColors].blue = (unsigned char)b;
            colors[numColors].alpha = 255;
            numColors++;
        } else {
            // Parsing hex with alpha
            char hexResult = sscanf_s(line, "%2x%2x%2x%2x", &r, &g, &b, &a);
            if(hexResult == 4) {
                colors[numColors].red = (unsigned char)r;
                colors[numColors].green = (unsigned char)g;
                colors[numColors].blue = (unsigned char)b;
                colors[numColors].alpha = (unsigned char)a;
                numColors++;
            } else if(hexResult == 3) {
                // Parsing hex without alpha
                colors[numColors].red = (unsigned char)r;
                colors[numColors].green = (unsigned char)g;
                colors[numColors].blue = (unsigned char)b;
                colors[numColors].alpha = 255;
                numColors++;
            }
        }
    }
}

void readColorsFromFile() {
    FILE* file = fopen("colors.txt", "r");
    if (file == NULL) {
        printf("Failed to open file 'colors.txt'\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\r\n")] = '\0'; // Remove newline characters
        if (strlen(line) > 0)
            parseColor(line);
    }

    fclose(file);
}

void printColorInfo(Color color) {
    printf("Red: %d\n", color.red);
    printf("Green: %d\n", color.green);
    printf("Blue: %d\n", color.blue);
    printf("Alpha: %d\n", color.alpha);

    printf("Hex: #%02x%02x%02x%02x\n", color.red, color.green, color.blue, color.alpha);

    //HSL conversion
    float hue, saturation, lightness;
    float r = color.red / 255.0f;
    float g = color.green / 255.0f;
    float b = color.blue / 255.0f;
    float max = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
    float min = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
    float delta = max - min;

    // Calculate hue
    if (delta == 0) {
        hue = 0;
    } else if (max == r) {
        hue = 60 * (((g - b) / delta) + 6);
    } else if (max == g) {
        hue = 60 * (((b - r) / delta) + 2);
    } else {
        hue = 60 * (((r - g) / delta) + 4);
    }
    if (hue < 0)
        hue += 360;

    // Calculate lightness
    lightness = (max + min) / 2;

    // Calculate saturation
    if (delta == 0) {
        saturation = 0;
    } else {
        saturation = delta / (1 - fabs(2 * lightness - 1));
    }

    printf("Hue: %.2f\n", hue);
    printf("Saturation: %.2f\n", saturation);
    printf("Lightness: %.2f\n", lightness);
    printf("\n");
//pointless comment
}

Color mixColors() {
    // Cant be in color struct, cause char would overflow
    unsigned int resultR = 0, resultG = 0, resultB = 0, resultA = 0;
    for (int i = 0; i < numColors; i++) {
        resultR += colors[i].red;
        resultG += colors[i].green;
        resultB += colors[i].blue;
        resultA += colors[i].alpha;
    }
    if (numColors != 0) {
        Color result = {resultR / numColors, resultG / numColors, resultB / numColors, resultA / numColors};
        return result;
    } else {
        Color result = {0,0,0,0};
        printf("No colors in file, default values!\n");
        return result;
    }
}

Color findLowestColor() {
    if (numColors != 0) {
        Color result = {255, 255, 255, 255};
        for (int i = 0; i < numColors; i++) {
            if (colors[i].red < result.red)
                result.red = colors[i].red;
            if (colors[i].green < result.green)
                result.green = colors[i].green;
            if (colors[i].blue < result.blue)
                result.blue = colors[i].blue;
            if (colors[i].alpha < result.alpha)
                result.alpha = colors[i].alpha;
        }
        return result;
    } else {
        Color result = {0,0,0,0};
        printf("No colors in file, default values!\n");
        return result;
    }
}

Color findHighestColor() {
    if (numColors != 0) {
    Color result = {0, 0, 0, 0};
    for (int i = 0; i < numColors; i++) {
        if (colors[i].red > result.red)
            result.red = colors[i].red;
        if (colors[i].green > result.green)
            result.green = colors[i].green;
        if (colors[i].blue > result.blue)
            result.blue = colors[i].blue;
        if (colors[i].alpha > result.alpha)
            result.alpha = colors[i].alpha;
    }
    return result;
    } else {
    Color result = {0,0,0,0};
    printf("No colors in file, default values!\n");
    return result;
    }
}

Color mixSaturateColors() {
     if (numColors != 0) {
    Color result = colors[numColors-1];

    float sumSaturation = 0.0f;
    for (int i = 0; i < numColors-1; i++) {
        float r = colors[i].red / 255.0f;
        float g = colors[i].green / 255.0f;
        float b = colors[i].blue / 255.0f;
        float max = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
        float min = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
        float delta = max - min;
        float saturation = (delta == 0) ? 0 : delta / (1 - fabs(2 * ((r + g + b) / 3) - 1));
        sumSaturation += saturation;
    }
    result.red = result.green = result.blue = (unsigned char)(sumSaturation / numColors * 255);
    return result;
     } else
     {
    Color result = {0,0,0,0};
    printf("No colors in file, default values!\n");
    return result;
     }
}

int main(int argc, char* argv[]) {
    readColorsFromFile();
    char* mode = "mix";
    if (argc > 2 && (strcmp(argv[1], "--mode") == 0 || strcmp(argv[1], "-m") == 0))
        mode = argv[2];

    for(int i = 3; i < argc; i++)
        parseColor(argv[i]);

    Color result;
    if (strcmp(mode, "lowest") == 0)
        result = findLowestColor();
    else if (strcmp(mode, "highest") == 0)
        result = findHighestColor();
    else if (strcmp(mode, "mix-saturate") == 0)
        result = mixSaturateColors();
    else
        result = mixColors();

    printf("Mode: %s\n", mode);
    printf("RESULT COLOR\n");
    printColorInfo(result);

    // arguments test
    //    for(int i = 0; i < argc; i++)
    //    {
    //        printf(argv[i]);
    //    }
    // colors from file test
    //     for (int i = 0; i < numColors; i++) {
    //         printf("Color %d:\n", i + 1);
    //         printColorInfo(colors[i]);
    //     }

    return 0;
}
