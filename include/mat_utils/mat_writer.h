#pragma once

#include <string>
#include <vector>

// We use the PIMPL idiom to avoid having to include MATLAB headers into
// application code
struct MatWriterImpl;

class MatWriter {
  public:
    MatWriter() = delete;

    MatWriter(const std::string &mat_path);
    ~MatWriter();

    void close();

    void write_matrix(const std::string &name, const std::vector<float> &matrix,
                      size_t rows, size_t cols);

  private:
    MatWriterImpl *impl;
};