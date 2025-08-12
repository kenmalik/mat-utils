#pragma once

#include <string>

// We use the PIMPL idiom to avoid having to include MATLAB headers into application code
struct SuiteSparseMatrixImpl;

class SuiteSparseMatrix
{
public:
    size_t *jc();
    size_t jc_size();
    size_t *ir();
    size_t ir_size();
    double *data();
    size_t size();

    size_t cols();
    size_t rows();
    size_t nnz();
    size_t data_width();

    SuiteSparseMatrix(const std::string &mat_file_name, const std::string &arr = "Problem", const std::string &field = "A");
    ~SuiteSparseMatrix();

private:
    SuiteSparseMatrixImpl *impl;
};