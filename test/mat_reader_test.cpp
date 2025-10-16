#include <algorithm>
#include <filesystem>
#include <vector>

#include <gtest/gtest.h>

#include "mat_utils/mat_reader.h"

std::filesystem::path data = TEST_DATA_DIR;

TEST(MatReaderTest, ReadDense) {
    mat_utils::DnMatReader reader{data / "5x5_dense_incrementing.mat", {}, "A"};

    constexpr int mat_size = 25;
    std::vector<int> expected(mat_size);
    int i = 1;
    for (auto &x : expected) {
        x = i++;
    }

    std::vector<int> got(reader.size());
    std::copy(reader.data(), reader.data() + reader.size(), got.data());

    ASSERT_EQ(expected, got);
}

TEST(MatReaderTest, ReadSparse) {
    mat_utils::SpMatReader reader{data / "5x5_sparse_identity.mat", {}, "A"};

    std::vector<int> expected_data(5, 1);
    std::vector<int> expected_ir(5);
    int i = 0;
    for (auto &x : expected_ir) {
        x = i++;
    }
    std::vector<int> expected_jc(6);
    i = 0;
    for (auto &x : expected_jc) {
        x = i++;
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
