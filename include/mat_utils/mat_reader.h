#pragma once

#include "handles.h"
#include "supported_type.h"

#include <cstdint>
#include <mat.h>
#include <matrix.h>

#include <cstddef>
#include <format>
#include <span>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace mat_utils {
enum class Sparsity : std::uint8_t { Dense, Sparse };

template <Sparsity S = Sparsity::Dense> class [[nodiscard]] MatReader {
  public:
    MatReader(const std::string &mat_file_name,
              const std::vector<std::string> &structs, const std::string &field)
        : mat_file{matOpen(mat_file_name.c_str(), "r"),
                   handles::close_mat_file} {
        if (mat_file == nullptr) {
            throw std::runtime_error(
                std::format("Error opening file '{}'", mat_file_name));
        }

        if (structs.empty()) {
            mxArray *arr = matGetVariable(mat_file.get(), field.c_str());
            A_ptr.reset(mxDuplicateArray(arr));

            if (A_ptr.get() == nullptr) {
                throw std::invalid_argument(
                    std::format("mxArray not found '{}'", field));
            }
        } else {
            handles::mxArrayPtr last_struct = open_last_struct(structs);

            if (mxGetFieldNumber(last_struct.get(), field.c_str()) == -1) {
                throw std::invalid_argument(
                    std::format("field not found: '{}'", field));
            }

            constexpr int INDEX = 0;
            mxArray *arr = mxGetField(last_struct.get(), INDEX, field.c_str());
            A_ptr.reset(mxDuplicateArray(arr));
        }

        validate_sparsity(field);
    }

    MatReader(const MatReader &) = delete;
    MatReader &operator=(const MatReader &) = delete;

    MatReader(MatReader &&) noexcept = default;
    MatReader &operator=(MatReader &&) noexcept = default;

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

    template <SupportedType T> [[nodiscard]] std::span<T> values() const {
        std::size_t length = 0;

        if constexpr (S == Sparsity::Sparse) {
            length = nonzero_count();
        } else {
            length = size();
        }

        if constexpr (std::is_same_v<T, double>) {
            if (!mxIsDouble(A_ptr.get())) {
                throw std::invalid_argument{"values are not double-precision"};
            }
            return {mxGetDoubles(A_ptr.get()), length};
        } else {
            if (!mxIsSingle(A_ptr.get())) {
                throw std::invalid_argument{"values are not single-precision"};
            }
            return {mxGetSingles(A_ptr.get()), length};
        }
    }

    [[nodiscard]] bool is_sparse() const { return mxIsSparse(A_ptr.get()); }

    // Sparse methods

    [[nodiscard]] std::span<std::size_t> column_pointers() const {
        return {mxGetJc(A_ptr.get()), column_pointer_count()};
    }

    [[nodiscard]] std::span<std::size_t> row_indices() const
        requires(S == Sparsity::Sparse)
    {
        return {mxGetIr(A_ptr.get()), row_index_count()};
    }

    [[nodiscard]] std::size_t nonzero_count() const
        requires(S == Sparsity::Sparse)
    {
        return column_pointers()[cols()];
    }

  private:
    handles::MATFilePtr mat_file;
    handles::mxArrayPtr A_ptr{nullptr, handles::destroy_mxArray};

    handles::mxArrayPtr
    open_last_struct(const std::vector<std::string> &structs) {
        std::stack<handles::mxArrayPtr> open_structs{};

        auto structs_iter = structs.cbegin();
        if (structs_iter != structs.cend()) {
            auto arr = *structs_iter;

            handles::mxArrayPtr mat_variable{
                matGetVariable(mat_file.get(), arr.c_str()),
                handles::destroy_mxArray};
            if (mat_variable.get() == nullptr) {
                throw std::invalid_argument(
                    std::format("mxArray not found '{}'", arr));
            }

            if (mxGetClassID(mat_variable.get()) != mxSTRUCT_CLASS) {
                throw std::invalid_argument(
                    std::format("'{}' is not a structure", arr));
            }

            open_structs.push(std::move(mat_variable));
            ++structs_iter;
        }

        for (; structs_iter != structs.end(); ++structs_iter) {
            auto *current = open_structs.top().get();
            const auto &arr = *structs_iter;

            if (mxGetFieldNumber(current, arr.c_str()) == -1) {
                throw std::invalid_argument(
                    std::format("field not found: '{}'", arr));
            }

            constexpr int INDEX = 0;
            handles::mxArrayPtr next_struct{
                mxGetField(current, INDEX, arr.c_str()),
                handles::destroy_mxArray};
            if (next_struct.get() == nullptr) {
                throw std::invalid_argument(
                    std::format("mxArray not found '{}'", arr));
            }

            if (mxGetClassID(next_struct.get()) != mxSTRUCT_CLASS) {
                throw std::invalid_argument(
                    std::format("'{}' is not a structure", arr));
            }

            open_structs.push(std::move(next_struct));
        }

        return std::move(open_structs.top());
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

    [[nodiscard]] std::size_t column_pointer_count() const
        requires(S == Sparsity::Sparse)
    {
        return cols() + 1;
    }

    [[nodiscard]] std::size_t row_index_count() const
        requires(S == Sparsity::Sparse)
    {
        return nonzero_count();
    }
};

} // namespace mat_utils
