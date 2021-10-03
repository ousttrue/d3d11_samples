# D3D11 Samples

<https://ousttrue.github.io/d3d11_samples/>

* 履歴を破棄して作り直し
* d3d11 の sample コードと簡単な説明

## lib

### 🦍 gorilla(HWND d3d11)
* Thin wrapper

### 🍌 banana(asset scene)
* glTF loader
* Node tree
* Camera

## TODO

* [x] ConstantBuffer
* [x] Camera(Projection)
* [x] MouseInput
* [x] Texture
* [x] DepthBuffer
* [x] glTF
* [x] ImageLoader
* [x] MikkTSpace
* [ ] skinning
* [ ] morph target
* [ ] Animation: BVH
* [ ] CubeMap
* [ ] IBL
* [x] AsciiTexture <https://evanw.github.io/font-texture-generator/>
* [ ] ScreenGizmo: drag
* [x] Gizmo: Grid
* [ ] Gizmo: wire frame
* [ ] Gizmo: BoundingBox
* [x] Gizmo: T
* [x] Gizmo: R
* [x] Gizmo: S
* [ ] Gizmo: point select
* [ ] Gizmo: edge select
* [ ] Gizmo: face select

## sphinx
### init

* pip install sphinx
* mkdir docs
* cd docs
* sphinx-quickstart
* rm Makefile, make.bat
* task: docs$ sphinx-build . ../public

### customize

```
$ pip install --upgrade myst-parser
$ pip install sphinx-autobuild
```

```py
# conf.py
extensions = ['myst_parser']
```

### actions

* [actions](./.github/workflows/sphinx.yml)
