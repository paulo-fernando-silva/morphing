/*
 * The MIT License (MIT)
 *
 * Copyright (C) 2004-2013 Paulo Silva <paulo.jnkml@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef VEC_HPP
#define VEC_HPP

#include <cmath>        // sqrt
#include <iostream>     // operator<< operator>> istream.read ostream.write
#include <algorithm>    // min max
#include <cassert>


/**
 * @file vec.hpp
 * Initially it was just another implementation of vector algebra for c++
 * but eventually I decided to make it as close as I "could" to glsl.
 * @version 0.8
 * Next versions should have complete testing and documentation.
 */
namespace cgl {


/* *****************************************************************************
 * Comparison operator
 **************************************************************************** */
template<typename T>
struct CompOp {
    inline bool operator()(const T& a, const T& b) const {
		return a == b;
	}
};


template<>
struct CompOp<float> {
    inline bool operator()(const float& a, const float& b) const {
		const float dt(a - b);
		return (1e-3f >= (dt < 0? -dt : dt));
	}
};


template<>
struct CompOp<double> {
    inline bool operator()(const double& a, const double& b) const {
		const double dt(a - b);
		return (1e-6f >= (dt < 0? -dt : dt));
	}
};



/* *****************************************************************************
 * vector types predeclaration
 **************************************************************************** */
template<typename T, typename Cmp = CompOp<T> >
struct Vec2;

template<typename T, typename Cmp = CompOp<T> >
struct Vec3;

template<typename T, typename Cmp = CompOp<T> >
struct Vec4;



/* *****************************************************************************
 * glsl vector types.
 **************************************************************************** */
typedef Vec2<bool> bvec2;
typedef Vec3<bool> bvec3;
typedef Vec4<bool> bvec4;

typedef Vec2<int> ivec2;
typedef Vec3<int> ivec3;
typedef Vec4<int> ivec4;

typedef Vec2<unsigned> uvec2;
typedef Vec3<unsigned> uvec3;
typedef Vec4<unsigned> uvec4;

typedef Vec2<float> vec2;
typedef Vec3<float> vec3;
typedef Vec4<float> vec4;

typedef Vec2<double> dvec2;
typedef Vec3<double> dvec3;
typedef Vec4<double> dvec4;


/* *****************************************************************************
 * Extra very common vector types.
 **************************************************************************** */
typedef Vec2<unsigned char> ubvec2;
typedef Vec3<unsigned char> ubvec3;
typedef Vec4<unsigned char> ubvec4;


/** ****************************************************************************
 * Two dimensional vector
 **************************************************************************** */
template<typename T, typename C>
struct Vec2 {
    typedef T Type;
    typedef C CompOp;

    typedef Vec2<T, C> vec;

    static const unsigned SIZE = 2;

    // Will not compile with -pedantic but it's very convenient
    union {
        struct { T x, y; };
        struct { T r, g; };
        struct { T s, t; };
        T vals[SIZE];
    };


    inline explicit Vec2(const T& t = T(0)):
        x(t),
        y(t)
    {}


    inline explicit Vec2(const T& x, const T& y):
		x(x),
		y(y)
	{}


    template<typename Tp2, typename Cmp2>
    inline explicit Vec2(const Vec2<Tp2, Cmp2>& v):
        x(T(v.x)),
        y(T(v.y))
	{}


    explicit Vec2(const Vec3<T, C>& v);

    explicit Vec2(const Vec4<T, C>& v);


	inline vec& operator+=(const vec& v) {
		x += v.x;
		y += v.y;

		return *this;
	}


	inline vec& operator-=(const vec& v) {
		x -= v.x;
		y -= v.y;

		return *this;
	}


    inline vec& operator*=(const T& t) {
		x *= t;
		y *= t;

		return *this;
	}


	inline vec operator-() const {
		return vec(-x, -y);
	}


    bool operator==(const vec& v) const;


    inline bool operator!=(const vec& v) const {
        return !(*this == v);
    }


    inline vec operator+(const vec& v) const {
        return vec(*this) += v;
    }


    inline vec operator-(const vec& v) const {
        return vec(*this) -= v;
    }


    inline vec operator*(const T& t) const {
        return vec(*this) *= t;
    }


    inline const T& operator[](const unsigned i) const {
        assert(i < SIZE);
        return vals[i];
    }


