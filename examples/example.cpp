#include "mat_utils/mat_reader.h"
#include "mat_utils/mat_writer.h"
#include <iostream>

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        std::cerr << "Invalid arguments\n";
        return 1;
    }
    mat_utils::MatReader<double, mat_utils::Sparsity::Sparse> mat(
        argv[1], {"Problem"}, "A"); // NOLINT

    std::cout << "jc: ";
    for (const auto val : mat.column_pointers()) {
        std::cout << val << ' ';
    }
    std::cout << '\n';

    std::cout << "ir: ";
    for (const auto val : mat.row_indices()) {
        std::cout << val << ' ';
    }
    std::cout << '\n';

    std::cout << "jc: " << mat.column_pointers().data() << '\n';
    std::cout << "ir: " << mat.row_indices().data() << '\n';
    std::cout << "data: " << mat.values().data() << '\n';

    std::cout << "cols: " << mat.cols() << '\n';
    std::cout << "rows: " << mat.rows() << '\n';

    std::cout << "data_width: " << mat.data_width() << '\n';
    std::cout << "size: " << mat.size() << '\n';
    std::cout << "nnz: " << mat.nonzero_count() << '\n';

    std::vector<float> A(3 * 3, 10); // NOLINT
    mat_utils::MatWriter w_A("A.mat");
    w_A.write_dense("A", A, 3, 3);

    std::vector<double> B(3 * 3, 10); // NOLINT
    mat_utils::MatWriter w_B("B.mat");
    w_B.write_dense("B", B, 3, 3);
}
