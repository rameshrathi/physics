#pragma once

// Rectangle physics body
#include <Physics/PhysicsBody.h>
#include <Physics/Vector2D.h>
#include <iostream>

class Rectangle : public PhysicsBody {
public:
    float width, height;
    
    Rectangle(Vector2D pos, float w, float h, float m, float rest = 0.8f) 
        : PhysicsBody(pos, m, rest), width(w), height(h) {}
    
    bool checkCollision(const PhysicsBody& other) const override {
        const Rectangle* otherRect = dynamic_cast<const Rectangle*>(&other);
        if (otherRect) {
            return (position.x < otherRect->position.x + otherRect->width &&
                    position.x + width > otherRect->position.x &&
                    position.y < otherRect->position.y + otherRect->height &&
                    position.y + height > otherRect->position.y);
        }
        return false;
    }
    
    void resolveCollision(PhysicsBody& other) override {
        Rectangle* otherRect = dynamic_cast<Rectangle*>(&other);
        if (!otherRect) return;
        
        // Simple AABB collision resolution
        Vector2D center1 = position + Vector2D(width * 0.5f, height * 0.5f);
        Vector2D center2 = otherRect->position + Vector2D(otherRect->width * 0.5f, otherRect->height * 0.5f);
        
        Vector2D diff = center1 - center2;
        float overlapX = (width + otherRect->width) * 0.5f - std::abs(diff.x);
        float overlapY = (height + otherRect->height) * 0.5f - std::abs(diff.y);
        
        if (overlapX > 0 && overlapY > 0) {
            Vector2D normal;
            float separation;
            
            if (overlapX < overlapY) {
                normal = Vector2D(diff.x > 0 ? 1 : -1, 0);
                separation = overlapX;
            } else {
                normal = Vector2D(0, diff.y > 0 ? 1 : -1);
                separation = overlapY;
            }
            
            // Separate objects
            if (!isStatic && !otherRect->isStatic) {
                Vector2D sep = normal * (separation * 0.5f);
                position += sep;
                otherRect->position -= sep;
            } else if (!isStatic) {
                position += normal * separation;
            } else if (!otherRect->isStatic) {
                otherRect->position -= normal * separation;
            }
            
            // Apply collision response
            Vector2D relVel = velocity - otherRect->velocity;
            float velAlongNormal = relVel.dot(normal);
            
            if (velAlongNormal > 0) return;
            
            float e = std::min(restitution, otherRect->restitution);
            float j = -(1 + e) * velAlongNormal;
            j /= (1.0f / mass) + (1.0f / otherRect->mass);
            
            Vector2D impulse = normal * j;
            if (!isStatic) velocity += impulse * (1.0f / mass);
            if (!otherRect->isStatic) otherRect->velocity -= impulse * (1.0f / otherRect->mass);
        }
    }
    
    void draw() const override {
        std::cout << "Rectangle at (" << position.x << ", " << position.y 
                  << ") size: " << width << "x" << height << std::endl;
    }
};
