# Physically Based Rendering

<https://learnopengl.com/PBR/Lighting>

## KHRONOS
比較的シンプルな古いバージョンらしい。
<https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/glTF-WebGL-PBR/shaders/pbr-frag.glsl>

```glsl
// Calculation of analytical lighting contribution
vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
// Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
vec3 color = NdotL * u_LightColor * (diffuseContrib + specContrib);
```
