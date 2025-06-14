#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <Physics/PhysicsBody.h>

// Physics World class
class PhysicsWorld {
private:
    std::vector<std::unique_ptr<PhysicsBody>> bodies;
    Vector2D gravity;

public:
    PhysicsWorld(Vector2D g = Vector2D(0, 9.8f)) : gravity(g) {}

    void addBody(std::unique_ptr<PhysicsBody> body) {
        bodies.push_back(std::move(body));
    }

    void setGravity(Vector2D g) {
        gravity = g;
    }

    void update(float dt) {
        // Apply gravity to all non-static bodies
        for (auto& body : bodies) {
            if (!body->isStatic) {
                body->applyForce(gravity * body->mass);
            }
        }

        // Update all bodies
        for (auto& body : bodies) {
            body->update(dt);
        }

        // Check collisions
        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = i + 1; j < bodies.size(); ++j) {
                if (bodies[i]->checkCollision(*bodies[j])) {
                    bodies[i]->resolveCollision(*bodies[j]);
                }
            }
        }
    }

    void draw() const {
        std::cout << "=== Physics World ===" << std::endl;
        for (const auto& body : bodies) {
            body->draw();
        }
        std::cout << std::endl;
    }

    size_t getBodyCount() const {
        return bodies.size();
    }
};
