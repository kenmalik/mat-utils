#pragma once

#include <mat.h>
#include <matrix.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace mat_utils {

class MatWriter {
  public:
    MatWriter() = delete;

    MatWriter(const std::string &mat_path)
        : mat_file{matOpen(mat_path.c_str(), "w")} {

        if (mat_file == nullptr) {
            throw std::runtime_error("Error opening mat file");
        }
    }

    MatWriter(const MatWriter &) = default;
    MatWriter(MatWriter &&) = delete;
    MatWriter &operator=(const MatWriter &) = default;
    MatWriter &operator=(MatWriter &&) = delete;

    ~MatWriter() { close(); }

    void close() {
        if (mat_file != nullptr) {
            if (matClose(mat_file) != 0) {
                std::cerr << "Error closing mat file\n";
                exit(1);
            }
            mat_file = nullptr;
        }
    }

    void write_dense(const std::string &name, const std::vector<float> &matrix,
                     size_t rows, size_t cols) {
        mxArray *pArr =
            mxCreateNumericMatrix(rows, cols, mxSINGLE_CLASS, mxREAL);
        if (pArr == nullptr) {
            throw std::runtime_error("Error creating float matrix");
        }

        auto *data = static_cast<float *>(mxGetData(pArr));
        std::copy(matrix.begin(), matrix.end(), data);

        if (matPutVariable(mat_file, name.c_str(), pArr) != 0) {
            mxDestroyArray(pArr);
            throw std::runtime_error("Error writing matrix '" + name +
                                     "' to file");
        }
    }

    void write_dense(const std::string &name, const std::vector<double> &matrix,
                     size_t rows, size_t cols) {
        mxArray *pArr =
            mxCreateNumericMatrix(rows, cols, mxDOUBLE_CLASS, mxREAL);
        if (pArr == nullptr) {
            throw std::runtime_error("Error creating double matrix");
        }

        auto *data = static_cast<double *>(mxGetData(pArr));
        std::copy(matrix.begin(), matrix.end(), data);

        if (matPutVariable(mat_file, name.c_str(), pArr) != 0) {
            mxDestroyArray(pArr);
            throw std::runtime_error("Error writing matrix '" + name +
                                     "' to file");
        }
    }

  private:
    MATFile *mat_file = nullptr;
};

} // namespace mat_utils
