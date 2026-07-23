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
    for (auto *iter = mat.jc(); *iter < mat.jc_size(); ++iter) {
        std::cout << *iter << ' ';
    }
    std::cout << '\n';

    std::cout << "ir: ";
    for (auto *iter = mat.ir(); *iter < mat.ir_size(); ++iter) {
        std::cout << *iter << ' ';
    }
    std::cout << '\n';

    std::cout << "jc: " << mat.jc() << '\n';
    std::cout << "ir: " << mat.ir() << '\n';
    std::cout << "data: " << mat.data() << '\n';
    std::cout << "cols: " << mat.cols() << '\n';
    std::cout << "rows: " << mat.rows() << '\n';
    std::cout << "data_width: " << mat.data_width() << '\n';
    std::cout << "size: " << mat.size() << '\n';
    std::cout << "nnz: " << mat.nnz() << '\n';

    std::vector<float> A(3 * 3, 10); // NOLINT
    mat_utils::MatWriter w_A("A.mat");
    w_A.write_dense("A", A, 3, 3);

    std::vector<double> B(3 * 3, 10); // NOLINT
    mat_utils::MatWriter w_B("B.mat");
    w_B.write_dense("B", B, 3, 3);
}
