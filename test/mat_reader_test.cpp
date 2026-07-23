#include "mat_utils/mat_reader.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <stdexcept>
#include <vector>

const std::filesystem::path data = TEST_DATA_DIR;

TEST(MatReaderTest, ReadDense) {
    mat_utils::MatReader<double> reader{
        data / "5x5_dense_incrementing.mat", {}, "A"};

    constexpr int mat_size = 25;
    std::vector<int> expected(mat_size);
    int new_val = 1;
    for (auto &val : expected) {
        val = new_val++;
    }

    std::vector<int> got(reader.values().begin(), reader.values().end());

    ASSERT_EQ(expected, got);
}

TEST(MatReaderTest, ReadDenseFailsOnSparse) {
    auto construct = [] {
        mat_utils::MatReader<double> reader(data / "5x5_sparse_identity.mat",
                                            {}, "A");
    };

    ASSERT_THROW(construct(), std::invalid_argument);
}

TEST(MatReaderTest, ReadSparse) {
    mat_utils::MatReader<double, mat_utils::Sparsity::Sparse> reader{
        data / "5x5_sparse_identity.mat", {}, "A"};

    std::vector<int> expected_data(5, 1); // NOLINT
    std::vector<int> expected_ir(5);      // NOLINT

    int new_val = 0;
    for (auto &val : expected_ir) {
        val = new_val++;
    }

    std::vector<int> expected_jc(6); // NOLINT
    new_val = 0;
    for (auto &val : expected_jc) {
        val = new_val++;
    }

    std::vector<int> got_data(reader.values().begin(), reader.values().end());
    std::vector<int> got_row_indices(reader.row_indices().begin(),
                                     reader.row_indices().end());
    std::vector<int> got_column_pointers(reader.column_pointers().begin(),
                                         reader.column_pointers().end());

    ASSERT_EQ(expected_data, got_data);
    ASSERT_EQ(expected_ir, got_row_indices);
    ASSERT_EQ(expected_jc, got_column_pointers);
}

TEST(MatReaderTest, ReadSparseFailsOnDense) {
    auto construct = [] {
        mat_utils::MatReader<double, mat_utils::Sparsity::Sparse> reader(
            data / "5x5_dense_incrementing.mat", {}, "A");
    };

    ASSERT_THROW(construct(), std::invalid_argument);
}

TEST(MatReaderTest, ReadSingleFailsOnDouble) {
    ASSERT_THROW(mat_utils::MatReader<float> reader(
                     data / "5x5_dense_incrementing.mat", {}, "A"),
                 std::invalid_argument);
}

TEST(MatReaderTest, ReadDoubleFailsOnSingle) {
    ASSERT_THROW(mat_utils::MatReader<double> reader(
                     data / "5x5_dense_incrementing_single.mat", {}, "A"),
                 std::invalid_argument);
}
