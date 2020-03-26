#pragma once

#ifndef _GEOMETRIC_TYPES_
#define _GEOMETRIC_TYPES_

// Vector of length 3
struct Vector3 {
    float data[3];

    static Vector3 zero();
    Vector3 operator-() const;
};

// 3x3 matrix for storing rotation
struct Matrix33 {
    union {
        float data[9];
        float mat[3][3];
    };

    static Matrix33 identity();
    Matrix33 getTransposed() const;
};

// Homogeneous 4x4 matrix for storing camera pose (rotation + translation)
struct Matrix44 {
    union {
        float data[16];
        float mat[4][4];
    };

    Matrix44 getTransposed() const;
    Matrix44 getInvertedRT() const;
    static Matrix44 identity();
};

// Camera pose (rotation and translation)
class Transformation {
private:
    Matrix33 m_rotation;
    Vector3  m_translation;

public:
    Transformation();
    Transformation(const Matrix33& r, const Vector3& t);

    Matrix33& r();
    Vector3& t();

    const Matrix33& r() const;
    const Vector3& t() const;

    Matrix44 getMat44() const;

    Transformation getInverted() const;
};

#endif // !_GEOMETRIC_TYPES_
