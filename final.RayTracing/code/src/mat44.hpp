#ifndef MAT44_HPP_
#define MAT44_HPP_

#include "vec.hpp"
#include <cmath>
#include <iostream>

void print_mat(const char* msg, double matrix[16]) {
    std::cout << msg << std::endl;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << matrix[i*4+j] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

struct Mat44
{
    double matrix[16];

    Mat44(double fill=0.) { for (int i = 0; i < 16; i++) matrix[i] = fill; }
    Mat44(double mat[]) { memcpy(matrix, mat, sizeof(matrix)); }
    Mat44(const Vec3& v0, const Vec3& v1, const Vec3& v2): Mat44() {
        matrix[0] = v0.x, matrix[1] = v1.x, matrix[2] = v2.x;
        matrix[4] = v0.y, matrix[5] = v1.y, matrix[6] = v2.y;
        matrix[8] = v0.z, matrix[9] = v1.z, matrix[10] = v2.z;
        matrix[15] = 1.;
    }
    Mat44(const Mat44& m) { memcpy(matrix, m.matrix, sizeof(matrix)); }

    double& operator[](int k) { return matrix[k]; }
    double operator[](int k) const { return matrix[k]; }

    static Mat44 identity() {
        double mat[] = {
            1., 0., 0., 0.,
            0., 1., 0., 0.,
            0., 0., 1., 0.,
            0., 0., 0., 1.};
        return Mat44(mat);
    }
    static Mat44 scaling(double sx, double sy, double sz) {
        double mat[] = {
            sx, 0., 0., 0.,
            0., sy, 0., 0.,
            0., 0., sz, 0.,
            0., 0., 0., 1.};
        return Mat44(mat);
    }
    static Mat44 translation(double x, double y, double z) {
        double mat[] = {
            1., 0., 0., x,
            0., 1., 0., y,
            0., 0., 1., z,
            0., 0., 0., 1.};
        return Mat44(mat);
    }
    static Mat44 rot_x(double theta) {
        double sin_theta = sin(theta);
        double cos_theta = cos(theta);
        double mat[] = {
            1., 0., 0., 0.,
            0., cos_theta, -sin_theta, 0.,
            0., sin_theta, cos_theta, 0.,
            0., 0., 0., 1.};
        return Mat44(mat);
    }
    static Mat44 rot_y(double theta) {
        double sin_theta = sin(theta);
        double cos_theta = cos(theta);
        double mat[] = {
            cos_theta, 0., sin_theta, 0.,
            0., 1., 0., 0.,
            -sin_theta, 0., cos_theta, 0.,
            0., 0., 0., 1.};
        return Mat44(mat);
    }
    static Mat44 rot_z(double theta) {
        double sin_theta = sin(theta);
        double cos_theta = cos(theta);
        double mat[] = {
            cos_theta, -sin_theta, 0., 0.,
            sin_theta, cos_theta, 0., 0.,
            0., 0., 1., 0.,
            0., 0., 0., 1.};
        return Mat44(mat);
    }

    Mat44 mult(const Mat44& b) const {
        auto res = Mat44();
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                for (int k = 0; k < 4; k++) {
                    res[row*4+col] += matrix[row*4 + k] * b[k*4 + col];
                }
            }
        }
        return res;
    }

    Mat44 operator*(const Mat44& b) const { return mult(b); }

    Vec3 mult(const Vec3& v, bool translate=false) const {
        Vec3 res;
        res.x = matrix[0]*v.x + matrix[1]*v.y + matrix[2]*v.z;
        res.y = matrix[4]*v.x + matrix[5]*v.y + matrix[6]*v.z;
        res.z = matrix[8]*v.x + matrix[9]*v.y + matrix[10]*v.z;
        if (translate) {
            res.x += matrix[3];
            res.y += matrix[7];
            res.z += matrix[11];
        }
        return res;
    }

    Mat44 inversed() const {
        Mat44 mat_copy(*this);
        auto res = Mat44::identity();
        // print_mat("start:", mat_copy.matrix);

        for (int i = 0; i < 4; i++) {
            int non_zero_row;
            for (non_zero_row = i; non_zero_row < 4; non_zero_row++) {
                if (mat_copy[non_zero_row*4 + i] != 0)
                    break;
            }
            if (non_zero_row == 4) { // not invertible
                return Mat44(); // return 0
            }
            // gaussian elimination
            res.exchange_row(i, non_zero_row);
            mat_copy.exchange_row(i, non_zero_row);
            res.scale_row(i, 1. / mat_copy[i*4 + i]);
            mat_copy.scale_row(i, 1. / mat_copy[i*4 + i]);
            for (int j = i+1; j < 4; j++) {
                res.add_to_row(i, -mat_copy[j*4+i], j);
                mat_copy.add_to_row(i, -mat_copy[j*4+i], j);
            }
            // print_mat("res:", res.matrix);
            // print_mat("mat_copy:", mat_copy.matrix);
        }
        for (int i = 1; i < 4; i++) {
            for (int j = i-1; j >= 0; j--) {
                res.add_to_row(i, -mat_copy[j*4+i], j);
                mat_copy.add_to_row(i, -mat_copy[j*4+i], j);
            }
            // print_mat("res:", res.matrix);
            // print_mat("mat_copy:", mat_copy.matrix);
        }

        return res;
    }

    Mat44 transposed() const {
        Mat44 res;
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                res[col*4 + row] = matrix[row*4 + col];
            }
        }
        return res;
    }

    void exchange_row(int row1, int row2) {
        for (int k = 0; k < 4; k++) {
            double tmp = matrix[row1*4 + k];
            matrix[row1*4 + k] = matrix[row2*4 + k];
            matrix[row2*4 + k] = tmp;
        }
    }

    void scale_row(int row, double factor) {
        for (int k = 0; k < 4; k++)
            matrix[row*4 + k] *= factor;
    }

    void add_to_row(int row_from, double factor, int row_to) {
        for (int k = 0; k < 4; k++)
            matrix[row_to*4 + k] += matrix[row_from*4 + k] * factor;
    }

};

inline std::ostream &operator<<(std::ostream &os, const Mat44& m) {
    os << "Mat: \n";
    for (int row = 0; row < 4; row++) {
        for (int c = 0; c < 4; c++) {
            os << m[row*4 + c] << " ";
        }
        os << "\n";
    }
    return os;
}

#endif