# SceneGraph

glTF のローダーを作るので導入する。いつもの木構造。

* Node
  * Transform
    * Translation
    * Rotation
    * Scale
  * Mesh
    * SubMesh[]
      * Material
        * Texture

## ConstantBuffer

### World(VS|GS)

* Model: mat4
* View: mat4
* Projection: mat4
* NormalMatrix: mat3
* MVP: mat4
* Light

### Draw(PS)

* BaseColor
* BaseTexture
