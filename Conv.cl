__kernel void convolution( 
                        __global int* out_channels, 
                        __global int* in_channels, 
                        __global int* kernel_size, 
                        __global float * img, 
                        __global float * weights, 
                        __constant float * bias, 
                        __global float * out_img, 
                        __global int* rows, 
                        __global int* cols)
{
	int column = get_global_id(0);
	int row = get_global_id(1);

	/*
	if (column == 0 && row == 0)
	{
		printf("Out Channels : %d, In Channels : %d, Kernel Size : %d \n",*out_channels, *in_channels, *kernel_size);


		for(int in_channel=0; in_channel<*in_channels; in_channel++)
		{
			printf("Channel : %d\n",in_channel+1);

			
			for(int i=0; i<*rows; i++)
			{
				for(int j=0; j<*cols; j++)
				{
					printf("%.1f ",*(img + i**rows + j + in_channel*(*cols**rows)));
				}
			printf("\n");
			}
		}
		
	}
	*/

	int halfFilterWidth = (int)((*kernel_size)/2);
	int out_kernel_size = *in_channels * *kernel_size * *kernel_size;

	for(int out_channel=0; out_channel<=*out_channels; out_channel++)
	{
		float sum =  0.0;
		
		for(int in_channel=0; in_channel<*in_channels; in_channel++)
		{

			for(int k=-halfFilterWidth; k<=halfFilterWidth; k++)
			{
				for(int l=-halfFilterWidth; l<=halfFilterWidth; l++)
				{
					int r = row + k;
					int c = column + l;

					if (r>=0 && c>=0 && r<*rows && c<*cols)
					{
						int kernel_row = k + halfFilterWidth;
						int kernel_column = l + halfFilterWidth;
						sum += *(img + r**rows + c + in_channel*(*rows**cols)) * *(weights + kernel_row**kernel_size + kernel_column + in_channel*(*kernel_size**kernel_size) + out_channel*(out_kernel_size));
					}

				}
			}
		}


		*(out_img + row**rows + column + out_channel*(*rows * *cols)) = sum + *(bias + out_channel);

		// ReLU Implementation
		if ( *(out_img + row**rows + column + out_channel*(*rows * *cols)) < 0.0)
		{
			*(out_img + row**rows + column + out_channel*(*rows * *cols)) = 0.0;
		}
	}

}
