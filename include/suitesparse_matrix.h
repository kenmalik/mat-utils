#pragma once

#include "mat.h"
#include "matrix.h"

class SuiteSparseMatrix
{
public:
    size_t *jc();
    size_t *ir();
    double *data();

    size_t cols();
    size_t rows();
    size_t nnz();
    size_t data_width();
    size_t size();

    SuiteSparseMatrix(const std::string &mat_file_name, const std::string &arr, const std::string &field);
    ~SuiteSparseMatrix();

private:
    MATFile *mfPtr = nullptr;
    mxArray *aPtr = nullptr;
    mxArray *A = nullptr;
};