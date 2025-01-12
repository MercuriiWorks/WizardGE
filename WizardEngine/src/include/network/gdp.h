//
// Created by mecha on 25.05.2022.
//

#pragma once

#include <core/vector.h>
#include <core/exception.h>
#include <serialization/serialization.h>

/**
 * Game Data Protocol - used for transferring entity and component data through TCP/UDP network
 * address - used to recognize a source : Client-to-Client, Client-to-Server, Server-to-Client
 * type - used to recognize type of data to deserialize data : ex. type: , data: "Open_Door";
 * data - actual data that should be deserialized by provided Service class from the Game.
 * */
namespace engine::network {

    using namespace core;

    struct ENGINE_API NetworkData {
        char* data = nullptr;
        size_t size = 0;

        NetworkData(char* data, size_t size) : data(data), size(size) {}
    };

    // addresses
    constexpr u32 CLIENT_TO_CLIENT = 0;
    constexpr u32 CLIENT_TO_SERVER = 1;
    constexpr u32 SERVER_TO_CLIENT = 2;

    // types
    constexpr u32 SERVER_SAVE_SCENE = 1;
    constexpr u32 SERVER_LOAD_SCENE = 2;

    // states
    constexpr u32 SERVER_SUCCESS = 1;
    constexpr u32 SERVER_ERROR = 2;

    struct GDHeader {
        serializable()
        u32 address;
        u32 type;

        GDHeader() = default;
        GDHeader(u32 address, u32 type) : address(address), type(type) {}
    };

    class ENGINE_API GDSerializer final {
    public:
        template<typename GDBody>
        static NetworkData serialize(GDHeader& header, GDBody& body);
        static bool deserialize(char* gameData, std::pair<YAML::Node, GDHeader>& gdNodeHeader);
    };

    template<class V>
    struct GDPrimitive {
        decl_serializable()
        V value;
        GDPrimitive() = default;
        GDPrimitive(const V& value) : value(value) {}
    };

    template<class V>
    void GDPrimitive<V>::serialize(YAML::Emitter &out) {
        out << YAML::BeginMap;
        yaml::serialize(out, "value", value);
        out << YAML::EndMap;
    }

    template<class V>
    void GDPrimitive<V>::deserialize(const YAML::Node &parent) {
        value = parent["value"].template as<V>();
    }

    typedef GDPrimitive<u8> GDByte;
    typedef GDPrimitive<s8> GDChar;

    typedef GDPrimitive<u16> GDUShort;
    typedef GDPrimitive<s16> GDShort;

    typedef GDPrimitive<u32> GDUInt;
    typedef GDPrimitive<s32> GDInt;

    typedef GDPrimitive<f32> GDFloat;

    typedef GDPrimitive<u64> GDULong;
    typedef GDPrimitive<s64> GDLong;

    typedef GDPrimitive<double> GDDouble;

    typedef GDPrimitive<std::string> GDString;

    template<class V>
    struct GDPrimitives {
        decl_serializable()
        vector<V> values;
        GDPrimitives(const std::initializer_list<V>& values) : values(values) {}
    };

    template<typename GDBody>
    NetworkData GDSerializer::serialize(GDHeader &header, GDBody &body) {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;

        emitter << YAML::Key << "GameData";
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "header";
        header.serialize(emitter);
        emitter << YAML::Key << "body";
        body.serialize(emitter);
        emitter << YAML::EndMap;

        emitter << YAML::EndMap;
        // need to duplicate emitter c_str into network data, because emitter lives only in scopes of stack function
        NetworkData networkData(strdup(emitter.c_str()), emitter.size());
        ENGINE_INFO("GDSerializer: serialized NetworkData: \n{0}", networkData.data);
        return networkData;
    }

    template<class V>
    void GDPrimitives<V>::serialize(YAML::Emitter &out) {
        out << YAML::BeginMap;
        out << YAML::Key << "values" << YAML::Value << YAML::Flow;
        out << YAML::BeginSeq;
        for (auto value : values) {
            out << value;
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;
    }

    template<class V>
    void GDPrimitives<V>::deserialize(const YAML::Node &parent) {
        YAML::Node valuesNode = parent["values"];
    }

    /**
     * V - class/struct, which should have implemented Serializable interface. Otherwise it won't compile!
     * */
    template<class V>
    struct GDSerializables {
        decl_serializable()
        vector<V> serializables;
        GDSerializables(const std::initializer_list<V>& serializables) : serializables(serializables) {}
    };

    template<class V>
    void GDSerializables<V>::serialize(YAML::Emitter &out) {
        out << YAML::BeginMap;
        out << YAML::Key << "serializables" << YAML::Value << YAML::Flow;
        out << YAML::BeginSeq;
        for (auto value : serializables) {
            value.serialize(out);
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;
    }

    template<class V>
    void GDSerializables<V>::deserialize(const YAML::Node &parent) {
        YAML::Node valuesNode = parent["serializables"];
        for (auto value : serializables) {
            value.deserialize(valuesNode);
        }
    }

    constexpr u16 GD_CODE_200 = 200;
    constexpr u16 GD_CODE_404 = 404;

    struct GDResponse {
        serializable()
        u16 statusCode = GD_CODE_200;
        const char* message = "";
        GDResponse() = default;
        GDResponse(const char* message) : message(message) {}
        GDResponse(u32 statusCode, const char* message) : statusCode(statusCode), message(message) {}
    };
}
