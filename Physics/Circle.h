// Circle physics body

#include <algorithm>
#include <iostream>
#include <Physics/Vector2D.h>
#include <Physics/PhysicsBody.h>

class Circle : public PhysicsBody {
public:
    float radius;
    
    Circle(Vector2D pos, float r, float m, float rest = 0.8f) 
        : PhysicsBody(pos, m, rest), radius(r) {}
    
    bool checkCollision(const PhysicsBody& other) const override {
        const Circle* otherCircle = dynamic_cast<const Circle*>(&other);
        if (otherCircle) {
            Vector2D diff = position - otherCircle->position;
            float distSq = diff.dot(diff);
            float radSum = radius + otherCircle->radius;
            return distSq <= radSum * radSum;
        }
        return false;
    }
    
    void resolveCollision(PhysicsBody& other) override {
        Circle* otherCircle = dynamic_cast<Circle*>(&other);
        if (!otherCircle) return;
        
        Vector2D diff = position - otherCircle->position;
        float dist = diff.magnitude();
        
        if (dist == 0) return; // Avoid division by zero
        
        Vector2D normal = diff.normalize();
        float overlap = radius + otherCircle->radius - dist;
        
        // Separate objects
        if (!isStatic && !otherCircle->isStatic) {
            Vector2D separation = normal * (overlap * 0.5f);
            position += separation;
            otherCircle->position -= separation;
        } else if (!isStatic) {
            position += normal * overlap;
        } else if (!otherCircle->isStatic) {
            otherCircle->position -= normal * overlap;
        }
        
        // Calculate relative velocity
        Vector2D relVel = velocity - otherCircle->velocity;
        float velAlongNormal = relVel.dot(normal);
        
        // Don't resolve if velocities are separating
        if (velAlongNormal > 0) return;
        
        // Calculate restitution
        float e = std::min(restitution, otherCircle->restitution);
        
        // Calculate impulse scalar
        float j = -(1 + e) * velAlongNormal;
        j /= (1.0f / mass) + (1.0f / otherCircle->mass);
        
        // Apply impulse
        Vector2D impulse = normal * j;
        if (!isStatic) velocity += impulse * (1.0f / mass);
        if (!otherCircle->isStatic) otherCircle->velocity -= impulse * (1.0f / otherCircle->mass);
    }
    
    void draw() const override {
        std::cout << "Circle at (" << position.x << ", " << position.y 
                  << ") radius: " << radius << std::endl;
    }
};
