#include <algorithm>
#include <iostream>

#include <mat.h>
#include <matrix.h>

#include "mat_utils/mat_writer.h"

namespace mat_utils {

struct MatWriterImpl {
    MATFile *mat_file = nullptr;
};

MatWriter::MatWriter(const std::string &mat_path) : impl(new MatWriterImpl) {
    impl->mat_file = matOpen(mat_path.c_str(), "w");
    if (impl->mat_file == NULL) {
        throw std::runtime_error("Error opening mat file");
    }
}

MatWriter::~MatWriter() { close(); }

void MatWriter::close() {
    if (impl) {
        if (matClose(impl->mat_file) != 0) {
            std::cerr << "Error closing mat file" << std::endl;
            exit(1);
        }
        delete impl;
    }
}

void MatWriter::write_dense(const std::string &name,
                            const std::vector<float> &matrix, size_t rows,
                            size_t cols) {
    mxArray *pArr = mxCreateNumericMatrix(rows, cols, mxSINGLE_CLASS, mxREAL);
    if (pArr == NULL) {
        throw std::runtime_error("Error creating float matrix");
    }

    float *data = static_cast<float *>(mxGetData(pArr));
    std::copy(matrix.begin(), matrix.end(), data);

    if (matPutVariable(impl->mat_file, name.c_str(), pArr) != 0) {
        mxDestroyArray(pArr);
        throw std::runtime_error("Error writing matrix '" + name + "' to file");
    }
}

} // namespace mat_utils
