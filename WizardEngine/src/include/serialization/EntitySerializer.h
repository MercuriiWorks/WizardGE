//
// Created by mecha on 21.05.2022.
//

#pragma once

#include <ecs/Entity.h>
#include <yaml/yaml.h>

#define ENTITY_SERIALIZE_TEXT(entity, emitter) engine::io::EntitySerializer(entity).serializeText(emitter)
#define ENTITY_SERIALIZE_BINARY(entity, filepath) engine::io::EntitySerializer(entity).serializeBinary(filepath)

#define ENTITY_DESERIALIZE_TEXT(entity, node) engine::io::EntitySerializer(entity).deserializeText(node)
#define ENTITY_DESERIALIZE_BINARY(entity, filepath) engine::io::EntitySerializer(entity).deserializeBinary(filepath)

namespace engine::io {

    class ENGINE_API EntitySerializable {

    public:
        EntitySerializable(const ecs::Entity& entity) : entity(entity) {}

    public:
        void serialize(YAML::Emitter &out);
        void deserialize(const YAML::Node &parent);

    private:
        void serializeComponents(YAML::Emitter& out);
        template<typename T>
        void deserializeComponent(const YAML::Node &entityNode);
        template<typename T>
        void serializeComponent(YAML::Emitter& out);

    private:
        ecs::Entity entity;
    };

    template<typename T>
    void EntitySerializable::deserializeComponent(const YAML::Node &entityNode) {
        T* component = entity.get<T>();
        if (component) {
            component->deserialize(entityNode);
        } else {
            entity.add<T>(); // generates + adds new T component
            T* newComponent = entity.get<T>();
            newComponent->deserialize(entityNode);
        }
    }

    template<typename T>
    void EntitySerializable::serializeComponent(YAML::Emitter &out) {
        T* component = entity.get<T>();
        if (component) {
            component->serialize(out);
        }
    }

    class ENGINE_API EntitySerializer final {

    public:
        EntitySerializer(const ecs::Entity& entity) : entity(entity) {}

    public:
        void serializeText(YAML::Emitter& out);
        void serializeBinary(const char* filepath);
        const char* serializeText();
        void serializeTextFile(const char* filepath);

        void deserializeText(const YAML::Node& entityNode);
        void deserializeBinary(const char* filepath);
        void deserializeText(const char* entityText);
        bool deserializeTextFile(const char* filepath);

    private:
        EntitySerializable entity;
    };

}
