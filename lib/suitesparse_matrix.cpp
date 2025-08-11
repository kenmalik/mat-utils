#include <iostream>

#include <mat.h>
#include <matrix.h>

#include "suitesparse_matrix.h"

struct SuiteSparseMatrixImpl
{
    MATFile *mat_file_ptr = nullptr;
    mxArray *problem_ptr = nullptr;
    mxArray *A_ptr = nullptr;
};

size_t *SuiteSparseMatrix::jc()
{
    return mxGetJc(impl->A_ptr); // Column ptrs (size cols+1)
}

size_t *SuiteSparseMatrix::ir()
{
    return mxGetIr(impl->A_ptr); // Row indices (size nnz)
}

double *SuiteSparseMatrix::data()
{
    return mxGetPr(impl->A_ptr);
}

size_t SuiteSparseMatrix::cols()
{
    return mxGetN(impl->A_ptr);
}

size_t SuiteSparseMatrix::rows()
{
    return mxGetM(impl->A_ptr);
}

size_t SuiteSparseMatrix::nnz()
{
    return jc()[cols()];
}

size_t SuiteSparseMatrix::data_width()
{
    return mxGetElementSize(impl->A_ptr);
}

size_t SuiteSparseMatrix::size()
{
    return mxGetNumberOfElements(impl->A_ptr);
}

SuiteSparseMatrix::SuiteSparseMatrix(
    const std::string &mat_file_name,
    const std::string &arr,
    const std::string &field) : impl(new SuiteSparseMatrixImpl())
{
    using namespace std::string_literals;

    impl->mat_file_ptr = matOpen(mat_file_name.c_str(), "r");
    if (impl->mat_file_ptr == NULL)
    {
        throw std::runtime_error("Error opening file "s + mat_file_name);
    }

    impl->problem_ptr = matGetVariable(impl->mat_file_ptr, arr.c_str());
    if (impl->problem_ptr == NULL)
    {
        throw std::invalid_argument("mxArray not found "s + arr);
    }

    if (mxGetClassID(impl->problem_ptr) != mxSTRUCT_CLASS)
    {
        mxDestroyArray(impl->problem_ptr);
        throw std::invalid_argument(arr + " is not a structure"s);
    }

    if (mxGetFieldNumber(impl->problem_ptr, field.c_str()) == -1)
    {
        mxDestroyArray(impl->problem_ptr);
        throw std::invalid_argument("field not found: "s + field);
    }

    constexpr int INDEX = 0;
    impl->A_ptr = mxGetField(impl->problem_ptr, INDEX, field.c_str());

    if (!mxIsSparse(impl->A_ptr))
    {
        throw std::invalid_argument("matrix is not sparse");
    }

    mwSize rows = mxGetM(impl->A_ptr);
    mwSize cols = mxGetN(impl->A_ptr);
    if (rows != cols)
    {
        throw std::invalid_argument("error reading SPD matrix: m != n");
    }
}

SuiteSparseMatrix::~SuiteSparseMatrix()
{
    mxDestroyArray(impl->problem_ptr);
    if (matClose(impl->mat_file_ptr) != 0)
    {
        std::cerr << "Error closing mat file" << std::endl;
        exit(1);
    }
    delete impl;
}