    inline T& operator[](const unsigned i) {
        assert(i < SIZE);
        return vals[i];
    }
};




/** ****************************************************************************
 * Three dimensional vector
 **************************************************************************** */
template<typename T, typename C>
struct Vec3 {
    typedef T Type;
    typedef C CompOp;

    typedef Vec2<T, C> vec2;
    typedef Vec3<T, C> vec;

    static const unsigned SIZE = 3;

    // Will not compile with -pedantic but it's very convenient
    union {
        struct { T x, y, z; };
        struct { T r, g, b; };
        struct { T s, t, p; };
        T vals[SIZE];
    };


    inline explicit Vec3(const T& t = T(0)):
        x(t),
        y(t),
        z(t)
    {}


    inline explicit Vec3(const T& x, const T& y, const T& z):
		x(x),
		y(y),
		z(z)
	{}


    template<typename Tp2, typename Cmp2>
    inline explicit Vec3(const Vec3<Tp2, Cmp2>& v):
        x(T(v.x)),
        y(T(v.y)),
        z(T(v.z))
	{}


    inline explicit Vec3(const vec2& v, const T& z):
        x(T(v.x)),
        y(T(v.y)),
        z(T(z))
	{}


    inline explicit Vec3(const T& x, const vec2& v):
		x(x),
		y(v.x),
		z(v.y)
	{}


    explicit Vec3(const Vec4<T, C>& v);


	inline vec& operator+=(const vec& v) {
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}


	inline vec& operator-=(const vec& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}


    inline vec& operator*=(const T& t) {
		x *= t;
		y *= t;
		z *= t;

		return *this;
	}


	inline vec operator-() const {
		return vec(-x, -y, -z);
	}


    bool operator==(const vec& v) const;


    inline bool operator!=(const vec& v) const {
        return !(*this == v);
    }


    inline vec operator+(const vec& v) const {
        return vec(*this) += v;
    }


    inline vec operator-(const vec& v) const {
        return vec(*this) -= v;
    }


    inline vec operator*(const T& t) const {
        return vec(*this) *= t;
    }


    inline const T& operator[](const unsigned i) const {
        assert(i < SIZE);
        return vals[i];
    }


    inline T& operator[](const unsigned i) {
        assert(i < SIZE);
        return vals[i];
    }
};



/** ****************************************************************************
 * Four dimensional vector
 **************************************************************************** */
template<typename T, typename C>
struct Vec4 {
    typedef T Type;
    typedef C CompOp;

    typedef Vec2<T, C> vec2;
    typedef Vec3<T, C> vec3;
    typedef Vec4<T, C> vec;

    static const unsigned SIZE = 4;

    // Will not compile with -pedantic but it's very convenient
    union {
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
        struct { T s, t, p, q; };
        T vals[SIZE];
    };


    inline explicit Vec4(const T& t = T(0)):
        x(t),
        y(t),
        z(t),
        w(t)
    {}


    inline explicit Vec4(const T& x, const T& y, const T& z, const T& w):
        x(x),
        y(y),
        z(z),
        w(w)
    {}


    template<typename Tp2, typename Cmp2>
    inline explicit Vec4(const Vec4<Tp2, Cmp2>& v):
        x(T(v.x)),
        y(T(v.y)),
        z(T(v.z)),
        w(T(v.w))
	{}


    inline explicit Vec4(const vec3& v, const T& w):
		x(v.x),
		y(v.y),
		z(v.z),
		w(w)
	{}


    inline explicit Vec4(const T& x, const vec3& v):
		x(x),
		y(v.x),
		z(v.y),
		w(v.z)
	{}


    inline explicit Vec4(const vec2& a, const vec2& b):
		x(a.x),
		y(a.y),
		z(b.x),
		w(b.y)
	{}


    inline explicit Vec4(const vec2& v, const T& z, const T& w):
		x(v.x),
		y(v.y),
		z(z),
		w(w)
	{}


    inline explicit Vec4(const T& x, const vec2& v, const T& w):
		x(x),
		y(v.x),
		z(v.y),
		w(w)
	{}


    inline explicit Vec4(const T& x, const T& y, const vec2& v):
		x(x),
		y(y),
		z(v.x),
		w(v.y)
	{}


	inline vec& operator+=(const vec& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return *this;
	}


	inline vec& operator-=(const vec& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;
	}


    inline vec& operator*=(const T& t) {
		x *= t;
		y *= t;
		z *= t;
		w *= t;

		return *this;
	}


