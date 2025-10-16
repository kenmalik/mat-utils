#include <algorithm>
#include <filesystem>
#include <vector>

#include <gtest/gtest.h>

#include "mat_utils/mat_reader.h"

TEST(MatReaderTest, ReadDense) {
  std::filesystem::path data = TEST_DATA_DIR;
  mat_utils::MatReader reader{data / "5x5_dense_incrementing.mat", {}, "A"};

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
