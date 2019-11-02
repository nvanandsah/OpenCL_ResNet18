__kernel void max_pool2d(__global int* channels, __global int* inDim, __constant int* pool_size, __global int* outDim, __global float* inImg, __global float* outImg)
{

	int column = get_global_id(0);
	int row = get_global_id(1);

	/*
	if (row ==0 && column == 0)
	{
		printf("Channels : %d, inDim : %d, Pool Size : %d, outDim : %d, ",*channels, *inDim, *pool_size, *outDim);
	}*/

	float values[4]; // Fix this

	for(int channel=0; channel<*channels; channel++)
	{

		float sum = 0;

		for(int i=0; i<*pool_size; i++)
		{
			for(int j=0; j<*pool_size; j++)
			{
				int r, c;
				r = row**pool_size + i;
				c = column**pool_size + j;

				sum+ = *(inImg + r**inDim + c + channel*(*inDim * *inDim));
				float avg =sum/4;
			}
		}

		

		*(outImg + row**outDim + column + channel*(*outDim * *outDim)) = avg;
	}
}
