#pragma once

#include <cmath>

// Vector2D class for 2D math operations
class Vector2D {
public:
    float x, y;
    
    Vector2D(float x = 0, float y = 0) : x(x), y(y) {}
    
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    
    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    
    Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2D& operator-=(const Vector2D& other) {
	x -= other.x;
	y += other.y;
	return *this;
    }
    
    float dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }
    
    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }
    
    Vector2D normalize() const {
        float mag = magnitude();
        if (mag > 0) return Vector2D(x / mag, y / mag);
        return Vector2D(0, 0);
    }
};
