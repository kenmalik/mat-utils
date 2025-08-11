#include "mat.h"
#include "matrix.h"

#include <vector>
#include <iostream>

void read_suitesparse(const char *mat_file_name, const char *arr, const char *field)
{
    MATFile *mfPtr;
    mxArray *aPtr;

    mfPtr = matOpen(mat_file_name, "r");
    if (mfPtr == NULL)
    {
        std::cerr << "Error opening file" << mat_file_name << std::endl;
        exit(1);
    }

    aPtr = matGetVariable(mfPtr, arr);
    if (aPtr == NULL)
    {
        std::cerr << "mxArray not found:" << arr << std::endl;
        exit(1);
    }

    if (mxGetClassID(aPtr) != mxSTRUCT_CLASS)
    {
        std::cerr << arr << " is not a structure" << std::endl;
        mxDestroyArray(aPtr);
        exit(1);
    }

    if (mxGetFieldNumber(aPtr, field) == -1)
    {
        std::cerr << "Field not found:" << field << std::endl;
        mxDestroyArray(aPtr);
        exit(1);
    }

    mxArray *A = nullptr;
    A = mxGetField(aPtr, 0, field);

    if (!mxIsSparse(A))
    {
        std::cerr << "Matrix is not sparse!" << std::endl;
        return;
    }

    mwSize rows = mxGetM(A);
    mwSize cols = mxGetN(A);
    if (rows != cols)
    {
        throw std::runtime_error("error reading SPD mat: m != n");
    }

    mwIndex *jc = mxGetJc(A); // Column pointers (size cols+1)
    mwIndex *ir = mxGetIr(A); // Row indices (size nnz)
    double *A_data = mxGetPr(A);  // Non-zero values (size nnz)

    mwSize nnz = jc[cols];

    mwSize data_width = mxGetElementSize(A);
    mwSize size = mxGetNumberOfElements(A);

    std::cout << "Size: " << size << std::endl;
    std::cout << "# Non-Zeros: " << nnz << std::endl;

    mxDestroyArray(aPtr);
    mxDestroyArray(A);
    if (matClose(mfPtr) != 0)
    {
        std::cerr << "Error closing file" << mat_file_name << std::endl;
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }
    read_suitesparse(argv[1], "Problem", "A");
}
