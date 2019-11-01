__kernel void BN(__global float* input,__global float* output,__global int* rows,__global int* cols, float gamma, float beta , float mean, float var,int Batch Size )
{

int column = get_global_id(0);
	int row = get_global_id(1);
int nor;
nor = (*(input+row**cols+col) - mean) / sqrt(var);
*(output+row**cols+col)= gamma*nor+beta;

}
