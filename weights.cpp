#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
using namespace std;

class Conv2D
{
	//private:
	public:
	
    	int numChannels;
		int kernelWidth;
		int kernelHeight;
		int kernelDepth;
		string *layerName;
		string *weightFilePath;
    	double ****weights;
		double *biases;

		FILE *filePtr;

		Conv2D(string weightFilePath)
		{
			this->weightFilePath = new string(weightFilePath);
			this->layerName = NULL;

			bool status = readFile();
			
			if(status == 0)
				throw std::invalid_argument("read failed, please make sure you are provding correct file path...");
			else
				cout<<"Pointer to file "<<*(this->weightFilePath)<<" opened successfully..."<<endl;
			
			// Get layer name
			parseLayerName();

			// Get kernel dimensions
			parseKernelDimensions();

			// Allocate space to hold weights
			allocateSpace();

			// Parse weights value into array
			parseWeights();

			// Parse biases value into array
			parseBiases();

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
	string weightFilePath("conv2d_1.txt");
	Conv2D layer1(weightFilePath);
	layer1.layerSummary();

	
	for(int i=0; i<layer1.kernelWidth; i++)
	{
		for(int j=0; j<layer1.kernelHeight; j++)
		{
			for(int k=0; k<layer1.kernelDepth; k++)
				cout<<layer1.weights[0][i][j][k]<<" ";
		}
	}
	

	
	/*for(int i=0; i<128; i++)
		cout<<layer1.biases[i]<<" ";
	cout<<endl;
	*/

	return 0;
}