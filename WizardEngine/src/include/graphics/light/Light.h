//
// Created by mecha on 04.11.2021.
//

#pragma once

#include <ecs/Entity.h>
#include "LightComponents.h"

namespace engine::graphics {

    using namespace math;

    class ENGINE_API Light : public ecs::Entity {

    public:
        Light() = default;

        Light(const std::string &tag, ecs::EntityContainer* container) : Entity(tag, container) {
            create();
        }

        Light(ecs::EntityContainer* container) : Entity("Light", container) {
            create();
        }

        ~Light() = default;

    public:
        vec3f& getPosition();
        vec4f& getColor();

    private:
        void create();

    };

    class ENGINE_API PhongLight : public ecs::Entity {

    public:
        PhongLight() = default;

        PhongLight(const std::string &tag, ecs::EntityContainer* container) : Entity(tag, container) {
            create();
        }

        PhongLight(ecs::EntityContainer* container) : Entity("PhongLight", container) {
            create();
        }

        ~PhongLight() = default;

    public:
        vec3f& getPosition();
        vec4f& getColor();

    private:
        void create();

    };

    class ENGINE_API DirectLight : public ecs::Entity {

    public:
        DirectLight() = default;

        DirectLight(const std::string &tag, ecs::EntityContainer* container) : Entity(tag, container) {
            create();
        }

        DirectLight(ecs::EntityContainer* container) : Entity("DirectLight", container) {
            create();
        }

        ~DirectLight() = default;

    private:
        void create();

    };

    class ENGINE_API PointLight : public ecs::Entity {

    public:
        PointLight() = default;

        PointLight(const std::string &tag, ecs::EntityContainer* container) : Entity(tag, container) {
            create();
        }

        PointLight(ecs::EntityContainer* container) : Entity("PointLight", container) {
            create();
        }

        ~PointLight() = default;

    private:
        void create();

    };

    class ENGINE_API FlashLight : public ecs::Entity {

    public:
        FlashLight() = default;

        FlashLight(const std::string &tag, ecs::EntityContainer* container) : Entity(tag, container) {
            create();
        }

        FlashLight(ecs::EntityContainer* container) : Entity("FlashLight", container) {
            create();
        }

        ~FlashLight() = default;

    private:
        void create();

    };

}
