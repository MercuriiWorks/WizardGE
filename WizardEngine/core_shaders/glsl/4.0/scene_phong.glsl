#version 400 core

#include include/f_scene.glsl
#include material/material.glsl

layout(location = 1) out vec4 brightColor;
layout(location = 2) out int uuid;
layout(location = 3) out vec4 pbrColor;

const uint LIGHT_COUNT = 4;
uniform Light light[LIGHT_COUNT];

uniform Material material[4];

uniform int uuids[2];

void main() {
    vec3 pos = f_pos;
    vec2 uv = f_uv;
    vec3 normal = f_normal;
    vec3 tangent = f_tangent;
    vec3 bitangent = f_bitangent;
    vec3 viewPos = viewPosition;
    vec3 viewDir = normalize(viewPos - pos);

    mat3 TBN = transpose(mat3(tangent, bitangent, normal));
    // apply TBN matrix
    pos *= TBN;
    viewPos *= TBN;

    vec3 albedo = material[getId()].color.rgb;
    if (material[getId()].enableAlbedoMap) {
        albedo = pow(texture(material[getId()].albedoSlot, uv).rgb, vec3(material[getId()].gamma));
    }

    if (material[getId()].enableParallaxMap) {
        uv = parallax(
        material[getId()].depthSlot, uv, viewDir,
        material[getId()].heightScale,
        material[getId()].minLayers, material[getId()].maxLayers
        );
        // UV bounds should be [0, 1]
        // discard pixels that are out of bounds [0, 1]
        if (uv.x > 1.0 || uv.y > 1.0 || uv.x < 0.0 || uv.y < 0.0) {
            discard;
        }
    }

    if (material[getId()].enableNormalMap) {
        // if tangent, bitangent is not pre-calculated yet
//        vec3 tangentNormal = texture(material[getId()].normalSlot, uv).rgb * 2.0 - 1.0;
//
//        vec3 Q1 = dFdx(pos);
//        vec3 Q2 = dFdy(pos);
//        vec2 st1 = dFdx(uv);
//        vec2 st2 = dFdy(uv);
//
//        vec3 N = normalize(normal);
//        vec3 T = normalize(Q1*st2.t - Q2*st1.t);
//        vec3 B = -normalize(cross(N, T));
//        TBN = mat3(T, B, N);
//
//        normal = normalize(TBN * tangentNormal);

        // if tangent, bitangent already calculated
        normal = texture(material[getId()].normalSlot, uv).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    }

    float metallic = material[getId()].metallic;
    float roughness = material[getId()].roughness;
    float ao = material[getId()].ao;

    if (material[getId()].enableMetallicMap) {
        metallic *= texture(material[getId()].metallicSlot, uv).r;
    }

    if (material[getId()].enableRoughnessMap) {
        roughness *= texture(material[getId()].roughnessSlot, uv).r;
    }

    if (material[getId()].enableAOMap) {
        ao *= texture(material[getId()].aoSlot, uv).r;
    }

    vec3 Lo = vec3(0.0); // reflectance equation
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    for (uint i = 0 ; i < LIGHT_COUNT ; i++) {
        vec3 lightPos = light[i].position;
        lightPos *= TBN;
        vec3 lightDir = normalize(lightPos - pos);

        // calculate PBR surface model
        // calculate per-light radiance
        vec3 H = normalize(viewDir + lightDir);
        float distance = length(lightPos - pos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light[i].color.rgb * attenuation;

        // cook-torrance brdf
        float NDF = distributionGGX(normal, H, roughness);
        float G = geometrySmith(normal, viewDir, lightDir, roughness);
        vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        // add to outgoing radiance Lo
        float NdotL = max(dot(normal, lightDir), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    // result without HDR tone mapping and gamma correction
    fragment = vec4(color, material[getId()].color.w);

    pbrColor = vec4(metallic, roughness, ao, 1); // for troubleshooting

    float brightness = dot(fragment.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > material[getId()].brightness) {
        brightColor = vec4(fragment.rgb, 1);
    } else {
        brightColor = vec4(0, 0, 0, 1);
    }

    uuid = uuids[getId()];
}