# CreateDevice

```{literalinclude} ../../samples/CreateDevice/gorilla/device.cpp
:language: cpp
:caption:
```

* D3D11_CREATE_DEVICE_DEBUG があるとアプリケーション終了時に COM の Release 忘れが Console に表示される

`#include <wrl/client.h>` で `Microsoft::WRL::ComPtr<T>` を使うのが楽。

* D3D11_CREATE_DEVICE_BGRA_SUPPORT は、D2D デバイスを作る場合は必要
