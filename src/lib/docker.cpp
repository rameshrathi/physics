#include <iostream>
#include <string>
#include <sstream>

#include "docker.hpp"

// **WARNING:** Using system() is generally insecure and not robust.
// A real application should use the Docker Engine API via a C++ client library
// or inter-process communication (IPC) with the Docker daemon.
int runDockerCommand(const std::string& command) {
    std::cout << "Executing Docker Command: " << command << std::endl;
    // In a real service, redirect stdout/stderr appropriately
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Docker command failed with exit code: " << result << std::endl;
    }
    return result;
}

// Constructs and runs a 'docker run' command for a service
bool startService(const ServiceDefinition& service) {
    std::stringstream dockerCmd;
    dockerCmd << "docker run -d --name " << service.id; // Run detached, assign name

    for (const auto& port : service.ports) {
        dockerCmd << " -p " << port;
    }
    for (const auto& volume : service.volumes) {
        dockerCmd << " -v " << volume;
    }
    for (const auto& env : service.environment) {
        dockerCmd << " -e " << env.first << "=" << env.second;
    }
    // Add other options here (network, restart policy etc.)

    dockerCmd << " " << service.dockerImage;

    if (!service.command.empty()) {
        dockerCmd << " " << service.command;
    }

    return runDockerCommand(dockerCmd.str()) == 0;
}

// Constructs and runs a 'docker stop' and 'docker rm' command
bool stopService(const std::string& serviceId) {
    std::string stopCmd = "docker stop " + serviceId;
    std::string rmCmd = "docker rm " + serviceId;
    bool stopped = runDockerCommand(stopCmd) == 0;
    // Attempt removal even if stop failed (might already be stopped)
    bool removed = runDockerCommand(rmCmd) == 0;
    return stopped || removed; // Consider success if removed
}

// --- Docker Container Management Functions ---

// List all running Docker containers
void listRunningContainers() {
    std::cout << "\n=== Running Docker Containers ===\n" << std::endl;
    runDockerCommand("docker ps --format \"table {{.ID}}\t{{.Names}}\t{{.Image}}\t{{.Status}}\t{{.Ports}}\"");
    std::cout << std::endl;
}

// List all Docker containers (running and stopped)
void listAllContainers() {
    std::cout << "\n=== All Docker Containers ===\n" << std::endl;
    runDockerCommand("docker ps -a --format \"table {{.ID}}\t{{.Names}}\t{{.Image}}\t{{.Status}}\"");
    std::cout << std::endl;
}

// List available Docker images
void listDockerImages() {
    std::cout << "\n=== Available Docker Images ===\n" << std::endl;
    runDockerCommand("docker images --format \"table {{.Repository}}:{{.Tag}}\t{{.ID}}\t{{.Size}}\"");
    std::cout << std::endl;
}

// Pull a Docker image
bool pullDockerImage(const std::string& image) {
    std::cout << "Pulling Docker image: " << image << std::endl;
    return runDockerCommand("docker pull " + image) == 0;
}