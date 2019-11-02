#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "gold.h"
float* readImgtxt(char *filename){
	float *img;
	int channels = 3;
	int size = 32;
	img = new float [size*size*channels];

	FILE *fp = fopen(filename, "r");
	
	if (fp == NULL){
		std::cout<<"test image open failed!";
		exit(-1);
	}

	for(int channels=0; channels<3; channels++){
		for(int i=0; i<size; i++){
			for(int j=0; j<size; j++)
				fscanf(fp, "%f\n", img + i*size + j + channels*(size*size));
		}
	}
	return img;
}
float* convolutionGoldFloat(float *image, int rows, int cols, float *filter, int filterWidth,float * biases)
{	char* inputImagePath = "snail.txt";
hInputImage = readImgtxt(inputImagePath);
   /* Allocate space for the filtered image */
   float *outputImage;
   outputImage = new float [imageRows*imageCols*out_channels];
   if (!outputImage) { exit(-1); }

   /* Compute the filter width (intentionally truncate) */
   int halfFilterWidth = (int)filterWidth/2;

   /* Iterate over the rows of the source image */
   int i; 
   int j;
   int k;
   int l;
    for (out_channel = 0; out_channel<3; out_channel++) 
      {
   for (i = 0; i < rows; i++) 
   {
      /* Iterate over the columns of the source image */
      for (j = 0; j <cols; j++) 
      {
         /* Reset sum for new source pixel */
        
         float sum = 0;
         	for(int in_channel=0; in_channel<*in_channels; in_channel++){
         /* Apply the filter to the neighborhood */
         for (k = -halfFilterWidth; k <= halfFilterWidth; k++) 
         {
            for (l = -halfFilterWidth; l <= halfFilterWidth; l++)
            {
               /* Indices used to access the image */
               int r = i+k;
               int c = j+l;
               
               /* Handle out-of-bounds locations by clamping to
                * the border pixel */
               r = (r < 0) ? 0 : r;
               c = (c < 0) ? 0 : c;
               r = (r >= rows) ? rows-1 : r;
               c = (c >= cols) ? cols-1 : c;       
               
               sum += image[r*cols+c+in_channel*(*rows**cols)] *
                      filter[(k+halfFilterWidth)*filterWidth + 
                         (l+halfFilterWidth)+ in_channel*(*kernel_size**kernel_size) + out_channel*(out_kernel_size)];
            }
         }
         }
         /* Write the new pixel value */
         outputImage[i*cols+j+ out_channel*(*rows * *cols)] = sum+*(bias + out_channel);
      }
   }
}
   return outputImage;
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
}
