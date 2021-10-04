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

TEST_CASE("lexer", "[hlsl_parser]") {
  auto source = banana::Asset::from_string(R"(
row_major float4x4 MVP: WORLDVIEWPROJECTION;
)");

  gorilla::hlsl::Lexer lexer(source);
  auto list = lexer.list();
  gorilla::hlsl::Token result[] = {
      {gorilla::hlsl::TokenTypes::Symbol, "row_major"},
      {gorilla::hlsl::TokenTypes::Symbol, "float4x4"},
      {gorilla::hlsl::TokenTypes::Symbol, "MVP"},
      {gorilla::hlsl::TokenTypes::Colon},
      {gorilla::hlsl::TokenTypes::Symbol, "WORLDVIEWPROJECTION"},
      {gorilla::hlsl::TokenTypes::Semicolon},
      {gorilla::hlsl::TokenTypes::End},
  };
  REQUIRE(list.size() == _countof(result));
  REQUIRE(std::equal(list.begin(), list.end(), result));
}

TEST_CASE("include", "[hlsl_parse]") {
  static auto NEST = R"(
MVP
)";

  static auto INCLUDE = R"(
float4x4
#include "nest.inc"
: WORLDVIEWPROJECTION;
)";

  auto source = banana::Asset::from_string(INCLUDE);
  auto nest = banana::Asset::from_string(NEST);
  source->get = [nest](auto) { return nest; };

  gorilla::hlsl::Lexer lexer(source);
  auto list = lexer.list();

  gorilla::hlsl::Token result[] = {
      {gorilla::hlsl::TokenTypes::Symbol, "float4x4"},
      {gorilla::hlsl::TokenTypes::Symbol, "MVP"},
      {gorilla::hlsl::TokenTypes::Colon},
      {gorilla::hlsl::TokenTypes::Symbol, "WORLDVIEWPROJECTION"},
      {gorilla::hlsl::TokenTypes::Semicolon},
      {gorilla::hlsl::TokenTypes::End},
  };
  REQUIRE(list.size() == _countof(result));
  REQUIRE(std::equal(list.begin(), list.end(), result));
}
