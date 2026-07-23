#pragma once

#include "handles.h"

#include <mat.h>
#include <matrix.h>

#include <algorithm>
#include <format>
#include <stdexcept>
#include <string>
#include <vector>

namespace mat_utils {

class [[nodiscard]] MatWriter {
  public:
    explicit MatWriter(const std::string &filename)
        : mat_file{matOpen(filename.c_str(), "w"), handles::close_mat_file} {
        if (mat_file == nullptr) {
            throw std::runtime_error(
                std::format("Error opening file '{}'", filename));
        }
    }

    MatWriter(const MatWriter &) = delete;
    MatWriter &operator=(const MatWriter &) = delete;

    MatWriter(MatWriter &&) = default;
    MatWriter &operator=(MatWriter &&) = default;

    ~MatWriter() = default;

    void close() { mat_file.reset(); }

    void write_dense(const std::string &name, const std::vector<float> &matrix,
                     size_t rows, size_t cols) {
        mxArray *pArr =
            mxCreateNumericMatrix(rows, cols, mxSINGLE_CLASS, mxREAL);
        if (pArr == nullptr) {
            throw std::runtime_error("Error creating float matrix");
        }

        auto *data = static_cast<float *>(mxGetData(pArr));
        std::ranges::copy(matrix, data);

        if (matPutVariable(mat_file.get(), name.c_str(), pArr) != 0) {
            mxDestroyArray(pArr);
            throw std::runtime_error(
                std::format("Error writing matrix '{}' to file", name));
        }
    }

    void write_dense(const std::string &name, const std::vector<double> &matrix,
                     size_t rows, size_t cols) {
        handles::mxArrayPtr pArr{
            mxCreateNumericMatrix(rows, cols, mxDOUBLE_CLASS, mxREAL),
            handles::destroy_mxArray};
        if (pArr == nullptr) {
            throw std::runtime_error("Error creating double matrix");
        }

        auto *data = static_cast<double *>(mxGetData(pArr.get()));
        std::ranges::copy(matrix, data);

        if (matPutVariable(mat_file.get(), name.c_str(), pArr.get()) != 0) {
            throw std::runtime_error(
                std::format("Error writing matrix '{}' to file", name));
        }
    }

  private:
    handles::MATFilePtr mat_file;
};

} // namespace mat_utils
