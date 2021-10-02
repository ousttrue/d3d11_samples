# Phong reflection model: ADS(Ambient Diffuse Specular)

* n: **n** ormal vector at the surface point.
* s: the direction from the surface point to light **s** ource.
* Ld: light intensity.
* Kd: surface diffuse reflection.

`Intensity = Ld x Kd (n dot s)`

## VertexADS(Ambient, Diffuse, Specular)

```{literalinclude} ../../assets/lighting/vertex_ads.hlsl
:language: hlsl
:caption:
```

## PixelADS(Ambient, Diffuse, Specular)
