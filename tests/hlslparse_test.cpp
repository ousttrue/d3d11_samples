#define CATCH_CONFIG_MAIN
#include <banana/asset.h>
#include <catch.hpp>
#include <gorilla/dxsas.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>
#include <iostream>

TEST_CASE("grid", "[hlsl_parse]") {

  auto source = banana::get_string("grid.hlsl");
  REQUIRE(!source.empty());
  auto [compiled, error] = gorilla::compile_gs("gs", source, "gsMain", {});
  if (!compiled) {
    std::cerr << (const char *)error->GetBufferPointer() << std::endl;
  }
  REQUIRE(compiled);
  gorilla::ShaderReflection reflection;
  REQUIRE(reflection.reflect(compiled));

  gorilla::DXSAS dxsas;
  dxsas.parse(source);

  auto found = dxsas.find("CAMERA_VIEW");
  REQUIRE(found->name == "b0View");
  REQUIRE(found->type == "float4x4");
  REQUIRE(found->line == 16);
}

TEST_CASE("gltf", "[hlsl_parse]") {
  auto source = banana::get_string("gltf.hlsl");
  REQUIRE(!source.empty());

  gorilla::DXSAS dxsas;
  dxsas.parse(source);

  auto found = dxsas.find("MATERIAL_COLOR", "Texture2D");
  REQUIRE(found);
  REQUIRE(found->name == "BaseColorTexture");
}
