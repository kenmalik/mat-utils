#include "mat.h"
#include "matrix.h"

#include <vector>
#include <iostream>

class SuiteSparseMatrix
{
public:
    size_t *jc()
    {
        return mxGetJc(A); // Column pointers (size cols+1)
    }

    size_t *ir()
    {
        return mxGetIr(A); // Row indices (size nnz)
    }

    double *data()
    {
        return mxGetPr(A);
    }

    size_t cols()
    {
        return mxGetN(A);
    }

    size_t rows()
    {
        return mxGetM(A);
    }

    size_t nnz()
    {
        return jc()[cols()];
    }

    size_t data_width()
    {
        return mxGetElementSize(A);
    }

    size_t size()
    {
        return mxGetNumberOfElements(A);
    }

    SuiteSparseMatrix(const char *mat_file_name, const char *arr, const char *field)
    {

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
    }

    ~SuiteSparseMatrix()
    {
        mxDestroyArray(aPtr);
        mxDestroyArray(A);
        if (matClose(mfPtr) != 0)
        {
            std::cerr << "Error closing mat file" << std::endl;
            exit(1);
        }
    }

private:
    MATFile *mfPtr = nullptr;
    mxArray *aPtr = nullptr;
    mxArray *A = nullptr;
};

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }
    SuiteSparseMatrix mat(argv[1], "Problem", "A");

    std::cout << "jc: " << mat.jc() << std::endl;
    std::cout << "ir: " << mat.ir() << std::endl;
    std::cout << "data: " << mat.data() << std::endl;
    std::cout << "cols: " << mat.cols() << std::endl;
    std::cout << "rows: " << mat.rows() << std::endl;
    std::cout << "data_width: " << mat.data_width() << std::endl;
    std::cout << "size: " << mat.size() << std::endl;
    std::cout << "nnz: " << mat.nnz() << std::endl;
}
