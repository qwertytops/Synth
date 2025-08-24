#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include "SynthComponent.hpp"

class SynthComponentFactory {
public:
    static SynthComponentFactory& instance();

    void registerClass(const std::string& name, std::function<SynthComponent*()> creator);
    SynthComponent* create(const std::string& name);

    std::vector<std::string> availableClasses() const;

private:
    std::unordered_map<std::string, std::function<SynthComponent*()>> registry;
};

#define REGISTER_COMPONENT(CLASS_NAME) \
    static bool _registered_##CLASS_NAME = [](){ \
        SynthComponentFactory::instance().registerClass(#CLASS_NAME, []() { \
            return new CLASS_NAME(); \
        }); \
        return true; \
    }()