#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "qdbmp.h"

#define WIDTH_IM 1024  //nbr pixel abscisse
#define HIGHT_IM 50  //nbr pixel ordonnée

typedef struct PIXEL{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}PIXEL;

// Map the pixel array to the bmp image
void fillBMPImage(PIXEL **image, BMP *bmpImage)
{
    for (int i = 0; i < WIDTH_IM; i++)
        for (int j = 0; j < HIGHT_IM; j++)
            BMP_SetPixelRGB(bmpImage, i, j, image[i][j].r, image[i][j].g, image[i][j].b);

}


// convert HSV values to RGB
// 0 < hue < 360
// 0 < saturation < 1
// 0 < value < 1
PIXEL getRgbFromHsv(double h, double s, double v)
{
    PIXEL color;
    double c = 0.0, m = 0.0, x = 0.0, r,g,b;

    c = v * s;

    x = c * (1.0 - fabs(fmod(h / 60.0, 2) - 1.0));
    m = v - c;

    if (h >= 0.0 && h < 60.0){
        r = c + m;
        g = x + m;
        b = m;
    }
    else if (h >= 60.0 && h < 120.0){
        r = x + m;
        g = c + m;
        b = m;
    }
    else if (h >= 120.0 && h < 180.0){
        r = m;
        g = c + m;
        b = x + m;
    }
    else if (h >= 180.0 && h < 240.0){
        r = m;
        g = x + m;
        b = c + m;
    }
    else if (h >= 240.0 && h < 300.0){
        r = x + m;
        g = m;
        b = c + m;
    }
    else if (h >= 300.0 && h < 360.0){
        r = c + m;
        g = m;
        b = x + m;
    }
    else{
        printf("Hue > 360\n");
    }

    color.r = (unsigned char) (r*255);
    color.g = (unsigned char) (g*255);
    color.b = (unsigned char) (b*255);

    return color;
}


// returns the RGB values from a cool/warm color map
PIXEL getColor(int const inputValue, int const maxValue){

    // stores the final color
    PIXEL color;

    double hue, saturation, value; 

    // The colorMap ranges from 0 to 2048, 
    // yet the range of the input value could be less (for example: 0 to 127)
    // Therefore the value must be scaled up
    int scaledValue = inputValue * (int)(2048 / (double)maxValue);
 
    // if the value is low this will be a cold color
    // cold colors hue ranges from 245 to 175 
    if (scaledValue < 1024){
        hue = 245 - scaledValue * 0.068359375;
    }
    // if the value is high this will be a warm color
    // warm color hue ranges from 80 to 0
    else {
        hue = 70 - (scaledValue - 1024) * 0.068359375;
    }

    // reduce the saturation in the middle of the color map to smooth out the transition from warm to cool colors
    if (fabs(1024 - scaledValue) < 512){
        saturation = 1 - ((512 - (float)abs(1024 - scaledValue)) / 512);
    }
    else    
        saturation = 1;

    // reduce the value of the extremities 
    if (scaledValue < 512){
        value = 1 - (float)(512 - scaledValue) / 512;
    }
    else if (scaledValue > 1536){
        value = 1 - (float)(scaledValue - 1536 ) / 512;
    }
    else
        value = 1;

    color = getRgbFromHsv(hue, saturation, value);

    return color;
}

int main ()
{   
    
    // 2d array of pixels used to hold the fractal's pixels
    PIXEL **fractal;

    // a dynamic allocation is used instead of a simple 2d array declaration (PIXEL fractal[w][h])
    // so high resolution images can be handled 
    fractal = (PIXEL **)malloc(WIDTH_IM * sizeof(PIXEL *));
    for (int i = 0; i < WIDTH_IM; i++)
        fractal[i] = (PIXEL *)malloc(HIGHT_IM * sizeof(PIXEL));


    // create the gradient
    for (int i = 0; i < WIDTH_IM; i++ ){
        PIXEL pixel = getColor(i, WIDTH_IM);
        for (int j = 0; j < HIGHT_IM; j++)
            fractal[i][j] = pixel;
    }

    // bitmap image 
    BMP *bmpImage;

    // Create a new bmp image from scratch.
    bmpImage = BMP_Create(WIDTH_IM, HIGHT_IM, 24);
    BMP_CHECK_ERROR(stderr, -3);

    // fill the bmp image from the fractal's pixels 
    fillBMPImage(fractal, bmpImage);

    /* Save output image */
    BMP_WriteFile(bmpImage, "gradient.bmp");
    BMP_CHECK_ERROR(stderr, -5);

    /* Free output image memory */
    BMP_Free(bmpImage);

    // free the 2d array fractal memory
    for (int i = 0; i < WIDTH_IM; i++)
        free(fractal[i]);

    free(fractal);

    return 0;
}
