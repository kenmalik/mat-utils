#pragma once

#include "handles.h"

#include <mat.h>
#include <matrix.h>

#include <cstddef>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace mat_utils {

class [[nodiscard]] MatReader {
  public:
    MatReader(const std::string &mat_file_name,
              const std::vector<std::string> &structs, const std::string &field)
        : mat_file{matOpen(mat_file_name.c_str(), "r"),
                   handles::close_mat_file} {
        using namespace std::string_literals;

        std::stack<handles::mxArrayPtr> open_structs{};

        if (mat_file == nullptr) {
            throw std::runtime_error("Error opening file "s + mat_file_name);
        }

        if (structs.empty()) {
            mxArray *arr = matGetVariable(mat_file.get(), field.c_str());
            A_ptr.reset(mxDuplicateArray(arr));

            if (A_ptr.get() == nullptr) {
                throw std::invalid_argument("mxArray not found "s + field);
            }

            return;
        }

        auto structs_iter = structs.begin();
        if (structs_iter != structs.end()) {
            auto arr = *structs_iter;

            handles::mxArrayPtr mat_variable{
                matGetVariable(mat_file.get(), arr.c_str()),
                handles::destroy_mxArray};
            if (mat_variable.get() == nullptr) {
                throw std::invalid_argument("mxArray not found "s + arr);
            }

            if (mxGetClassID(mat_variable.get()) != mxSTRUCT_CLASS) {
                throw std::invalid_argument(arr + " is not a structure"s);
            }

            open_structs.push(std::move(mat_variable));
            ++structs_iter;
        }

        for (; structs_iter != structs.end(); ++structs_iter) {
            auto *current = open_structs.top().get();
            const auto &arr = *structs_iter;

            if (mxGetFieldNumber(current, arr.c_str()) == -1) {
                throw std::invalid_argument("field not found: "s + arr);
            }

            constexpr int INDEX = 0;
            handles::mxArrayPtr next_struct{
                mxGetField(current, INDEX, arr.c_str()),
                handles::destroy_mxArray};
            if (next_struct.get() == nullptr) {
                throw std::invalid_argument("mxArray not found "s + arr);
            }

            if (mxGetClassID(next_struct.get()) != mxSTRUCT_CLASS) {
                throw std::invalid_argument(arr + " is not a structure"s);
            }

            open_structs.push(std::move(next_struct));
        }

        auto *last_struct = open_structs.top().get();
        if (mxGetFieldNumber(last_struct, field.c_str()) == -1) {
            throw std::invalid_argument("field not found: "s + field);
        }

        constexpr int INDEX = 0;
        mxArray *arr =
            mxGetField(open_structs.top().get(), INDEX, field.c_str());
        A_ptr.reset(mxDuplicateArray(arr));
    }

    MatReader(const MatReader &) = delete;
    MatReader &operator=(const MatReader &) = delete;

    MatReader(MatReader &&rhs) noexcept = default;
    MatReader &operator=(MatReader &&rhs) noexcept = default;

    virtual ~MatReader() = default;

    [[nodiscard]] std::size_t cols() const { return mxGetN(A_ptr.get()); }
    [[nodiscard]] std::size_t rows() const { return mxGetM(A_ptr.get()); }
    [[nodiscard]] std::size_t data_width() const {
        return mxGetElementSize(A_ptr.get());
    }
    [[nodiscard]] std::size_t size() const {
        return mxGetNumberOfElements(A_ptr.get());
    }
    [[nodiscard]] double *data() const { return mxGetDoubles(A_ptr.get()); }

    [[nodiscard]] bool is_sparse() const { return mxIsSparse(A_ptr.get()); }

    void close() {
        A_ptr.reset();
        mat_file.reset();
    }

  private:
    handles::MATFilePtr mat_file;

  protected:
    handles::mxArrayPtr A_ptr{nullptr, handles::destroy_mxArray}; // NOLINT
};

class DnMatReader : public MatReader {
  public:
    DnMatReader(const std::string &mat_file_name,
                const std::vector<std::string> &arr, const std::string &field)
        : MatReader(mat_file_name, arr, field) {
        if (this->is_sparse()) {
            throw std::invalid_argument("matrix is not dense");
        }
    }
};

class SpMatReader : public MatReader {
  public:
    SpMatReader(const std::string &mat_file_name,
                const std::vector<std::string> &arr, const std::string &field)
        : MatReader(mat_file_name, arr, field) {
        if (!this->is_sparse()) {
            throw std::invalid_argument("matrix is not sparse");
        }
    }

    [[nodiscard]] std::size_t *jc() const { return mxGetJc(A_ptr.get()); }
    [[nodiscard]] std::size_t jc_size() const { return cols() + 1; }
    [[nodiscard]] std::size_t *ir() const { return mxGetIr(A_ptr.get()); }
    [[nodiscard]] std::size_t ir_size() const { return nnz(); }
    [[nodiscard]] std::size_t nnz() const { return jc()[cols()]; } // NOLINT
};

} // namespace mat_utils