	inline vec operator-() const {
		return vec(-x, -y, -z, -w);
	}


    bool operator==(const vec& v) const;


    inline bool operator!=(const vec& v) const {
        return !(*this == v);
    }


    inline vec operator+(const vec& v) const {
        return vec(*this) += v;
    }


    inline vec operator-(const vec& v) const {
        return vec(*this) -= v;
    }


    inline vec operator*(const T& t) const {
        return vec(*this) *= t;
    }


    inline const T& operator[](const unsigned i) const {
        assert(i < SIZE);
        return vals[i];
    }


    inline T& operator[](const unsigned i) {
        assert(i < SIZE);
        return vals[i];
    }
};



/* *****************************************************************************
 * extra vector contructors
 **************************************************************************** */
template<typename T, typename C>
inline Vec2<T, C>::Vec2(const Vec3<T, C>& v):
	x(v.x),
	y(v.y)
{}


template<typename T, typename C>
inline Vec2<T, C>::Vec2(const Vec4<T, C>& v):
	x(v.x),
	y(v.y)
{}


template<typename T, typename C>
inline Vec3<T, C>::Vec3(const Vec4<T, C>& v):
	x(v.x),
	y(v.y),
	z(v.z)
{}


/* *****************************************************************************
 * vector common mathematical operations
 **************************************************************************** */
template<typename vec>
inline vec operator*(const typename vec::Type& t, const vec& v) {
	return vec(v) *= t;
}


template<typename T, typename C>
inline T dot(const Vec2<T, C>& a, const Vec2<T, C>& b) {
    return  a.x * b.x +
            a.y * b.y;
}


template<typename T, typename C>
inline T dot(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    return  a.x * b.x +
            a.y * b.y +
            a.z * b.z;
}


template<typename T, typename C>
inline T dot(const Vec4<T, C>& a, const Vec4<T, C>& b) {
    return  a.x * b.x +
            a.y * b.y +
            a.z * b.z +
            a.w * b.w;
}


template<typename T, typename C>
inline Vec3<T, C> cross(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    return Vec3<T, C>(a.y * b.z - a.z * b.y,
                      a.z * b.x - a.x * b.z,
                      a.x * b.y - a.y * b.x);
}


template<class vec>
inline typename vec::Type length(const vec& v) {
	return typename vec::Type(std::sqrt(dot(v, v)));
}


template<typename vec>
inline vec normalize(const vec& v) {
    typedef typename vec::Type T;

    const T& len2(dot(v, v));

    if(len2 != T(0))
        return v * (T(1) / std::sqrt(len2));

	return v;
}



/* *****************************************************************************
 * Component wise operators for RGBA colors and others.
 **************************************************************************** */
template<typename T, typename C>
inline Vec2<T, C>& operator*=(Vec2<T, C>& a, const Vec2<T, C>& b) {
    a.x *= b.x;
    a.y *= b.y;

    return a;

}


template<typename T, typename C>
inline Vec3<T, C>& operator*=(Vec3<T, C>& a, const Vec3<T, C>& b) {
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;

    return a;

}


template<typename T, typename C>
inline Vec4<T, C>& operator*=(Vec4<T, C>& a, const Vec4<T, C>& b) {
    a.r *= b.r;
    a.g *= b.g;
    a.b *= b.b;
    a.a *= b.a;

    return a;
}


template<typename T, typename C>
inline Vec2<T, C> operator*(const Vec2<T, C>& a, const Vec2<T, C>& b) {
    Vec2<T, C> c(a);
    return c *= b;
}


template<typename T, typename C>
inline Vec3<T, C> operator*(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    Vec3<T, C> c(a);
    return c *= b;
}


template<typename T, typename C>
inline Vec4<T, C> operator*(const Vec4<T, C>& a, const Vec4<T, C>& b) {
    Vec4<T, C> c(a);
    return c *= b;
}


template<typename T, typename C>
inline Vec2<T, C> inv(const Vec2<T, C>& v) {
    return Vec2<T, C>(T(1) / v.x, T(1) / v.y);
}


template<typename T, typename C>
inline Vec3<T, C> inv(const Vec3<T, C>& v) {
    return Vec3<T, C>(T(1) / v.x, T(1) / v.y, T(1) / v.z);
}


template<typename T, typename C>
inline Vec4<T, C> inv(const Vec4<T, C>& v) {
    return Vec4<T, C>(T(1) / v.x, T(1) / v.y, T(1) / v.z, T(1) / v.w);
}



/* *****************************************************************************
 * vector I/O
 **************************************************************************** */
/// Two dimensional
template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Vec2<T>& v) {
	out << v.x << ' ' << v.y;
	return out;
}


