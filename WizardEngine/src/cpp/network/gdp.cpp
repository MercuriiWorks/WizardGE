//
// Created by mecha on 29.05.2022.
//

#include <network/gdp.h>
#include <io/Logger.h>

namespace engine::network {

    void GDHeader::serialize(YAML::Emitter &out) {
        yaml::serialize(out, "address", address);
        yaml::serialize(out, "type", type);
    }

    void GDHeader::deserialize(const YAML::Node &parent) {
        address = parent["address"].as<u32>();
        type = parent["type"].as<u32>();
    }

    NetworkData GDSerializer::serialize(GDHeader &header, GDBody &body) {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;

        emitter << YAML::Key << "GameData";
        emitter << YAML::BeginMap;
        header.serialize(emitter);
        body.serialize(emitter);
        emitter << YAML::EndMap;

        emitter << YAML::EndMap;
        // need to duplicate emitter c_str into network data, because emitter lives only in scopes of stack function
        NetworkData networkData(strdup(emitter.c_str()), emitter.size());
        ENGINE_INFO("GDSerializer: serialized NetworkData: \n{0}", networkData.data);
        return networkData;
    }

    bool GDSerializer::deserialize(char *gameData, std::pair<YAML::Node, GDHeader>& gdNodeHeader) {
        try {
            YAML::Node gdNode = YAML::Load(gameData)["GameData"];
            if (gdNode) {
                GDHeader header;
                header.deserialize(gdNode);
                gdNodeHeader = { gdNode, header };
                return true;
            }

            return false;
        } catch (YAML::ParserException& e) {
            ENGINE_ERR("GDSerializer: Failed to parse YAML text!");
            ENGINE_ERR(e.msg);
            return false;
        }
    }
}