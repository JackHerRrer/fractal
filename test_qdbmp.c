#include <stdlib.h>
#include <stdio.h>
#include "qdbmp.h"

#define WIDTH_IM 9000  //nbr pixel abscisse
#define HIGHT_IM 9000  //nbr pixel ordonn�e

typedef struct PIXEL{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}PIXEL;

void fillBMPImage(PIXEL image[WIDTH_IM][HIGHT_IM], BMP *bmpImage)
{
    for (int i = 0; i < WIDTH_IM; i++)
    {
        for (int j = 0; j < HIGHT_IM; j++)
        {
            BMP_SetPixelRGB(bmpImage, i, j, image[i][j].r, image[i][j].g, image[i][j].b);
        }
    }
}

int main ()
{
    // table of pixels holding the fractal
    PIXEL **fractal;

    fractal = (PIXEL **)malloc(WIDTH_IM * sizeof(PIXEL *));
    for (int i = 0; i < WIDTH_IM; i++)
        fractal[i] = (PIXEL *)malloc(HIGHT_IM * sizeof(PIXEL));

  /*
    for (int i = 0; i < WIDTH_IM; i++)
    {
        for (int j = 0; j < HIGHT_IM; j++)
        {
            fractal[i][j].r = j % 255;
            fractal[i][j].g = i % 255;
            fractal[i][j].b = 255 - j % 255;
        }
    }
    */
    // bitmap image 
    BMP *bmpImage;

    // Create a new bmp image from scratch.
    bmpImage = BMP_Create(WIDTH_IM, HIGHT_IM, 24);
    BMP_CHECK_ERROR(stderr, -3);

    for (int i = 0; i < HIGHT_IM; i++){
        for (int j = 0; j < WIDTH_IM; j++){
                BMP_SetPixelRGB(bmpImage, i, j, j % 255, i % 255, 255 - j % 255);
            }
        }

    // fill the bmp image from the fractal's pixels 
    //fillBMPImage(fractal, bmpImage);

    /* Save output image */
    BMP_WriteFile(bmpImage, "test_fractal.bmp");
    BMP_CHECK_ERROR(stderr, -5);

    /* Free output image memory */
    BMP_Free(bmpImage);

    return 0;
}
