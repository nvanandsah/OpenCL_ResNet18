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

float* readImgtxt(const char *filename, int* rows, int* cols){}
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

class Conv2D
{
	public:
		int numChannels;
		int kernelWidth;
		int kernelHeight;
		int kernelDepth;
		string *layerName;
		string *weightFilePath;
		FILE *filePtr;
		double ****weights;
		double *biases;

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

	char* inputImagePath = "lion.txt";

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
   hInputImage = readImgtxt(inputImagePath, &imageRows, &imageCols);

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


	string weightFilePath("conv2d_1.txt");
	Conv2D layer1(weightFilePath);
	layer1.layerSummary();

	
	for(int i=0; i<layer1.kernelWidth; i++){
		for(int j=0; j<layer1.kernelHeight; j++){
			for(int k=0; k<layer1.kernelDepth; k++)
				cout<<layer1.weights[0][i][j][k]<<" ";
		}
	}
    /// Layer 1
		int in_channels, out_channels, kernel_size, imgRows, imgCols;
		in_channels = 1;
		out_channels = 1;
		kernel_size = 3;
		imgRows = imageRows;
		imgCols = imageCols;	
		cl::Buffer inputBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, in_channels*imgRows*imgCols*sizeof(float));
		cl::Buffer filterBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, in_channels*out_channels*kernel_size*kernel_size*sizeof(float));
		cl::Buffer biasBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, out_channels*sizeof(float));
		cl::Buffer outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, out_channels*imgRows*imgCols*sizeof(float));
		cl::Buffer in_channelsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int));
		cl::Buffer out_channelsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int));
		cl::Buffer kernelSizeBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int));
		cl::Buffer imgRowsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int));
		cl::Buffer imgColsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int));

		queue.enqueueWriteBuffer(inputBuffer, CL_TRUE, 0, in_channels*imgRows*imgCols*sizeof(float), hInputImage);
		queue.enqueueWriteBuffer(filterBuffer, CL_TRUE, 0, in_channels*out_channels*kernel_size*kernel_size*sizeof(float), filter);
		queue.enqueueWriteBuffer(biasBuffer, CL_TRUE, 0, out_channels*sizeof(float), );
		queue.enqueueWriteBuffer(outputBuffer, CL_TRUE, 0, out_channels*imgRows*imgCols*sizeof(float), output_buffer);
		queue.enqueueWriteBuffer(in_channelsBuffer, CL_TRUE, 0, sizeof(int), &in_channels);
		queue.enqueueWriteBuffer(out_channelsBuffer, CL_TRUE, 0, sizeof(int), &out_channels);
		queue.enqueueWriteBuffer(kernelSizeBuffer, CL_TRUE, 0, sizeof(int), &kernel_size);
		queue.enqueueWriteBuffer(imgRowsBuffer, CL_TRUE, 0, sizeof(int), &imgRows);
		queue.enqueueWriteBuffer(imgColsBuffer, CL_TRUE, 0, sizeof(int), &imgCols);

		std::ifstream sourceFile("Kernels/conv.cl");
        std::string sourceCode(
         std::istreambuf_iterator<char>(sourceFile),
         (std::istreambuf_iterator<char>()));
         cl::Program::Sources source(1,
         std::make_pair(sourceCode.c_str(),
         sourceCode.length() + 1));

     	cl::Program program = cl::Program(context, source);

     	program.build(devices);
     	
     	cl::Kernel kernel(program, "convolution");

     	kernel.setArg(0, out_channelsBuffer);
     	kernel.setArg(1, in_channelsBuffer);
     	kernel.setArg(2, kernelSizeBuffer);
     	kernel.setArg(3, inputBuffer);
     	kernel.setArg(4, filterBuffer);
     	kernel.setArg(5, biasBuffer);
     	kernel.setArg(6, outputBuffer);
     	kernel.setArg(7, imgRowsBuffer);
     	kernel.setArg(8, imgColsBuffer);

     	cl::NDRange global(imgCols, imgRows);
     	cl::NDRange local(2, 2);
      cl::Event event;
     	queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local,NULL,&event);
      queue.finish();
     	// Read data back
     	queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, out_channels*imgRows*imgCols*sizeof(float), hOutputImage);
     cl_ulong time_start;
     cl_ulong time_end;
     
     event.wait();
    double total_time;
    event.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end); 
    event.getProfilingInfo(CL_PROFILING_COMMAND_START, &time_start);
    total_time = time_end - time_start;

/* Results */
std::cout << "Execution time in milliseconds for convolution layer " << total_time*1.0e-6f << std::endl;   
	}
	catch(cl::Error error)
	{
		std::cout << error.what() << "(" << error.err() << ")" <<std::endl;
	}
    weight_count = weight_count+2;
    for (int p = 0;p<(out_channels*imgRows*imgCols);p++)
          { 
               
              input_buffer[p] = output_buffer[p]; 
      }
    }
   
    else if (layer[j][0]==1)
      {
  

    
    
        
      
   

