# DXSAS

`Direct X Standard Annotations and Semantics`

<https://docs.microsoft.com/en-us/windows/win32/direct3d9/dx9-graphics-reference-effects-dxsas>

廃れてしまった `D3D9` 時代の仕様なのだけど、便利なので実装を試みる。

* <https://docs.microsoft.com/en-us/windows/win32/direct3d9/data-binding>
## Implementations

* [Using SAS in FX and CgFX File Formats](https://www.nvidia.com/en-us/drivers/using-sas/)
    * PDF [Creating Real Shaders Creating Real Shaders in FX Composer](http://download.nvidia.com/developer/presentations/2005/GDC/Sponsored_Day/Writing_w_FXComposer.pdf)
* [MMEリファレンス](https://sites.google.com/site/mmereference/home)
    * [‎2-1. ジオメトリ変換](https://sites.google.com/site/mmereference/home/Annotations-and-Semantics-of-the-parameter/2-1-geometry-translation)
    * [2-2. ライトとマテリアル](https://sites.google.com/site/mmereference/home/Annotations-and-Semantics-of-the-parameter/2-2-lights-and-materials)
* [VVVV チュートリアル ジオメトリモーフィング GPU1](https://vvvv.org/documentation/%E3%83%81%E3%83%A5%E3%83%BC%E3%83%88%E3%83%AA%E3%82%A2%E3%83%AB-%E3%82%B8%E3%82%AA%E3%83%A1%E3%83%88%E3%83%AA%E3%83%A2%E3%83%BC%E3%83%95%E3%82%A3%E3%83%B3%E3%82%B0-gpu1)

* [Guidelines for Writing Effects](https://digitalrune.github.io/DigitalRune-Documentation/html/17442709-63e0-419c-abe8-00697ca4fc3a.htm)

## Bindings

```{literalinclude} ../lib/banana/semantics.h
:language: cpp
:caption:
```

```
+------+
|Camera|
+------+
|Light |
+------+

+--------+
|Object  |
+--------+
|Material|
+--------+

System
+----------------+
|Time            |
|Mouse           |
|RenderTargetSize|
+----------------+
```

## technique と pass

TODO:
