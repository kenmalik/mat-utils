#pragma once

#include <mat.h>
#include <matrix.h>

#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace mat_utils {

namespace detail {

inline void destroy_mxArray(mxArray *ptr) { mxDestroyArray(ptr); }
using mxArrayPtr = std::unique_ptr<mxArray, decltype(&destroy_mxArray)>;

inline void close_mat_file(MATFile *ptr) { matClose(ptr); }
using MATFilePtr = std::unique_ptr<MATFile, decltype(&close_mat_file)>;

} // namespace detail

class MatReader {
  public:
    MatReader(const std::string &mat_file_name,
              const std::vector<std::string> &structs,
              const std::string &field) {
        using namespace std::string_literals;

        std::stack<detail::mxArrayPtr> open_structs{};

        mat_file_ptr.reset(matOpen(mat_file_name.c_str(), "r"));
        if (mat_file_ptr == NULL) {
            throw std::runtime_error("Error opening file "s + mat_file_name);
        }

        if (structs.empty()) {
            mxArray *A = matGetVariable(mat_file_ptr.get(), field.c_str());
            A_ptr.reset(mxDuplicateArray(A));

            if (A_ptr.get() == NULL) {
                throw std::invalid_argument("mxArray not found "s + field);
            }

            return;
        }

        auto structs_iter = structs.begin();
        if (structs_iter != structs.end()) {
            auto arr = *structs_iter;

            detail::mxArrayPtr mat_variable{
                matGetVariable(mat_file_ptr.get(), arr.c_str()),
                detail::destroy_mxArray};
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
            detail::mxArrayPtr next_struct{
                mxGetField(current, INDEX, arr.c_str()),
                detail::destroy_mxArray};
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
        A_ptr.reset(mxDuplicateArray(A));
    }

    virtual ~MatReader() = default;

    MatReader(MatReader &&rhs) noexcept = default;
    MatReader &operator=(MatReader &&rhs) noexcept = default;

    size_t cols() const { return mxGetN(A_ptr.get()); }
    size_t rows() const { return mxGetM(A_ptr.get()); }
    size_t data_width() const { return mxGetElementSize(A_ptr.get()); }
    size_t size() const { return mxGetNumberOfElements(A_ptr.get()); }
    void close() {
        A_ptr.reset();
        mat_file_ptr.reset();
    }
    double *data() const { return mxGetDoubles(A_ptr.get()); }

  protected:
    detail::MATFilePtr mat_file_ptr{nullptr, detail::close_mat_file};
    detail::mxArrayPtr A_ptr{nullptr, detail::destroy_mxArray};
};

class DnMatReader : public MatReader {
  public:
    DnMatReader(const std::string &mat_file_name,
                const std::vector<std::string> &arr, const std::string &field)
        : MatReader(mat_file_name, arr, field) {
        if (mxIsSparse(A_ptr.get())) {
            throw std::invalid_argument("matrix is sparse");
        }
    }
};

class SpMatReader : public MatReader {
  public:
    size_t *jc() const { return mxGetJc(A_ptr.get()); }
    size_t jc_size() const { return cols() + 1; }
    size_t *ir() const { return mxGetIr(A_ptr.get()); }
    size_t ir_size() const { return nnz(); }
    size_t nnz() const { return jc()[cols()]; }

    SpMatReader(const std::string &mat_file_name,
                const std::vector<std::string> &arr, const std::string &field)
        : MatReader(mat_file_name, arr, field) {
        if (!mxIsSparse(A_ptr.get())) {
            throw std::invalid_argument("matrix is not sparse");
        }
    }
};

} // namespace mat_utils
