#pragma once

#include <memory>
#include <string>
#include <vector>

namespace mat_utils {

class MatReader {
  public:
    MatReader(const std::string &mat_file_name,
              const std::vector<std::string> &arr, const std::string &field);
    virtual ~MatReader();

    MatReader(MatReader &&rhs) noexcept;
    MatReader &operator=(MatReader &&rhs) noexcept;

    size_t cols() const;
    size_t rows() const;
    size_t data_width() const;
    size_t size() const;
    void close();
    double *data() const;

  protected:
    // We use the PIMPL idiom to avoid having to include MATLAB headers into
    // application code
    struct MatReaderImpl;
    std::unique_ptr<MatReaderImpl> impl;
};

class DnMatReader : public MatReader {
  public:
    DnMatReader(const std::string &mat_file_name,
                const std::vector<std::string> &arr, const std::string &field);
};

class SpMatReader : public MatReader {
  public:
    size_t *jc() const;
    size_t jc_size() const;
    size_t *ir() const;
    size_t ir_size() const;
    size_t nnz() const;

    SpMatReader(const std::string &mat_file_name,
                const std::vector<std::string> &arr, const std::string &field);
};

} // namespace mat_utils
