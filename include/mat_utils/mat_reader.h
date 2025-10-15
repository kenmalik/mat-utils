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

  size_t cols();
  size_t rows();
  size_t data_width();
  size_t size();
  void close();
  double *data();

protected:
  // We use the PIMPL idiom to avoid having to include MATLAB headers into
  // application code
  struct MatReaderImpl;
  std::unique_ptr<MatReaderImpl> impl;
};

class MatDnReader : public MatReader {
public:
  MatDnReader(const std::string &mat_file_name,
              const std::vector<std::string> &arr, const std::string &field);
};

class MatSpReader : public MatReader {
public:
  size_t *jc();
  size_t jc_size();
  size_t *ir();
  size_t ir_size();
  size_t nnz();

  MatSpReader(const std::string &mat_file_name,
              const std::vector<std::string> &arr, const std::string &field);
};

} // namespace mat_utils
