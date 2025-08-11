#include <iostream>

#include "suitesparse_matrix.h"

size_t *SuiteSparseMatrix::jc()
{
    return mxGetJc(A); // Column pointers (size cols+1)
}

size_t *SuiteSparseMatrix::ir()
{
    return mxGetIr(A); // Row indices (size nnz)
}

double *SuiteSparseMatrix::data()
{
    return mxGetPr(A);
}

size_t SuiteSparseMatrix::cols()
{
    return mxGetN(A);
}

size_t SuiteSparseMatrix::rows()
{
    return mxGetM(A);
}

size_t SuiteSparseMatrix::nnz()
{
    return jc()[cols()];
}

size_t SuiteSparseMatrix::data_width()
{
    return mxGetElementSize(A);
}

size_t SuiteSparseMatrix::size()
{
    return mxGetNumberOfElements(A);
}

SuiteSparseMatrix::SuiteSparseMatrix(const std::string &mat_file_name, const std::string &arr, const std::string &field)
{
    using namespace std::string_literals;

    mfPtr = matOpen(mat_file_name.c_str(), "r");
    if (mfPtr == NULL)
    {
        throw std::runtime_error("Error opening file "s + mat_file_name);
    }

    aPtr = matGetVariable(mfPtr, arr.c_str());
    if (aPtr == NULL)
    {
        throw std::invalid_argument("mxArray not found "s + arr);
    }

    if (mxGetClassID(aPtr) != mxSTRUCT_CLASS)
    {
        mxDestroyArray(aPtr);
        throw std::invalid_argument(arr + " is not a structure"s);
    }

    if (mxGetFieldNumber(aPtr, field.c_str()) == -1)
    {
        mxDestroyArray(aPtr);
        throw std::invalid_argument("field not found: "s + field);
    }

    A = mxGetField(aPtr, 0, field.c_str());

    if (!mxIsSparse(A))
    {
        throw std::invalid_argument("matrix is not sparse");
    }

    mwSize rows = mxGetM(A);
    mwSize cols = mxGetN(A);
    if (rows != cols)
    {
        throw std::invalid_argument("error reading SPD matrix: m != n");
    }
}

SuiteSparseMatrix::~SuiteSparseMatrix()
{
    mxDestroyArray(aPtr);
    if (matClose(mfPtr) != 0)
    {
        std::cerr << "Error closing mat file" << std::endl;
        exit(1);
    }
}