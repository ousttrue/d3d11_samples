#include <gorilla/asset.h>
#include <gorilla/gltf.h>

int main(int argc, char **argv) {

  auto bytes = gorilla::assets::get_bytes(
      "glTF-Sample-Models/2.0/Box/glTF-Binary/Box.glb");
  if (bytes.empty()) {
    return 1;
  }

  gorilla::gltf::Glb glb;
  if (!glb.parse(bytes)) {
    return 2;
  }

  gorilla::gltf::GltfLoader loader(glb.json, glb.bin);
  if(!loader.load())
  {
    return 3;
  }

  return 0;
}
