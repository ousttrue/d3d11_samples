#define CATCH_CONFIG_MAIN
#include <banana/asset.h>
#include <catch.hpp>
#include <gorilla/dxsas.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>
#include <iostream>

class HlslParser {

public:
  void parse(std::string_view source) {}
};

TEST_CASE("hlsl_parse", "[hlsl]") {

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

  auto found = dxsas.semantics_map.find("CAMERA_VIEW");
  REQUIRE(found != dxsas.semantics_map.end());
  REQUIRE(found->second.name == "b0View");
  REQUIRE(found->second.type == "float4x4");
  REQUIRE(found->second.line == 16);
}
