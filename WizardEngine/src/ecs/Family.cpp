//
// Created by mecha on 24.10.2021.
//

#include "Family.h"

namespace engine {

    void Family::destroy() {
        container = nullptr;
        clear();
    }

    Entity Family::createEntity3d(const std::string &tag) {
        Entity entity = container->createEntity3d(tag);
        _entities.push_back(entity);
        return entity;
    }

    Entity Family::createEntity2d(const std::string &tag) {
        Entity entity = container->createEntity2d(tag);
        _entities.push_back(entity);
        return entity;
    }

    void Family::deleteEntity(const Entity &entity) {
        container->deleteEntityFromRegistry(entity);
        auto it = std::find(_entities.begin(), _entities.end(), entity);
        _entities.erase(it);
    }

    void Family::clear() {
        for (const auto& entity : _entities) {
            container->deleteEntityFromRegistry(entity);
        }
        _entities.clear();
    }

    bool Family::isEmpty() const {
        return _entities.empty();
    }

    size_t Family::size() const {
        return _entities.size();
    }

    const Entity &Family::getEntity(const uint32_t &index) {
        return _entities[index];
    }

    const Entity &Family::operator[](const uint32_t &index) const {
        return _entities[index];
    }

    Entity Family::createEntity(const std::string &tag) {
        return container->createEntity(tag);
    }
}