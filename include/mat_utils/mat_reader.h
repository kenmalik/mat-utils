#pragma once

#include <string>
#include <vector>

namespace mat_utils {

// We use the PIMPL idiom to avoid having to include MATLAB headers into
// application code
struct MatReaderImpl;

class MatReader {
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

    void close();

    MatReader(const std::string &mat_file_name,
              const std::vector<std::string> &arr, const std::string &field);
    ~MatReader();

  private:
    MatReaderImpl *impl;
};

} // namespace mat_utils