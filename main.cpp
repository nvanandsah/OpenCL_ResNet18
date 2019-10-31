#include <CL/cl.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;

#include "utils.h"
#include "bmp-utils.h"
#include "gold.h"


static const char* inputImagePath = "../../Images/cat.bmp";
class Conv2D
{
	private:
		int numChannels;
		int kernelWidth;
		int kernelHeight;
		int kernelDepth;
		string *layerName;
		string *weightFilePath;
		FILE *filePtr;

	public:
		Conv2D(string weightFilePath)
		{
			this->weightFilePath = new string(weightFilePath);
			this->layerName = NULL;

			bool status = readFile();
			if(status == 0)
				throw std::invalid_argument("read failed, please make sure you are provding correct file path...");
			else
				cout<<"Pointer to file "<<*(this->weightFilePath)<<" opened successfully..."<<endl;
			
			parseLayerName();   // Get layer name
			parseKernelDimensions();    // Get kernel dimensions
			allocateSpace();    // Allocate space to hold weights
			parseWeights();     // Parse weights value into array
			parseBiases();      // Parse biases value into array
		}
		~Conv2D()
		{
			delete this->layerName;
			delete this->weightFilePath;
			deallocateSpace();
		}

		void layerSummary()
		{
			cout<<"Layer Name : "<<*(this->layerName)<<endl;
			cout<<"Kernel Width : "<<this->kernelWidth<<endl;
			cout<<"Kernel Height : "<<this->kernelHeight<<endl;
			cout<<"Kernel Depth : "<<this->kernelDepth<<endl;
			cout<<"Channels : "<<this->numChannels<<endl;
		}

		double ****weights;
		double *biases;

	protected:
		void allocateSpace()
		{	
			/* Allocate Space for weights */
			// number of channels x width x height x depth
			this->weights = new double***[this->numChannels];
			for(int channel=0; channel<(this->numChannels); channel++)
			{
				this->weights[channel] = new double**[this->kernelWidth];
				for(int width=0; width<(this->kernelWidth); width++)
				{
					this->weights[channel][width] = new double*[this->kernelHeight];
					for(int height=0; height<(this->kernelHeight); height++)
					{
						this->weights[channel][width][height] = new double[this->kernelDepth];
					}
				}
			}

			/* Allocate space for biases */
			this->biases = new double[this->numChannels];
		}

		void deallocateSpace()
		{
			/* Deallocate space of weights */
			for(int channel=0; channel<(this->numChannels); channel++)
			{
				for(int width=0; width<(this->kernelWidth); width++)
				{
					for(int height=0; height<(this->kernelHeight); height++)
					{
						delete[] this->weights[channel][width][height];
					}
					delete[] this->weights[channel][width];
				}
				delete[] this->weights[channel];
			}
			delete[] this->weights;
			/* Deallocates space of biases */
			delete[] this->biases;
		}

		bool readFile()
		{	
			this->filePtr = fopen(this->weightFilePath->c_str(), "r");
			if(this->filePtr == 0)
				return false;
			else
				return true;
		}

		void parseLayerName()
		{
			char tmp[100];
			fscanf(this->filePtr, "%s\n", tmp);
			this->layerName = new string(tmp);
		}

		void parseKernelDimensions()
		{
			fscanf(this->filePtr, "%d %d %d %d\n", &this->kernelWidth, &this->kernelHeight, &this->kernelDepth, &this->numChannels);
		}

		void parseWeights()
		{
			for(int channel=0; channel<(this->numChannels); channel++)
			{
				for(int width=0; width<(this->kernelWidth); width++)
				{
					for(int height=0; height<(this->kernelHeight); height++)
					{
						for(int depth=0; depth<(this->kernelDepth); depth++)
							fscanf(this->filePtr, "%lf ", &weights[channel][width][height][depth]);
					}
				}
			}
			fscanf(this->filePtr, "\n");
		}

		void parseBiases()
		{
			for(int channel=0; channel<(this->numChannels); channel++)
				fscanf(this->filePtr,"%lf ", &biases[channel]);
		}
};
int main() 
{
   float *hInputImage;
   float *hOutputImage;

   int imageRows;
   int imageCols;


   static float gaussianBlurFilterFactor = 273.0f;
   static float gaussianBlurFilter[25] = {
    1.0f,  4.0f,  7.0f,  4.0f, 1.0f,
    4.0f, 16.0f, 26.0f, 16.0f, 4.0f,
    7.0f, 26.0f, 41.0f, 26.0f, 7.0f,
    4.0f, 16.0f, 26.0f, 16.0f, 4.0f,
    1.0f,  4.0f,  7.0f,  4.0f, 1.0f};
   static const int gaussianBlurFilterWidth = 5;

   int filterWidth = gaussianBlurFilterWidth;
   float filterFactor = gaussianBlurFilterFactor;
   float *filter = gaussianBlurFilter;   
   for (int i = 0; i < filterWidth*filterWidth; i++) 
   {
      filter[i] = filter[i]/filterFactor;
   }

   /* Read in the BMP image */
   hInputImage = readBmpFloat(inputImagePath, &imageRows, &imageCols);

   /* Allocate space for the output image */
   hOutputImage = new float [imageRows*imageCols];

    /* Query for platforms */
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    /* Get a list of devices on this platform */
    std::vector<cl::Device> devices;
    platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

    /* Create a context for the devices */
    cl::Context context(devices);

    /* Create a command queue for the first device */
    cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);

    /* Create the images */
    cl::ImageFormat imageFormat = cl::ImageFormat(CL_R, CL_FLOAT);
    cl::Image2D inputImage = cl::Image2D(context, CL_MEM_READ_ONLY,
        imageFormat, imageCols, imageRows);
    cl::Image2D outputImage = cl::Image2D(context, CL_MEM_WRITE_ONLY,
        imageFormat, imageCols, imageRows);

    /* Create a buffer for the filter */
    cl::Buffer filterBuffer = cl::Buffer(context, CL_MEM_READ_ONLY,
        filterWidth*filterWidth*sizeof(float));


    /// Layer 1
    

    
    
        
      
   

