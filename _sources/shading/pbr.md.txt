# Physically Based Rendering

* [物理ベースレンダリング -基礎編-](https://tech.cygames.co.jp/archives/2129/)
* [物理ベースレンダリングを柔らかく説明してみる（４）](https://qiita.com/emadurandal/items/76348ad118c36317ec5c)
* <https://learnopengl.com/PBR/Lighting>

## BRDF



## Albedo
色の反射率

## Metallic
粘土 < 陶器 < 鏡

## Roughness
* 鏡面のぼやけ具合
* 艶消し

## Cook-Torrance

<http://www.codinglabs.net/article_physically_based_rendering_cook_torrance.aspx>

`fr=kdflambert+ksfcook−torrance`

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