template<typename T>
inline std::istream& operator>>(std::istream& in, Vec2<T>& v) {
	in >> v.x >> v.y;
	return in;
}


/// Three dimensional
template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Vec3<T>& v) {
	out << v.x << ' ' << v.y << ' ' << v.z;
	return out;
}


template<typename T>
inline std::istream& operator>>(std::istream& in, Vec3<T>& v) {
	in >> v.x >> v.y >> v.z;
	return in;
}


/// Four dimensional
template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Vec4<T>& v) {
	out << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
	return out;
}


template<typename T>
inline std::istream& operator>>(std::istream& in, Vec4<T>& v) {
	in >> v.x >> v.y >> v.z >> v.w;
	return in;
}


/// n-dimensional load
template<class vec>
inline void load(std::istream& in, vec& v) {
	in.read((char*)&v, sizeof(vec));
}


/// n-dimensional save
template<class vec>
inline void save(std::ostream& out, const vec& v) {
	out.write((char*)&v, sizeof(vec));
}



/* *****************************************************************************
 * relational functions
 **************************************************************************** */
/* *****************************************************************************
 * a < b
 **************************************************************************** */
template<typename T, typename C>
inline bvec2 lessThan(const Vec2<T, C>& a, const Vec2<T, C>& b) {
    return bvec2(a.x < b.x,
                 a.y < b.y);
}


template<typename T, typename C>
inline bvec3 lessThan(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    return bvec3(a.x < b.x,
                 a.y < b.y,
                 a.z < b.z);
}


template<typename T, typename C>
inline bvec4 lessThan(const Vec4<T, C>& a, const Vec4<T, C>& b) {
    return bvec4(a.x < b.x,
                 a.y < b.y,
                 a.z < b.z,
                 a.w < b.w);
}


/* *****************************************************************************
 * a == b
 **************************************************************************** */
template<typename T, typename C>
inline bvec2 equal(const Vec2<T, C>& a, const Vec2<T, C>& b) {
    C eq;
    return bvec2(eq(a.x, b.x),
                 eq(a.y, b.y));
}


template<typename T, typename C>
inline bvec3 equal(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    C eq;
    return bvec3(eq(a.x, b.x),
                 eq(a.y, b.y),
                 eq(a.z, b.z));
}


template<typename T, typename C>
inline bvec4 equal(const Vec4<T, C>& a, const Vec4<T, C>& b) {
    C eq;
    return bvec4(eq(a.x, b.x),
                 eq(a.y, b.y),
                 eq(a.z, b.z),
                 eq(a.w, b.w));
}


/* *****************************************************************************
 * a != b
 **************************************************************************** */
inline bvec2 operator not (const bvec2& v) {
    return bvec2(not v.x, not v.y);
}


inline bvec3 operator not (const bvec3& v) {
    return bvec3(not v.x, not v.y, not v.z);
}


inline bvec4 operator not (const bvec4& v) {
    return bvec4(not v.x, not v.y, not v.z, not v.w);
}


/* *****************************************************************************
 * a and b
 **************************************************************************** */
inline bvec2 operator and (const bvec2& a, const bvec2& b) {
    return bvec2(a.x and b.x,
                 a.y and b.y);
}


inline bvec3 operator and (const bvec3& a, const bvec3& b) {
    return bvec3(a.x and b.x,
                 a.y and b.y,
                 a.z and b.z);
}


inline bvec4 operator and (const bvec4& a, const bvec4& b) {
    return bvec4(a.x and b.x,
                 a.y and b.y,
                 a.z and b.z,
                 a.w and b.w);
}


/* *****************************************************************************
 * a or b
 **************************************************************************** */
inline bvec2 operator or (const bvec2& a, const bvec2& b) {
    return bvec2(a.x or b.x,
                 a.y or b.y);
}


inline bvec3 operator or (const bvec3& a, const bvec3& b) {
    return bvec3(a.x or b.x,
                 a.y or b.y,
                 a.z or b.z);
}


inline bvec4 operator or (const bvec4& a, const bvec4& b) {
    return bvec4(a.x or b.x,
                 a.y or b.y,
                 a.z or b.z,
                 a.w or b.w);
}


