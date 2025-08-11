#pragma once

#include <string>

// We use the PIMPL idiom to avoid having to include MATLAB headers into application code
struct SuiteSparseMatrixImpl;

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
    SuiteSparseMatrixImpl *impl;
};