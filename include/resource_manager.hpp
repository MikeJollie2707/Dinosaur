#ifndef DINOSAUR_INCLUDE_RESOURCE_MANAGER_HPP
#define DINOSAUR_INCLUDE_RESOURCE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <exception>
#include <cassert>

template<typename Resource, typename Identifier>
class ResourceManager
{
public:
    void load(Identifier id, const std::string& filename) {
        std::unique_ptr<Resource> resource = std::make_unique<Resource>();
        if (!resource->loadFromFile(filename)) {
            throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);
        }

        auto existed = resource_map.insert(std::make_pair(id, std::move(resource)));
        assert(existed.second);
    }
    Resource& get(Identifier id) {
        auto existed = resource_map.find(id);
        return *existed->second;
    }
    const Resource& get(Identifier id) const {
        auto existed = resource_map.find(id);
        return *existed->second;
    }

private:
    std::map<Identifier, std::unique_ptr<Resource>> resource_map;
};

#endif