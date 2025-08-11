#if WITH_MATLAB

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <iomanip>

#include "mat.h"

std::tuple<std::vector<float>, size_t, size_t> read_suitesparse_mat(std::string filename)
{
    const char *file = filename.c_str();
    MATFile *pmat = nullptr;
    pmat = matOpen(file, "r");
    if (pmat == nullptr)
    {
        throw std::invalid_argument("Cannot find " + std::string(file));
    }

    const char *varname = nullptr;
    mxArray *problem_struct = matGetNextVariable(pmat, &varname);
    if (problem_struct == nullptr)
    {
        matClose(pmat);
        throw std::invalid_argument("Failed to read variable from MAT file.");
    }

    mxArray *A = mxGetField(problem_struct, 0, "A");
    if (A == nullptr)
    {
        matClose(pmat);
        mxDestroyArray(problem_struct);
        throw std::invalid_argument("Field 'A' is not found in struct 'Problem'");
    }

    if (!mxIsSparse(A))
    {
        mxDestroyArray(A);
        matClose(pmat);
        throw std::invalid_argument("Matrix is not sparse");
    }

    mwSize rows = mxGetM(A);
    mwSize cols = mxGetN(A);
    mwIndex *jc = mxGetJc(A); // column pointers (size cols+1)
    mwIndex *ir = mxGetIr(A); // row indices (size nzmax)
    double *pr = mxGetPr(A);  // nonzero values (size nzmax)

    std::vector<float> dense(rows * cols, 0.0);
    for (mwIndex j = 0; j < cols; ++j)
    {
        for (mwIndex k = jc[j]; k < jc[j + 1]; ++k)
        {
            mwIndex i = ir[k];
            dense[i + j * rows] = static_cast<float>(pr[k]);
        }
    }

    mxDestroyArray(problem_struct);
    matClose(pmat);

    return {dense, rows, cols};
}

int main(int argc, char *argv[])
{
    auto [mat, rows, cols] = read_suitesparse_mat(argv[1]);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            std::cout << std::setw(8) << std::setprecision(2) << mat.at(i * rows + j) << " ";
        }
        std::cout << std::endl;
    }
}

#endif
