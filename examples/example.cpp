#include "mat_utils/mat_reader.h"
#include "mat_utils/mat_writer.h"
#include <iostream>

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }
    MatReader mat(argv[1], {"Problem"}, "A");

    std::cout << "jc: ";
    for (auto iter = mat.jc(); *iter < mat.jc_size(); iter++) {
        std::cout << *iter << ' ';
    }
    std::cout << std::endl;

    std::cout << "ir: ";
    for (auto iter = mat.ir(); *iter < mat.ir_size(); iter++) {
        std::cout << *iter << ' ';
    }
    std::cout << std::endl;

    std::cout << "jc: " << mat.jc() << std::endl;
    std::cout << "ir: " << mat.ir() << std::endl;
    std::cout << "data: " << mat.data() << std::endl;
    std::cout << "cols: " << mat.cols() << std::endl;
    std::cout << "rows: " << mat.rows() << std::endl;
    std::cout << "data_width: " << mat.data_width() << std::endl;
    std::cout << "size: " << mat.size() << std::endl;
    std::cout << "nnz: " << mat.nnz() << std::endl;

    std::vector<float> A(3 * 3, 10);
    MatWriter w("test_mat.mat");
    w.write_matrix("A", A, 3, 3);
}