/* *****************************************************************************
 * a != b
 **************************************************************************** */
template<typename T, typename C>
inline Vec2<bool> notEqual(const Vec2<T, C>& a, const Vec2<T, C>& b) {
	return not equal(a, b);
}


template<typename T, typename C>
inline Vec3<bool> notEqual(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    return not equal(a, b);
}


template<typename T, typename C>
inline Vec4<bool> notEqual(const Vec4<T, C>& a, const Vec4<T, C>& b) {
    return not equal(a, b);
}


/* *****************************************************************************
 * a > b
 **************************************************************************** */
template<typename T, typename C>
inline Vec2<bool> greaterThan(const Vec2<T, C>& a, const Vec2<T, C>& b) {
    return not greaterThan(a, b) and not equal(a, b);
}


template<typename T, typename C>
inline Vec3<bool> greaterThan(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    return not greaterThan(a, b) and not equal(a, b);
}


template<typename T, typename C>
inline Vec4<bool> greaterThan(const Vec4<T, C>& a, const Vec4<T, C>& b) {
    return not greaterThan(a, b) and not equal(a, b);
}


/* *****************************************************************************
 * a <= b
 **************************************************************************** */
template<typename T, typename C>
inline Vec2<bool> lessThanEqual(const Vec2<T, C>& a, const Vec2<T, C>& b) {
	return lessThan(a, b) or equal(a, b);
}


template<typename T, typename C>
inline Vec3<bool> lessThanEqual(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    return lessThan(a, b) or equal(a, b);
}


template<typename T, typename C>
inline Vec4<bool> lessThanEqual(const Vec4<T, C>& a, const Vec4<T, C>& b) {
    return lessThan(a, b) or equal(a, b);
}


/* *****************************************************************************
 * a >= b
 **************************************************************************** */
template<typename T, typename C>
inline Vec2<bool> greaterThanEqual(const Vec2<T, C>& a, const Vec2<T, C>& b) {
	return not lessThan(a, b);
}


template<typename T, typename C>
inline Vec3<bool> greaterThanEqual(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    return not lessThan(a, b);
}


template<typename T, typename C>
inline Vec4<bool> greaterThanEqual(const Vec4<T, C>& a, const Vec4<T, C>& b) {
    return not lessThan(a, b);
}


/* *****************************************************************************
 * any v
 **************************************************************************** */
inline bool any(const bvec2& v) {
    return v.x or v.y;
}


inline bool any(const bvec3& v) {
    return v.x or v.y or v.z;
}


inline bool any(const bvec4& v) {
    return v.x or v.y or v.z or v.w;
}


/* *****************************************************************************
 * all v
 **************************************************************************** */
inline bool all(const bvec2& v) {
    return v.x and v.y;
}


inline bool all(const bvec3& v) {
    return v.x and v.y and v.z;
}


inline bool all(const bvec4& v) {
    return v.x and v.y and v.z and v.w;
}


/* *****************************************************************************
 * Implementation of the vector comparison
 **************************************************************************** */
template<typename T, typename C>
inline bool Vec2<T, C>::operator==(const Vec2<T, C>& v) const {
    return all(equal(*this, v));
}


template<typename T, typename C>
inline bool Vec3<T, C>::operator==(const Vec3<T, C>& v) const {
    return all(equal(*this, v));
}


template<typename T, typename C>
inline bool Vec4<T, C>::operator==(const Vec4<T, C>& v) const {
    return all(equal(*this, v));
}


/* *****************************************************************************
 * min component wise operators for vectors.
 **************************************************************************** */
template<typename T, typename C>
inline Vec2<T, C> min(const Vec2<T, C>& a, const Vec2<T, C>& b) {
    return Vec2<T, C>(std::min(a.x, b.x),
                      std::min(a.y, b.y));
}


template<typename T, typename C>
inline Vec3<T, C> min(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    return Vec3<T, C>(std::min(a.x, b.x),
                      std::min(a.y, b.y),
                      std::min(a.z, b.z));
}


template<typename T, typename C>
inline Vec4<T, C> min(const Vec4<T, C>& a, const Vec4<T, C>& b) {
    return Vec4<T, C>(std::min(a.x, b.x),
                      std::min(a.y, b.y),
                      std::min(a.z, b.z),
                      std::min(a.w, b.w));
}


/* *****************************************************************************
 * max component wise operators for vectors.
 **************************************************************************** */
