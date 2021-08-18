#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "qdbmp.h"

#define WIDTH_IM 1600  //nbr pixel abscisse 3000
#define HIGHT_IM 1050  //nbr pixel ordonnée 1876

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




// This function processes the fractal. 
//                                        
// For each pixel, the function solves the following serie: Zn+1 = Zn^3 + Zc    
// until the norme of Zn+1 is superior to a value : ||Zn+1|| > blowUp           
// or until the iteration limit is reached                                      
// The value associated to the pixel is the number of iteration                 
// - Zn and Zc are complex values                                               
// - Z0 = x + yi where x and y are the absciss and the ordinate of the pixel    
// - Zc = ConstReel + constIm * i                                             
// - To reduce the process time, the square of the norm is compared to the square of the blowUp value 
// ( a² + b² > blowUp² instead of sqrt(a² + b²) > blowUp )  
//
// Known values that give a beautiful render are:
// constReel = 0.195
// constIm = 0.889 or 0.89, 
// blowUpCarre = 1.5 or 1.1
// iterMax = 127
// double = 1.3
void processFractalTriskel(PIXEL **fractal, double const constReel, double const constIm, double blowUpCarre, int const iterMax, double const zoom, double const angle)
{

    double reel, xIm, norme, prevReel;        //variable used to process the next value of the serie (Zn+1)

    double centreX = -0.8, centreY = 0.18;       //point au centre de la fractale
    //double zoom = 1.3;                    // x = 1.3; y = 1.3
    double scaledZoom = zoom * WIDTH_IM / HIGHT_IM;

    // The variable that determines the shape of the fractal
    // double constReel = 0.195; //0.195
    // double constIm = 0.889; //0.89, 0.889
    // double blowUpCarre = 1.5; // cool values: 1.1, 1.5 // 
    // int iterMax = 127; //360

    int i, j, nbrIter;

    // for each pixels
    for(i = 0; i < WIDTH_IM; i++){
        for(j = 0; j < HIGHT_IM; j++){
            // set the initial values of the serie (Z0)
            reel = (centreX - zoom) + 2*scaledZoom*i/WIDTH_IM;
            xIm = (centreY - zoom) + 2*zoom*j/HIGHT_IM;

            // reset the number of iteration
            nbrIter = 0;

            do{
                prevReel = reel;
                // process the real value of the next iteration of the serie
                reel = reel*reel*reel-3*reel*xIm*xIm + constReel;
                // process the imaginary value of the next iteration of the serie
                xIm = -xIm*xIm*xIm+3*prevReel*prevReel*xIm+ constIm;
                // process the square of the norm of the next iteration of the serie
                norme = xIm*xIm+reel*reel;
                // increment the number of iteration
                nbrIter++;
            }
            // until the number of iteration reaches the max iteration amount or the norm is too big
            while(nbrIter < iterMax && norme < blowUpCarre);

            // Convert the number of iteration to a color using a cool warm color map
            fractal[i][j] = getColor(nbrIter, iterMax);

        }
    }
}

// Zn+1 = Zn^2
// Z0 = x + z + yi where x and z are the absciss and the ordinate of the pixel in the real plan and y is the position int the imaginary plan
void process3DFractalSquare(PIXEL **fractal, 
                            double const constReel, 
                            double const constIm, 
                            double blowUpCarre, 
                            int const iterMax, 
                            double const zoom, 
                            double const z,
                            double const y)
{

    double reel, im, norme, prevReel;        //variable used to process the next value of the serie (Zn+1)

    double centreX = -0.8, centreIm = 0.18;       //point au centre de la fractale
    //double zoom = 1.3;                    // x = 1.3; y = 1.3
    double scaledZoom = zoom * WIDTH_IM / HIGHT_IM;


    int i, j, nbrIter;

    // for each pixels
    for(i = 0; i < WIDTH_IM; i++){
        for(j = 0; j < HIGHT_IM; j++){
            // set the initial values of the serie (Z0)
            reel = (centreX - zoom) + 2*scaledZoom*i/WIDTH_IM;
            im = (centreIm - zoom) + 2*zoom*j/HIGHT_IM;

            // reset the number of iteration
            nbrIter = 0;

            do{
                prevReel = reel;
                // process the real value of the next iteration of the serie
                reel = reel*reel*-im*im + constReel;
                // process the imaginary value of the next iteration of the serie
                im = 2*prevReel*im + constIm;
                // process the square of the norm of the next iteration of the serie
                norme = im*im+reel*reel;
                // increment the number of iteration
                nbrIter++;
            }
            // until the number of iteration reaches the max iteration amount or the norm is too big
            while(nbrIter < iterMax && norme < blowUpCarre);

            // Convert the number of iteration to a color using a cool warm color map
            fractal[i][j] = getColor(nbrIter, iterMax);

        }
    }
}

