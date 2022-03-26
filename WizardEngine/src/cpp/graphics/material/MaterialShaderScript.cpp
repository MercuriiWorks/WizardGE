//
// Created by mecha on 01.11.2021.
//

#include <graphics/material/MaterialShaderScript.h>

namespace engine::shader {

    ShaderScript materialScript() {
        auto script = ShaderScript();
        script.updateRegistry = [](const BaseShader& shader, entt::registry &registry) {
            auto materials = registry.view<MaterialComponent>();
            for (auto [entity, material] : materials.each()) {
                updateMaterial(shader, material);
            }
        };
        script.updateEntity = [](const BaseShader& shader, const Entity& entity) {
            auto material = entity.getPtr<MaterialComponent>();
            if (material) {
                updateMaterial(shader, *material);
            }
        };
        return script;
    }

    ShaderScript materialUboScript() {
        auto script = ShaderScript();
        script.updateRegistry = [](const BaseShader& shader, entt::registry &registry) {
            auto materials = registry.view<MaterialComponent>();
            auto index = 0;
            for (auto [entity, material] : materials.each()) {
                updateMaterialUbo(shader, material, index++);
            }
        };
        script.updateEntity = [](const BaseShader& shader, const Entity& entity) {
            auto material = entity.getPtr<MaterialComponent>();
            if (material) {
                updateMaterialUbo(shader, *material, 0);
            }
        };
        return script;
    }

    ShaderScript materialArrayScript() {
        auto script = ShaderScript();
        script.updateRegistry = [](const BaseShader& shader, entt::registry &registry) {
            auto materials = registry.view<MaterialComponent>();
            auto index = 0;
            for (auto [entity, material] : materials.each()) {
                updateMaterial(shader, material, index++);
            }
        };
        script.updateEntity = [](const BaseShader& shader, const Entity& entity) {
            auto material = entity.getPtr<MaterialComponent>();
            if (material) {
                updateMaterial(shader, *material, 0);
            }
        };
        return script;
    }

    ShaderScript materialMapScript() {
        auto script = ShaderScript();
        script.updateRegistry = [](const BaseShader& shader, entt::registry &registry) {
            auto materialMapsView = registry.view<MaterialMapsComponent>();
            for (auto [entity, materialMaps] : materialMapsView.each()) {
                updateMaterialMaps(shader, materialMaps);
            }
        };
        script.updateEntity = [](const BaseShader& shader, const Entity& entity) {
            auto materialMaps = entity.getPtr<MaterialMapsComponent>();
            if (materialMaps) {
                updateMaterialMaps(shader, *materialMaps);
            }
        };
        return script;
    }

    ShaderScript materialMapUboScript() {
        auto script = ShaderScript();
        script.updateRegistry = [](const BaseShader& shader, entt::registry &registry) {
            auto materialMapsView = registry.view<MaterialMapsComponent>();
            for (auto [entity, materialMaps] : materialMapsView.each()) {
                updateMaterialMapsUbo(shader, materialMaps);
            }
        };
        script.updateEntity = [](const BaseShader& shader, const Entity& entity) {
            auto materialMaps = entity.getPtr<MaterialMapsComponent>();
            if (materialMaps) {
                updateMaterialMapsUbo(shader, *materialMaps);
            }
        };
        return script;
    }

    ShaderScript materialMapArrayScript() {
        auto script = ShaderScript();
        script.updateRegistry = [](const BaseShader& shader, entt::registry &registry) {
            auto materialMaps = registry.view<MaterialMapsComponent>();
            auto index = 0;
            for (auto [entity, materialMap] : materialMaps.each()) {
                updateMaterialMaps(shader, materialMap, index++);
            }
        };
        script.updateEntity = [](const BaseShader& shader, const Entity& entity) {
            auto materialMap = entity.getPtr<MaterialMapsComponent>();
            if (materialMap) {
                updateMaterialMaps(shader, *materialMap, 0);
            }
        };
        return script;
    }

    void updateMaterial(const BaseShader& shader, MaterialComponent& material) {
        shader.setUniformStructField(material.name, material.color);
        shader.setUniformStructField(material.name, material.ambient);
        shader.setUniformStructField(material.name, material.diffuse);
        shader.setUniformStructField(material.name, material.specular);
        shader.setUniformStructField(material.name, material.shiny);
        shader.setUniformStructField(material.name, material.diffuseMapEnabled);
        shader.setUniformStructField(material.name, material.specularMapEnabled);
    }

    void updateMaterial(const BaseShader& shader, MaterialComponent& material, const uint32_t& index) {
        shader.setUniformArrayStructField(index, material.name, material.color);
        shader.setUniformArrayStructField(index, material.name, material.ambient);
        shader.setUniformArrayStructField(index, material.name, material.diffuse);
        shader.setUniformArrayStructField(index, material.name, material.specular);
        shader.setUniformArrayStructField(index, material.name, material.shiny);
        shader.setUniformArrayStructField(index, material.name, material.diffuseMapEnabled);
        shader.setUniformArrayStructField(index, material.name, material.specularMapEnabled);
    }

    void updateMaterialMaps(const BaseShader& shader, MaterialMapsComponent& materialMaps) {
        shader.setUniformStructField(materialMaps.name, materialMaps.diffuse.sampler);
        ACTIVATE_TEXTURE(materialMaps.diffuse);
        shader.setUniformStructField(materialMaps.name, materialMaps.specular.sampler);
        ACTIVATE_TEXTURE(materialMaps.specular);
    }

    void updateMaterialMaps(const BaseShader& shader, MaterialMapsComponent& materialMaps, const uint32_t& index) {
        shader.setUniformArrayStructField(index, materialMaps.name, materialMaps.diffuse.sampler);
        ACTIVATE_TEXTURE(materialMaps.diffuse);
        shader.setUniformArrayStructField(index, materialMaps.name, materialMaps.specular.sampler);
        ACTIVATE_TEXTURE(materialMaps.specular);
    }

    void updateMaterialUbo(const BaseShader& shader, MaterialComponent& material, const uint32_t& index) {
        uint32_t i = 0;
        shader.updateUniformBuffer(material.color, i++);
        shader.updateUniformBuffer(material.ambient, i++);
        shader.updateUniformBuffer(material.diffuse, i++);
        shader.updateUniformBuffer(material.specular, i++);
        shader.updateUniformBuffer(material.shiny, i++);
        shader.updateUniformBuffer(material.diffuseMapEnabled, i++);
        shader.updateUniformBuffer(material.specularMapEnabled, i++);
    }

    void updateMaterialMapsUbo(const BaseShader& shader, MaterialMapsComponent& materialMaps) {
        uint32_t i = 0;
        shader.updateUniformBuffer(materialMaps.diffuse.sampler, i++);
        ACTIVATE_TEXTURE(materialMaps.diffuse);
        shader.updateUniformBuffer(materialMaps.specular.sampler, i++);
        ACTIVATE_TEXTURE(materialMaps.specular);
    }

}