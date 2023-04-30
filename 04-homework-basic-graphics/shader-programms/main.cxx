#include "engine.hxx"
#include "shader-programm.hxx"
//
#include <glog/logging.h>

//
#include <algorithm>
#include <memory>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

class star_shader : public arci::ishader_programm {
 public:
  arci::vertex get_vertex_transformed(
      const arci::vertex& v_in) const override {
    return v_in;
  }
  arci::color get_color_transformed(
      const arci::vertex& v_in) const override {
    return arci::color{
        static_cast<unsigned char>(v_in.r),
        static_cast<unsigned char>(v_in.g),
        static_cast<unsigned char>(v_in.b),
    };
  }
};

///////////////////////////////////////////////////////////////////////////////

// A star figure from 6 triangles.
static std::vector<arci::vertex> star_vertices{
    {150.f, 0.f, 0.f, 0.f, 255.f},
    {100.f, 100.f, 0.f, 255.f, 0.f},
    {200.f, 100.f, 0.f, 255.f, 0.f},
    {0.f, 150.f, 255.f, 0.f, 0.f},
    {100.f, 200.f, 255.f, 255.f, 50.f},
    {200.f, 200.f, 255.f, 255.f, 50.f},
    {300.f, 150.f, 255.f, 0.f, 0.f},
    {150.f, 300.f, 0.f, 0.f, 255.f},
};

static const std::vector<uint32_t> star_indices{
    // Left triangle.
    3,
    1,
    4,
    // Upper triangle.
    0,
    2,
    1,
    // Middle low triangle.
    1,
    5,
    4,
    // Middle high triangle.
    1,
    2,
    5,
    // Right triangle.
    2,
    6,
    5,
    // Low triangle.
    4,
    5,
    7,
};

static void process_vertices_with_shader(
    std::vector<arci::vertex>& vertices,
    arci::ishader_programm* shader_programm) {
  std::for_each(
      vertices.begin(),
      vertices.end(),
      [shader_programm](arci::vertex& v) {
        v = shader_programm->get_vertex_transformed(v);
        const arci::color c = shader_programm->get_color_transformed(v);
        v.r = static_cast<float>(c.r);
        v.g = static_cast<float>(c.g);
        v.b = static_cast<float>(c.b);
      });
}

///////////////////////////////////////////////////////////////////////////////

int main(int, char** argv) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  // Create engine.
  std::unique_ptr<arci::iengine, void (*)(arci::iengine*)> engine{
      arci::engine_create(),
      arci::engine_destroy};

  engine->init();

  std::unique_ptr<arci::ishader_programm> shader_programm =
      std::make_unique<star_shader>();

  shader_programm->set_uniform(arci::uniform{0.f, 0.f});

  bool loop_continue{true};

  while (loop_continue) {
    arci::event event{};

    while (engine->process_input(event)) {
      if (event.is_quitting) {
        loop_continue = false;
        break;
      } else {
        if (event.keyboard_info) {
          if (event.keyboard_info.value() == arci::keyboard_event::left_button_pressed) {
            // TODO(arci): change shaders on any key pressed event.
          }
        }
        process_vertices_with_shader(star_vertices, shader_programm.get());
      }
    }

    engine->render(star_vertices, star_indices);
  }

  engine->uninit();

  return EXIT_SUCCESS;
}
