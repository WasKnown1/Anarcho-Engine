__kernel void simple_add(__global const int* A, __global const int* B, __global int* C) {
    int index = get_global_id(0);
    C[index] = A[index] + B[index];
}