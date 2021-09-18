# CreateDevice

```{literalinclude} ../../lib/gorilla/device.cpp
:language: cpp
:caption:
```

## CreateDevice flag
* `D3D11_CREATE_DEVICE_DEBUG` があるとアプリケーション終了時に COM の Release 忘れが Console に表示される
* `D3D11_CREATE_DEVICE_BGRA_SUPPORT` は、D2D デバイスを作る場合は必要

## Microsoft::WRL::ComPtr<T>

`#include <wrl/client.h>`
