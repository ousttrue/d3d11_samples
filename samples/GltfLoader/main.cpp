#include <banana/asset.h>
#include <banana/gltf.h>
#include <banana/glb.h>

int main(int argc, char **argv) {

  auto bytes = banana::get_bytes(
      "glTF-Sample-Models/2.0/Box/glTF-Binary/Box.glb");
  if (bytes.empty()) {
    return 1;
  }

  banana::gltf::Glb glb;
  if (!glb.parse(bytes)) {
    return 2;
  }

  banana::gltf::GltfLoader loader(glb.json, glb.bin);
  if(!loader.load())
  {
    return 3;
  }

  return 0;
}
