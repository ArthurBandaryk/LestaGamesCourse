#include "engine.hxx"
#include "shader-programm.hxx"
//
#include <glog/logging.h>

//
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

class simple_shader : public arci::ishader_programm {
 public:
  arci::vertex get_vertex_transformed(
      const arci::vertex& v_in) const override {
    arci::vertex result = v_in;
    // Do nothing.
    return result;
  }

  arci::color get_color_transformed(
      [[maybe_unused]] const arci::vertex& v_in) const override {
    const unsigned char r = static_cast<unsigned char>(v_in.r);
    const unsigned char g = static_cast<unsigned char>(v_in.g);
    const unsigned char b = static_cast<unsigned char>(v_in.b);
    return arci::color{r, g, b};
  }
};

///////////////////////////////////////////////////////////////////////////////

class breath_shader : public arci::ishader_programm {
 public:
  arci::vertex get_vertex_transformed(
      const arci::vertex& v_in) const override {
    arci::vertex result{v_in};

    constexpr float circle_radius{200.f};
    const float circle_center_x{m_uniform.u0};
    const float circle_center_y{m_uniform.u1};

    float delta_x{1.f};
    float delta_y{1.f};
    float new_x{}, new_y{};
    static bool is_reducing{false};

    const float distance_vertex_circle_center =
        std::sqrt(
            std::pow(v_in.x - circle_center_x, 2.f)
            + std::pow(v_in.y - circle_center_y, 2.f));

    if (distance_vertex_circle_center > circle_radius) {
      is_reducing = true;
    }

    if (distance_vertex_circle_center < 5.f) {
      is_reducing = false;
    }

    if (v_in.x < circle_center_x) {
      delta_x *= -1.f;
    }
    if (v_in.y < circle_center_y) {
      delta_y *= -1.f;
    }

    if (is_reducing) {
      delta_x *= -1.f;
      delta_y *= -1.f;
    }

    if (std::abs(v_in.x - circle_center_x) == 0) {
      new_x = v_in.x;
      new_y = v_in.y + delta_y;
    } else if (std::abs(v_in.y - circle_center_y) == 0) {
      new_x = v_in.x + delta_x;
      new_y = v_in.y;
    } else {
      new_x = v_in.x + delta_x;
      new_y = (new_x - circle_center_x)
              * (v_in.y - circle_center_y) / (v_in.x - circle_center_x)
          + circle_center_y;
    }

    result.x = new_x;
    result.y = new_y;

    return result;
  }

  arci::color get_color_transformed(
      [[maybe_unused]] const arci::vertex& v_in) const override {
    return arci::color{
        static_cast<unsigned char>(v_in.r),
        static_cast<unsigned char>(v_in.g),
        static_cast<unsigned char>(v_in.b),
    };
  }
};

///////////////////////////////////////////////////////////////////////////////

constexpr float triangle_height{100.f};
constexpr float triangle_base{100.f};
constexpr float center_circle_x{399.f};
constexpr float center_circle_y{299.f};

// A simple interpolated triangle.
static std::vector<arci::vertex> triangle_vertices1{
    {34.f, 54.f, 0.f, 0.f, 255.f},
    {675.f, 134.f, 0.f, 255.f, 0.f},
    {213.f, 546.f, 255.f, 0.f, 0.f},
};

static const std::vector<uint32_t> triangle_indices{0, 1, 2};

// A simple triangle for breathing.
static std::vector<arci::vertex> triangle_vertices2{
    {center_circle_x, center_circle_y - triangle_height / 2, 0.f, 0.f, 255.f},
    {center_circle_x - triangle_base / 2, center_circle_y + triangle_height / 2, 0.f, 255.f, 0.f},
    {center_circle_x + triangle_base / 2, center_circle_y + triangle_height / 2, 255.f, 0.f, 0.f},
};

// A star figure from 6 triangles.
static std::vector<arci::vertex> star_vertices{
    {
        center_circle_x,
        center_circle_y - triangle_base / 2 - triangle_height,
        0.f,
        0.f,
        255.f,
    },
    {
        center_circle_x - triangle_base / 2.f,
        center_circle_y - triangle_height / 2,
        0.f,
        255.f,
        0.f,
    },
    {
        center_circle_x + triangle_base / 2.f,
        center_circle_y - triangle_height / 2,
        0.f,
        255.f,
        0.f,
    },
    {
        center_circle_x - triangle_base / 2.f - triangle_height,
        center_circle_y,
        255.f,
        0.f,
        0.f,
    },
    {
        center_circle_x - triangle_base / 2.f,
        center_circle_y + triangle_base / 2.f,
        255.f,
        255.f,
        50.f,
    },
    {
        center_circle_x + triangle_base / 2.f,
        center_circle_y + triangle_base / 2.f,
        255.f,
        255.f,
        50.f,
    },
    {
        center_circle_x + triangle_base / 2.f + triangle_height,
        center_circle_y,
        255.f,
        0.f,
        0.f,
    },
    {
        center_circle_x,
        center_circle_y + triangle_base / 2.f + triangle_height,
        0.f,
        0.f,
        255.f,
    },
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

  std::array<std::unique_ptr<arci::ishader_programm>, 3> shaders{
      std::make_unique<simple_shader>(),
      std::make_unique<breath_shader>(),
      std::make_unique<breath_shader>(),
  };

  std::array<std::vector<arci::vertex>, 3> vertices{
      triangle_vertices1,
      triangle_vertices2,
      star_vertices,
  };

  std::array<std::vector<uint32_t>, 3> indices{
      triangle_indices,
      triangle_indices,
      star_indices,
  };

  shaders.at(1)->set_uniform(arci::uniform{center_circle_x, center_circle_y});
  shaders.at(2)->set_uniform(arci::uniform{center_circle_x, center_circle_y});

  bool loop_continue{true};
  std::size_t current_shader{};

  while (loop_continue) {
    arci::event event{};

    while (engine->process_input(event)) {
      if (event.is_quitting) {
        loop_continue = false;
        break;
      } else {
        if (event.keyboard_info) {
          if (*event.keyboard_info
              == arci::keyboard_event::right_button_pressed) {
            ++current_shader;
            current_shader %= 3;
          }
        }
      }
    }

    process_vertices_with_shader(
        vertices.at(current_shader),
        shaders.at(current_shader).get());

    engine->render(vertices.at(current_shader), indices.at(current_shader));
  }

  engine->uninit();

  return EXIT_SUCCESS;
}
