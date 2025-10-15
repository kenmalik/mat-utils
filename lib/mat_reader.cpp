#include <iostream>
#include <memory>
#include <stack>

#include <mat.h>
#include <matrix.h>

#include "mat_utils/mat_reader.h"

auto mxArray_deleter = [](mxArray *ptr) { mxDestroyArray(ptr); };
using mxArrayPtr = std::unique_ptr<mxArray, decltype(mxArray_deleter)>;

namespace mat_utils {

struct MatReader::MatReaderImpl {
  MATFile *mat_file_ptr = nullptr;
  mxArrayPtr A_ptr{nullptr, mxArray_deleter};
};

MatReader::MatReader(const std::string &mat_file_name,
                     const std::vector<std::string> &structs,
                     const std::string &field)
    : impl(new MatReaderImpl()) {
  using namespace std::string_literals;

  std::stack<mxArrayPtr> open_structs{};

  impl->mat_file_ptr = matOpen(mat_file_name.c_str(), "r");
  if (impl->mat_file_ptr == NULL) {
    throw std::runtime_error("Error opening file "s + mat_file_name);
  }

  if (structs.empty()) {
    mxArray *A = matGetVariable(impl->mat_file_ptr, field.c_str());
    impl->A_ptr.reset(mxDuplicateArray(A));

    if (impl->A_ptr.get() == NULL) {
      throw std::invalid_argument("mxArray not found "s + field);
    }

    mwSize rows = mxGetM(impl->A_ptr.get());
    mwSize cols = mxGetN(impl->A_ptr.get());
    if (rows != cols) {
      throw std::invalid_argument("error reading SPD matrix: m != n");
    }

    return;
  }

  auto structs_iter = structs.begin();
  if (structs_iter != structs.end()) {
    auto arr = *structs_iter;

    mxArrayPtr mat_variable{matGetVariable(impl->mat_file_ptr, arr.c_str()),
                            mxArray_deleter};
    if (mat_variable.get() == NULL) {
      throw std::invalid_argument("mxArray not found "s + arr);
    }

    if (mxGetClassID(mat_variable.get()) != mxSTRUCT_CLASS) {
      throw std::invalid_argument(arr + " is not a structure"s);
    }

    open_structs.push(std::move(mat_variable));
    ++structs_iter;
  }

  for (; structs_iter != structs.end(); ++structs_iter) {
    auto current = open_structs.top().get();
    auto arr = *structs_iter;

    if (mxGetFieldNumber(current, arr.c_str()) == -1) {
      throw std::invalid_argument("field not found: "s + arr);
    }

    constexpr int INDEX = 0;
    mxArrayPtr next_struct{mxGetField(current, INDEX, arr.c_str()),
                           mxArray_deleter};
    if (next_struct.get() == NULL) {
      throw std::invalid_argument("mxArray not found "s + arr);
    }

    if (mxGetClassID(next_struct.get()) != mxSTRUCT_CLASS) {
      throw std::invalid_argument(arr + " is not a structure"s);
    }

    open_structs.push(std::move(next_struct));
  }

  auto last_struct = open_structs.top().get();
  if (mxGetFieldNumber(last_struct, field.c_str()) == -1) {
    throw std::invalid_argument("field not found: "s + field);
  }

  constexpr int INDEX = 0;
  mxArray *A = mxGetField(open_structs.top().get(), INDEX, field.c_str());
  impl->A_ptr.reset(mxDuplicateArray(A));

  mwSize rows = mxGetM(impl->A_ptr.get());
  mwSize cols = mxGetN(impl->A_ptr.get());
  if (rows != cols) {
    throw std::invalid_argument("error reading SPD matrix: m != n");
  }
}

MatReader::~MatReader() { close(); }

void MatReader::close() {
  if (matClose(impl->mat_file_ptr) != 0) {
    std::cerr << "Error closing mat file" << std::endl;
    exit(1);
  }
  delete impl;
}

size_t MatReader::cols() { return mxGetN(impl->A_ptr.get()); }

size_t MatReader::rows() { return mxGetM(impl->A_ptr.get()); }

size_t MatReader::data_width() { return mxGetElementSize(impl->A_ptr.get()); }

size_t MatReader::size() { return mxGetNumberOfElements(impl->A_ptr.get()); }

double *MatReader::data() { return mxGetDoubles(impl->A_ptr.get()); }

// Sparse matrix reader

MatSpReader::MatSpReader(const std::string &mat_file_name,
                         const std::vector<std::string> &arr,
                         const std::string &field)
    : MatReader(mat_file_name, arr, field) {}

size_t *MatSpReader::jc() { return mxGetJc(impl->A_ptr.get()); }

size_t MatSpReader::jc_size() { return cols() + 1; }

size_t *MatSpReader::ir() { return mxGetIr(impl->A_ptr.get()); }

size_t MatSpReader::ir_size() { return nnz(); }

size_t MatSpReader::nnz() { return jc()[cols()]; }

} // namespace mat_utils
