// LAB/main.cpp
#include <Physics/Circle.h>
#include <Physics/Rectangle.h>
#include <Physics/PhysicsBody.h>
#include <Physics/PhysicsWorld.h>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <iostream>

int main(int argc, char *argv[])
{
    PhysicsWorld world(Vector2D(0, 98.0f));

    // Create some physics bodies
    auto circle1 = std::make_unique<Circle>(Vector2D(100, 50), 20, 1.0f, 0.9f);
    auto circle2 = std::make_unique<Circle>(Vector2D(150, 100), 15, 0.8f, 0.8f);
    auto ground = std::make_unique<Rectangle>(Vector2D(0, 400), 800, 50, 1.0f, 0.6f);

    // Make ground static
    ground->isStatic = true;

    // Add initial velocities
    circle1->velocity = Vector2D(50, -20);
    circle2->velocity = Vector2D(-30, 10);

    // Add bodies to world
    world.addBody(std::move(circle1));
    world.addBody(std::move(circle2));
    world.addBody(std::move(ground));

    std::cout << "Basic Physics Engine Demo" << std::endl;
    std::cout << "Bodies in world: " << world.getBodyCount() << std::endl;

    // Simulate for a few frames
    float dt = 1.0f / 60.0f; // 60 FPS

    for (int frame = 0; frame < 10; ++frame) {
        std::cout << "Frame " << frame + 1 << ":" << std::endl;
        world.update(dt);
        world.draw();
    }

    return 0;
}

