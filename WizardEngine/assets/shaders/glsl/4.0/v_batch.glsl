#version 400 core

#include include/v_scene_attributes.glsl
layout(location = 4) in float id;
#include include/v_scene.glsl

void main() {
    updateObject(id);
}