#include "mat_utils/mat_reader.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <stdexcept>
#include <vector>

const std::filesystem::path data = TEST_DATA_DIR;

TEST(MatReaderTest, ReadDenseDouble) {
    mat_utils::MatReader reader{data / "5x5_dense_incrementing.mat", {}, "A"};
    EXPECT_FALSE(reader.is_sparse());
    EXPECT_TRUE(reader.is_double());

    constexpr int mat_size = 25;
    std::vector<int> expected(mat_size);
    int new_val = 1;
    for (auto &val : expected) {
        val = new_val++;
    }

    std::vector<int> got(reader.values<double>().begin(),
                         reader.values<double>().end());

    EXPECT_EQ(expected, got);
}

TEST(MatReaderTest, ReadDenseSingle) {
    mat_utils::MatReader reader{
        data / "5x5_dense_incrementing_single.mat", {}, "A"};
    EXPECT_FALSE(reader.is_sparse());
    EXPECT_FALSE(reader.is_double());

    constexpr int mat_size = 25;
    std::vector<int> expected(mat_size);
    int new_val = 1;
    for (auto &val : expected) {
        val = new_val++;
    }

    std::vector<int> got(reader.values<float>().begin(),
                         reader.values<float>().end());

    EXPECT_EQ(expected, got);
}

TEST(MatReaderTest, ReadDenseFailsOnSparse) {
    auto construct = [] {
        mat_utils::MatReader reader(data / "5x5_sparse_identity.mat", {}, "A");
    };

    ASSERT_THROW(construct(), std::invalid_argument);
}

TEST(MatReaderTest, ReadSparseDouble) {
    mat_utils::MatReader<mat_utils::Sparsity::Sparse> reader{
        data / "5x5_sparse_identity.mat", {}, "A"};
    EXPECT_TRUE(reader.is_sparse());
    EXPECT_TRUE(reader.is_double());

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

    std::vector<int> got_data(reader.values<double>().begin(),
                              reader.values<double>().end());
    std::vector<int> got_row_indices(reader.row_indices().begin(),
                                     reader.row_indices().end());
    std::vector<int> got_column_pointers(reader.column_pointers().begin(),
                                         reader.column_pointers().end());

    EXPECT_EQ(expected_data, got_data);
    EXPECT_EQ(expected_ir, got_row_indices);
    EXPECT_EQ(expected_jc, got_column_pointers);
}

TEST(MatReaderTest, ReadSparseSingle) {
    mat_utils::MatReader<mat_utils::Sparsity::Sparse> reader{
        data / "5x5_sparse_identity_single.mat", {}, "A"};
    EXPECT_TRUE(reader.is_sparse());
    EXPECT_FALSE(reader.is_double());

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

    std::vector<int> got_data(reader.values<float>().begin(),
                              reader.values<float>().end());
    std::vector<int> got_row_indices(reader.row_indices().begin(),
                                     reader.row_indices().end());
    std::vector<int> got_column_pointers(reader.column_pointers().begin(),
                                         reader.column_pointers().end());

    EXPECT_EQ(expected_data, got_data);
    EXPECT_EQ(expected_ir, got_row_indices);
    EXPECT_EQ(expected_jc, got_column_pointers);
}

TEST(MatReaderTest, ReadSparseFailsOnDense) {
    auto construct = [] {
        mat_utils::MatReader<mat_utils::Sparsity::Sparse> reader(
            data / "5x5_dense_incrementing.mat", {}, "A");
    };

    ASSERT_THROW(construct(), std::invalid_argument);
}

TEST(MatReaderTest, ReadSingleFailsOnDouble) {
    mat_utils::MatReader reader{data / "5x5_dense_incrementing.mat", {}, "A"};

    // NOLINTNEXTLINE
    ASSERT_THROW(auto _ = reader.values<float>(), std::invalid_argument);
}

TEST(MatReaderTest, ReadDoubleFailsOnSingle) {
    mat_utils::MatReader reader(data / "5x5_dense_incrementing_single.mat", {},
                                "A");

    // NOLINTNEXTLINE
    ASSERT_THROW(auto _ = reader.values<double>(), std::invalid_argument);
}
