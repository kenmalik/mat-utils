#pragma once

#include "handles.h"
#include "supported_type.h"

#include <cstdint>
#include <mat.h>
#include <matrix.h>

#include <cstddef>
#include <format>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace mat_utils {
enum class Sparsity : std::uint8_t { Dense, Sparse };

template <SupportedType T, Sparsity S = Sparsity::Dense>
class [[nodiscard]] MatReader {
  public:
    MatReader(const std::string &mat_file_name,
              const std::vector<std::string> &structs, const std::string &field)
        : mat_file{matOpen(mat_file_name.c_str(), "r"),
                   handles::close_mat_file} {
        using namespace std::string_literals;

        if (mat_file == nullptr) {
            throw std::runtime_error("Error opening file "s + mat_file_name);
        }

        if (structs.empty()) {
            mxArray *arr = matGetVariable(mat_file.get(), field.c_str());
            A_ptr.reset(mxDuplicateArray(arr));

            if (A_ptr.get() == nullptr) {
                throw std::invalid_argument("mxArray not found "s + field);
            }
        } else {
            handles::mxArrayPtr last_struct = open_last_struct(structs);

            if (mxGetFieldNumber(last_struct.get(), field.c_str()) == -1) {
                throw std::invalid_argument("field not found: "s + field);
            }

            constexpr int INDEX = 0;
            mxArray *arr = mxGetField(last_struct.get(), INDEX, field.c_str());
            A_ptr.reset(mxDuplicateArray(arr));
        }

        validate_precision(field);
        validate_sparsity(field);
    }

    MatReader(const MatReader &) = delete;
    MatReader &operator=(const MatReader &) = delete;

    MatReader(MatReader &&rhs) noexcept = default;
    MatReader &operator=(MatReader &&rhs) noexcept = default;

    ~MatReader() = default;

    void close() {
        A_ptr.reset();
        mat_file.reset();
    }

    [[nodiscard]] std::size_t cols() const { return mxGetN(A_ptr.get()); }

    [[nodiscard]] std::size_t rows() const { return mxGetM(A_ptr.get()); }

    [[nodiscard]] std::size_t data_width() const {
        return mxGetElementSize(A_ptr.get());
    }

    [[nodiscard]] std::size_t size() const {
        return mxGetNumberOfElements(A_ptr.get());
    }

    [[nodiscard]] T *data() const { return mxGetDoubles(A_ptr.get()); }

    [[nodiscard]] bool is_sparse() const { return mxIsSparse(A_ptr.get()); }

    // Sparse methods

    [[nodiscard]] std::size_t *jc() const
        requires(S == Sparsity::Sparse)
    {
        return mxGetJc(A_ptr.get());
    }

    [[nodiscard]] std::size_t jc_size() const
        requires(S == Sparsity::Sparse)
    {
        return cols() + 1;
    }

    [[nodiscard]] std::size_t *ir() const
        requires(S == Sparsity::Sparse)
    {
        return mxGetIr(A_ptr.get());
    }

    [[nodiscard]] std::size_t ir_size() const
        requires(S == Sparsity::Sparse)
    {
        return nnz();
    }

    [[nodiscard]] std::size_t nnz() const
        requires(S == Sparsity::Sparse)
    {
        return jc()[cols()];
    }

  private:
    handles::MATFilePtr mat_file;
    handles::mxArrayPtr A_ptr{nullptr, handles::destroy_mxArray};

    handles::mxArrayPtr
    open_last_struct(const std::vector<std::string> &structs) {
        using namespace std::string_literals;

        std::stack<handles::mxArrayPtr> open_structs{};

        auto structs_iter = structs.cbegin();
        if (structs_iter != structs.cend()) {
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

        return std::move(open_structs.top());
    }

    void validate_precision(const std::string &field) {
        if constexpr (std::is_same_v<T, double>) {
            if (!mxIsDouble(A_ptr.get())) {
                throw std::invalid_argument{
                    std::format("field '{}' is not double-precision", field)};
            }
        } else {
            if (!mxIsSingle(A_ptr.get())) {
                throw std::invalid_argument{
                    std::format("field '{}' is not single-precision", field)};
            }
        }
    }

    void validate_sparsity(const std::string &field) {
        if constexpr (S == Sparsity::Sparse) {
            if (!mxIsSparse(A_ptr.get())) {
                throw std::invalid_argument{
                    std::format("field '{}' is not sparse", field)};
            }
        } else {
            if (mxIsSparse(A_ptr.get())) {
                throw std::invalid_argument{
                    std::format("field '{}' is not dense", field)};
            }
        }
    }
};

} // namespace mat_utils