// ./fractal fractal.bmp 0.69 0.185 2000 80 3
// ./fractal fractal.bmp 0.722 0.23 2000 130 3
// Zn+1 = Zn^4 + Zc
void processFractal(PIXEL **fractal, double const constReel, double const constIm, double blowUpCarre, int const iterMax, double const zoom)
{

    double reel, xIm, norme, prevReel;        //variable used to process the next value of the serie (Zn+1)

    double centreX = -0.0, centreY = 0.18;       //point au centre de la fractale
    //double zoom = 1.3;                    // x = 1.3; y = 1.3
    double scaledZoom = zoom * WIDTH_IM / HIGHT_IM;

    // The variable that determines the shape of the fractal
    // double constReel = 0.195; //0.195
    // double constIm = 0.889; //0.89, 0.889
    // double blowUpCarre = 1.5; // cool values: 1.1, 1.5 // 
    // int iterMax = 127; //360

    int i, j, nbrIter;

    // for each pixels
    for(i = 0; i < WIDTH_IM; i++){
        for(j = 0; j < HIGHT_IM; j++){
            // set the initial values of the serie (Z0)
            reel = (centreX - zoom) + 2*scaledZoom*i/WIDTH_IM;
            xIm = (centreY - zoom) + 2*zoom*j/HIGHT_IM;

            // reset the number of iteration
            nbrIter = 0;

            do{
                prevReel = reel;
                // process the real value of the next iteration of the serie
                reel = reel*reel*reel*reel-6*reel*reel*xIm*xIm + constReel;
                // process the imaginary value of the next iteration of the serie
                xIm = 4*prevReel*prevReel*prevReel*xIm-4*prevReel*xIm*xIm*xIm+ constIm;
                // process the square of the norm of the next iteration of the serie
                norme = xIm*xIm+reel*reel;
                // increment the number of iteration
                nbrIter++;
            }
            // until the number of iteration reaches the max iteration amount or the norm is too big
            while(nbrIter < iterMax && norme < blowUpCarre);

            // Convert the number of iteration to a color using a cool warm color map
            fractal[i][j] = getColor(nbrIter, iterMax);

        }
    }
}


int main (int argc, char *argv[])
{   
    // check that the correct amount of parameters have been provided to the program
    if (argc != 7){
        printf("use: fractal output constReel constIm blowUpCarre iterMax zoom\n");
        printf("example: fractal fractal.bmp 0.195 0.889 1.5 127 1.3\n");
        return -1;
    }

    double constReel, constIm, blowUpCarre, zoom;
    int iterMax;

    // retrieve the parameters 
    constReel = atof(argv[2]);
    constIm = atof(argv[3]);
    blowUpCarre = atof(argv[4]);
    iterMax = atoi(argv[5]);
    zoom = atof(argv[6]);

    // 2d array of pixels used to hold the fractal's pixels
    PIXEL **fractal;

    // a dynamic allocation is used instead of a simple 2d array declaration (PIXEL fractal[w][h])
    // so high resolution images can be handled 
    fractal = (PIXEL **)malloc(WIDTH_IM * sizeof(PIXEL *));
    for (int i = 0; i < WIDTH_IM; i++)
        fractal[i] = (PIXEL *)malloc(HIGHT_IM * sizeof(PIXEL));

    // Fill the array with the fractal
    //processFractalTriskel(fractal, constReel, constIm, blowUpCarre, iterMax, zoom, 0);
    process3DFractalSquare(fractal, constReel, constIm, blowUpCarre, iterMax, zoom, 0);

    // bitmap image 
    BMP *bmpImage;

    // Create a new bmp image from scratch.
    bmpImage = BMP_Create(WIDTH_IM, HIGHT_IM, 24);
    BMP_CHECK_ERROR(stderr, -3);

    // fill the bmp image from the fractal's pixels 
    fillBMPImage(fractal, bmpImage);

    /* Save output image */
    BMP_WriteFile(bmpImage, argv[1]);
    BMP_CHECK_ERROR(stderr, -5);

    /* Free output image memory */
    BMP_Free(bmpImage);

    // free the 2d array fractal memory
    for (int i = 0; i < WIDTH_IM; i++)
        free(fractal[i]);

    free(fractal);

    return 0;
}
