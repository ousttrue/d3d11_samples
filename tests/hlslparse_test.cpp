#define CATCH_CONFIG_MAIN
#include <banana/asset.h>
#include <catch.hpp>
#include <gorilla/dxsas.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>
#include <iostream>

TEST_CASE("grid", "[hlsl_parse]") {

  auto source = banana::get_asset("grid.hlsl");
  REQUIRE(source);
  auto [compiled, error] = gorilla::compile_gs("gs", source, "gsMain", {});
  if (!compiled) {
    std::cerr << (const char *)error->GetBufferPointer() << std::endl;
  }
  REQUIRE(compiled);
  gorilla::ShaderReflection reflection;
  REQUIRE(reflection.reflect(compiled));

  gorilla::DXSAS dxsas;
  dxsas.parse(source);

  auto found = dxsas.find(banana::Semantics::CAMERA_VIEW);
  REQUIRE(found->name == "b0View");
  REQUIRE(found->type == "float4x4");
  REQUIRE(found->line == 16);
}

TEST_CASE("gltf", "[hlsl_parse]") {
  auto source = banana::get_asset("gltf.hlsl");
  REQUIRE(source);

  gorilla::DXSAS dxsas;
  dxsas.parse(source);

  auto found = dxsas.find(banana::Semantics::MATERIAL_COLOR, "Texture2D");
  REQUIRE(found);
  REQUIRE(found->name == "BaseColorTexture");
}

auto NEST = R"(
MVP
)";

auto INCLUDE = R"(
float4 
#include "nest.inc"
: WORLDVIEWPROJECTION;
)";

TEST_CASE("include", "[hlsl_parse]") {
  auto source = banana::Asset::from_string(INCLUDE);
  source->get = [](const char *path)
  {
    return std::span{(const uint8_t*)NEST, strlen(NEST)};
  };

  gorilla::DXSAS dxsas;
  dxsas.parse(source);

  auto found = dxsas.find(banana::Semantics::WORLDVIEWPROJECTION);
  REQUIRE(found);
  REQUIRE(found->name == "MVP");
}
