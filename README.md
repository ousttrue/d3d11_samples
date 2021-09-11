# D3D11 Samples

<http://ousttrue.github.io/d3d11_samples/>

* 履歴を破棄して作り直し
* d3d11 の sample コードと簡単な説明

## sphinx

* pip install sphinx
* mkdir docs
* cd docs
* sphinx-quickstart
* rm Makefile, make.bat
* task: docs$ sphinx-build . ../public

* [actions](./.github/workflows/sphinx.yml)

* [Sphinx で使える Markdown 方言 'MyST'](https://qiita.com/Tachy_Pochy/items/53866eea43d0ad93ea1d)

```
pip install --upgrade myst-parser
pip install groundwork-sphinx-theme
```
