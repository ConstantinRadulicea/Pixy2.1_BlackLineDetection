#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <opencv2/opencv.hpp>

template <typename T>
class Matrix {
private:
    int rows;
    int cols;
    std::vector<T> _data;

public:
    // Constructor
    Matrix(int rows, int cols) : rows(rows), cols(cols), _data(rows* cols, 0) {}

    // Copy constructor
    Matrix(const Matrix& other) : rows(other.rows), cols(other.cols), _data(other._data) {}

    void* data() {
        return _data.data();
    }

    // Assignment operator
    Matrix& operator=(const Matrix& other) {
        if (this == &other) {
            return *this;
        }
        rows = other.rows;
        cols = other.cols;
        _data = other._data;
        return *this;
    }

    // Access elements
    T& operator()(int row, int col) {
        if (row >= rows || col >= cols || row < 0 || col < 0) {
            throw std::out_of_range("Matrix indices are out of range");
        }
        return _data[row * cols + col];
    }

    T operator()(int row, int col) const {
        if (row >= rows || col >= cols || row < 0 || col < 0) {
            throw std::out_of_range("Matrix indices are out of range");
        }
        return _data[row * cols + col];
    }

    // Matrix addition
    Matrix operator+(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrices dimensions do not match for addition");
        }
        Matrix result(rows, cols);
        for (int i = 0; i < rows * cols; ++i) {
            result._data[i] = _data[i] + other._data[i];
        }
        return result;
    }

    // Matrix subtraction
    Matrix operator-(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrices dimensions do not match for subtraction");
        }
        Matrix result(rows, cols);
        for (int i = 0; i < rows * cols; ++i) {
            result._data[i] = _data[i] - other._data[i];
        }
        return result;
    }

    // Print matrix
    void print() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                std::cout << _data[i * cols + j] << " ";
            }
            std::cout << std::endl;
        }
    }

    // Getters for dimensions
    int getRows() const { return rows; }
    int getCols() const { return cols; }
};

// Function to convert cv::Mat to Matrix class
template <typename T>
Matrix<T> matToMatrix(const cv::Mat& mat) {
    if (mat.empty()) {
        throw std::invalid_argument("Input cv::Mat is empty.");
    }

    // Check if the cv::Mat type matches the Matrix type
    if (mat.depth() != cv::DataType<T>::depth) {
        throw std::invalid_argument("cv::Mat type does not match Matrix<T> type.");
    }

    // Create a Matrix instance with the same dimensions
    Matrix<T> matrix(mat.rows, mat.cols);

    // Populate the Matrix instance with values from cv::Mat
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            matrix(i, j) = mat.at<T>(i, j);
        }
    }

    return matrix;
}


//
//int main() {
//    Matrix<int> mat1(2, 2);
//    Matrix<int> mat2(2, 2);
//
//    mat1(0, 0) = 1;
//    mat1(0, 1) = 2;
//    mat1(1, 0) = 3;
//    mat1(1, 1) = 4;
//
//    mat2(0, 0) = 5;
//    mat2(0, 1) = 6;
//    mat2(1, 0) = 7;
//    mat2(1, 1) = 8;
//
//    Matrix<int> result = mat1 + mat2;
//
//    std::cout << "Matrix 1:" << std::endl;
//    mat1.print();
//
//    std::cout << "Matrix 2:" << std::endl;
//    mat2.print();
//
//    std::cout << "Result of addition:" << std::endl;
//    result.print();
//
//    return 0;
//}
