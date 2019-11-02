__kernel void BN(__global float* input,__global float* output,__global int* rows,__global int* cols, float gamma,int out_channel, float beta , float mean, float var )
{

int col = get_global_id(0);
	int row = get_global_id(1);
int nor;
	for(in_channel=0;in_channel<*in_channels;in_channel++){
nor = (*(input+row**cols+col+in_channel*(*rows**cols)) - mean) / sqrt(var);
*(output+row**cols+col+out_channel*(*rows * *cols))= gamma*nor+beta;
	}
}
