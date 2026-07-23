#include "mat_utils/mat_reader.h"

#include <gtest/gtest.h>

#include <algorithm>
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

    std::vector<int> got(reader.size());
    std::copy(reader.data(), reader.data() + reader.size(), got.data());

    ASSERT_EQ(expected, got);
}

TEST(MatReaderTest, ReadDenseFailsOnSparse) {
    auto construct = [] {
        mat_utils::MatReader<double> reader(data / "5x5_sparse_identity.mat",
                                            {}, "A");
    };

    ASSERT_THROW(construct(), std::invalid_argument);
}

TEST(MatReaderTest, ReadSparseFailsOnDense) {
    auto construct = [] {
        mat_utils::MatReader<double, mat_utils::Sparsity::Sparse> reader(
            data / "5x5_dense_incrementing.mat", {}, "A");
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

    std::vector<int> got_data(reader.nnz());
    std::copy(reader.data(), reader.data() + reader.nnz(), got_data.data());
    std::vector<int> got_ir(reader.ir_size());
    std::copy(reader.ir(), reader.ir() + reader.ir_size(), got_ir.data());
    std::vector<int> got_jc(reader.jc_size());
    std::copy(reader.jc(), reader.jc() + reader.jc_size(), got_jc.data());

    ASSERT_EQ(expected_data, got_data);
    ASSERT_EQ(expected_ir, got_ir);
    ASSERT_EQ(expected_jc, got_jc);
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
