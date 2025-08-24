#include "SynthComponentFactory.hpp"

SynthComponentFactory& SynthComponentFactory::instance() {
    static SynthComponentFactory instance;
    return instance;
}

void SynthComponentFactory::registerClass(const std::string& name, std::function<SynthComponent*()> creator) {
    registry[name] = creator;
}

SynthComponent* SynthComponentFactory::create(const std::string& name) {
    auto it = registry.find(name);
    return it != registry.end() ? it->second() : nullptr;
}

std::vector<std::string> SynthComponentFactory::availableClasses() const {
    std::vector<std::string> keys;
    for (auto& pair : registry)
        keys.push_back(pair.first);
    return keys;
}
