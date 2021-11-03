#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in float instanceId;

out vec2 f_uv;
out flat int f_id;
out vec3 f_objPos;
out vec3 f_normal;

layout (std140) uniform Camera {
    mat4 camera;
};

uniform mat4 transform[128];

void main() {
    f_uv = uv;
    f_id = gl_InstanceID;
    vec4 objPos = transform[gl_InstanceID] * vec4(position, 1.0);
    f_objPos = vec3(objPos);
    f_normal = normal;
    gl_Position = camera * objPos;
}