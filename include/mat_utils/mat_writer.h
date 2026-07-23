#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <mat.h>
#include <matrix.h>

namespace mat_utils {

class MatWriter {
  public:
    MatWriter() = delete;

    MatWriter(const std::string &mat_path)
        : impl(std::make_unique<MatWriterImpl>()) {
        impl->mat_file = matOpen(mat_path.c_str(), "w");
        if (impl->mat_file == NULL) {
            throw std::runtime_error("Error opening mat file");
        }
    }

    ~MatWriter() { close(); }

    void close() {
        if (impl) {
            if (matClose(impl->mat_file) != 0) {
                std::cerr << "Error closing mat file" << std::endl;
                exit(1);
            }
            impl.reset();
        }
    }

    void write_dense(const std::string &name, const std::vector<float> &matrix,
                     size_t rows, size_t cols) {
        mxArray *pArr =
            mxCreateNumericMatrix(rows, cols, mxSINGLE_CLASS, mxREAL);
        if (pArr == NULL) {
            throw std::runtime_error("Error creating float matrix");
        }

        float *data = static_cast<float *>(mxGetData(pArr));
        std::copy(matrix.begin(), matrix.end(), data);

        if (matPutVariable(impl->mat_file, name.c_str(), pArr) != 0) {
            mxDestroyArray(pArr);
            throw std::runtime_error("Error writing matrix '" + name +
                                     "' to file");
        }
    }

    void write_dense(const std::string &name, const std::vector<double> &matrix,
                     size_t rows, size_t cols) {
        mxArray *pArr =
            mxCreateNumericMatrix(rows, cols, mxDOUBLE_CLASS, mxREAL);
        if (pArr == NULL) {
            throw std::runtime_error("Error creating double matrix");
        }

        double *data = static_cast<double *>(mxGetData(pArr));
        std::copy(matrix.begin(), matrix.end(), data);

        if (matPutVariable(impl->mat_file, name.c_str(), pArr) != 0) {
            mxDestroyArray(pArr);
            throw std::runtime_error("Error writing matrix '" + name +
                                     "' to file");
        }
    }

  private:
    struct MatWriterImpl {
        MATFile *mat_file = nullptr;
    };
    std::unique_ptr<MatWriterImpl> impl;
};

} // namespace mat_utils
