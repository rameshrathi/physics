
#ifndef docker_hpp
#define docker_hpp

#include <string>
#include <vector>
#include <map>

// --- Service Definition ---
struct ServiceDefinition {
    std::string id;
    std::string name;
    std::string description;
    std::string dockerImage;
    std::vector<std::string> ports;
    std::vector<std::string> volumes; 
    std::map<std::string, std::string> environment;
    std::string command;
    bool isLLM = false;
};

int runDockerCommand(const std::string &command);

bool startService(const ServiceDefinition &service);

bool stopService(const std::string &serviceId);

void listRunningContainers();

void listAllContainers();

void listDockerImages();

bool pullDockerImage(const std::string &image);

#endif