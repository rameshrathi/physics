#pragma once

#include <Physics/Vector2D.h>

// Base class for physics bodies
class PhysicsBody {
public:
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;
    float mass;
    float restitution; // bounciness (0-1)
    bool isStatic;
    
    PhysicsBody(Vector2D pos, float m, float rest = 0.8f) 
        : position(pos), mass(m), restitution(rest), isStatic(false) {}
    
    virtual ~PhysicsBody() = default;
    
    void update(float dt) {
        if (isStatic) return;
        
        velocity += acceleration * dt;
        position += velocity * dt;
        
        // Reset acceleration (forces applied per frame)
        acceleration = Vector2D(0, 0);
    }
    
    void applyForce(const Vector2D& force) {
        if (!isStatic && mass > 0) {
            acceleration += force * (1.0f / mass);
        }
    }
    
    virtual bool checkCollision(const PhysicsBody& other) const = 0;
    virtual void resolveCollision(PhysicsBody& other) = 0;
    virtual void draw() const = 0;
};