template<typename T, typename C>
inline Vec2<T, C> max(const Vec2<T, C>& a, const Vec2<T, C>& b) {
    return Vec2<T, C>(std::max(a.x, b.x),
                      std::max(a.y, b.y));
}


template<typename T, typename C>
inline Vec3<T, C> max(const Vec3<T, C>& a, const Vec3<T, C>& b) {
    return Vec3<T, C>(std::max(a.x, b.x),
                      std::max(a.y, b.y),
                      std::max(a.z, b.z));
}


template<typename T, typename C>
inline Vec4<T, C> max(const Vec4<T, C>& a, const Vec4<T, C>& b) {
    return Vec4<T, C>(std::max(a.x, b.x),
                      std::max(a.y, b.y),
                      std::max(a.z, b.z),
                      std::max(a.w, b.w));
}


/* *****************************************************************************
 * min/max component wise operators for vectors and single values.
 **************************************************************************** */
template<template<typename T, typename C> class vec, typename T, typename C>
inline vec<T, C> min(const vec<T, C>& v, const T& t) {
    return min(v, vec<T, C>(t));
}


template<template<typename T, typename C> class vec, typename T, typename C>
inline vec<T, C> max(const vec<T, C>& v, const T& t) {
    return max(v, vec<T, C>(t));
}


/* *****************************************************************************
 * Clamp vectors
 **************************************************************************** */
template<template<typename T, typename C> class vec, typename T, typename C>
inline vec<T, C> clamp(const vec<T, C>& t,
                       const vec<T, C>& a,
                       const vec<T, C>& b)
{
    assert(all(lessThanEqual(a, b)));
    return max(min(t, b), a);
}


template<template<typename T, typename C> class vec, typename T, typename C>
inline vec<T, C> clamp(const vec<T, C>& t,
                       const T& a,
                       const T& b)
{
    C eq;
    assert(a < b or eq(a, b));
    return max(min(t, b), a);
}


/* *****************************************************************************
 * Intervals.
 **************************************************************************** */
/**
 * @param v a point
 * @param min lower side of the interval
 * @param max upper side of the interval
 * @returns true if v is in the rectangle, parallelepiped, or hyperrectangle
 * formed by min and max.
 */
template<typename vec>
inline bool contains(const vec& v, const vec& min, const vec& max) {
    return all(greaterThanEqual(v, min)) and all(lessThanEqual(v, max));
}


template<typename vec>
struct IntersectionResult {
    inline IntersectionResult(const bool intersected,
                              const vec& begin,
                              const vec& end):
        intersected(intersected),
        begin(begin),
        end(end)
    {}

    bool intersected;
    vec begin;
    vec end;
};


/**
 * [c0, c1] = intersection of n-dimensional intervals [a0, a1] and [b0, b1]
 * @param a0 the start of interval a
 * @param a1 the end of interval a
 * @param b0 the start of interval b
 * @param b1 the end of interval b
 * @returns an intersection result containing true if any part of [a0, a1]
 * is within [b0 b1], and the begin and end of the intersection.
 */
template<typename vec>
inline IntersectionResult<vec> intersect(const vec& a0, const vec& a1,
                                         const vec& b0, const vec& b1)
{
    assert(all(lessThanEqual(a0, a1)));
    assert(all(lessThanEqual(b0, b1)));

    IntersectionResult<vec> ir(all(greaterThan(a0, b1)) or
                               all(lessThan(a1, b0)),
                               clamp(a0, b0, b1),
                               clamp(a1, b0, b1));

    return ir;
}


/* *****************************************************************************
 * @param p some point in space
 * @param a line point a
 * @param b line point b
 * @pre a != b
 * @returns the point on the straight line defined by ab, which is closer to p.
 * In other words, p' the projection of p onto ab.
 **************************************************************************** */
template<typename vec>
inline vec project(const vec& p, const vec& a, const vec& b) {
    assert(a != b);

    const vec& ab_n(normalize(b - a));
    const vec& ap(p - a);
    const vec& p_prime(a + ab_n * dot(ab_n, ap));

    return p_prime;
}


/* *****************************************************************************
 * Area and volume
 **************************************************************************** */
template<typename T, typename C>
inline T area(const Vec2<T, C>& v) {
    return v.x * v.y;
}


template<typename T, typename C>
inline T volume(const Vec3<T, C>& v) {
    return v.x * v.y * v.z;
}


} // namespace cgl


#endif // VEC_HPP
