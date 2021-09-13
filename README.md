# D3D11 Samples

<https://ousttrue.github.io/d3d11_samples/>

* 履歴を破棄して作り直し
* d3d11 の sample コードと簡単な説明

## TODO

* [x] ConstantBuffer
* [x] Camera(Projection)
* [x] MouseInput
* [x] Texture
* [x] DepthBuffer
* [ ] glTF
* [ ] ImageLoader
* [ ] imgui
* [ ] MikkTSpace
* [ ] skinning
* [ ] morph target
* [ ] Animation: BVH
* [ ] CubeMap
* [ ] IBL
* [ ] AsciiTexture によるデバッグ表示 <https://nothings.org/stb/font/>
* [ ] Gizmo: Grid
* [ ] Gizmo: drag
* [ ] Gizmo: wire frame
* [ ] Gizmo: BoundingBox
* [ ] Gizmo: T
* [ ] Gizmo: R
* [ ] Gizmo: S
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

### actions

* [actions](./.github/workflows/sphinx.yml)
