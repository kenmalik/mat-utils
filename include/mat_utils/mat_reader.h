#pragma once

#include <string>
#include <vector>

namespace mat_utils {

// We use the PIMPL idiom to avoid having to include MATLAB headers into
// application code
struct MatReaderImpl;

class MatReader {
public:
  size_t cols();
  size_t rows();
  size_t data_width();
  size_t size();
  void close();
  double *data();

  MatReader(const std::string &mat_file_name,
            const std::vector<std::string> &arr, const std::string &field);
  virtual ~MatReader();

protected:
  MatReaderImpl *impl;
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
