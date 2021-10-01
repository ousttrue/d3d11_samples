#pragma once

#include <banana/node.h>
#include <banana/scene_processor.h>
#include <banana/types.h>
#include <d3d11.h>
#include <gorilla/drawable.h>
#include <gorilla/input_assembler.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/texture.h>
#include <gorilla/window.h>
#include <memory>
#include <span>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

class Renderer {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  banana::SceneProcessor _processor;

  std::unordered_map<std::shared_ptr<banana::Image>,
                     std::shared_ptr<gorilla::Texture>>
      _texture_map;
  std::unordered_map<std::shared_ptr<banana::Material>,
                     std::shared_ptr<gorilla::Pipeline>>
      _material_map;

  std::unordered_map<banana::MaterialStates, std::shared_ptr<gorilla::State>>
      _state_map;
  std::unordered_map<std::shared_ptr<banana::Mesh>,
                     std::shared_ptr<gorilla::InputAssembler>>
      _mesh_map;

  std::shared_ptr<gorilla::InputAssembler> _ia;
  std::shared_ptr<gorilla::Pipeline> _pipeline;
  std::shared_ptr<gorilla::State> _state;

  struct TextureList {
    std::vector<ID3D11ShaderResourceView *> srv;
    std::vector<ID3D11SamplerState *> sampler;

    void clear(const gorilla::ShaderReflection &r) {
      srv.clear();
      srv.resize(r.srv_slots.size());

      sampler.clear();
      sampler.resize((r.sampler_slots.size()));
    }
  };
  TextureList _vs_list;
  TextureList _gs_list;
  TextureList _ps_list;

public:
  std::shared_ptr<gorilla::Texture>
  get_or_create(const ComPtr<ID3D11Device> &device,
                const std::shared_ptr<banana::Image> &src);
  std::shared_ptr<gorilla::Pipeline>
  get_or_create(const ComPtr<ID3D11Device> &device,
                const std::shared_ptr<banana::Material> &src);
  std::shared_ptr<gorilla::State>
  get_or_create(const ComPtr<ID3D11Device> &device,
                const banana::MaterialStates &src);
  std::shared_ptr<gorilla::InputAssembler>
  get_or_create(const ComPtr<ID3D11Device> &device,
                const std::shared_ptr<banana::Mesh> &src);

  void render(const ComPtr<ID3D11Device> &device,
              const ComPtr<ID3D11DeviceContext> &context,
              const std::shared_ptr<banana::Node> &root,
              const banana::OrbitCamera *camera,
              std::span<const banana::LightInfo> lights = {});

private:
  void draw(const ComPtr<ID3D11Device> &device,
            const ComPtr<ID3D11DeviceContext> &context,
            const banana::Command &command);
};
