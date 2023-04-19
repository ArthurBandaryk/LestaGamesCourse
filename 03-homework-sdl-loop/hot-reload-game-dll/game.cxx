#include "engine.hxx"

//
#include <glog/logging.h>

//
#include <algorithm>
#include <chrono>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace arci {

///////////////////////////////////////////////////////////////////////////////

struct target_rectangle {
  float left{};
  float top{};
  float width{};
  float height{};
  unsigned char r{};
  unsigned char g{};
  unsigned char b{};
  unsigned char a{};
};

///////////////////////////////////////////////////////////////////////////////

class aim_target_shooting_game final : public igame {
 public:
  aim_target_shooting_game(const aim_target_shooting_game&) = delete;
  aim_target_shooting_game(aim_target_shooting_game&&) = delete;
  aim_target_shooting_game& operator=(
      const aim_target_shooting_game&) = delete;
  aim_target_shooting_game& operator=(aim_target_shooting_game&&) = delete;

  aim_target_shooting_game(iengine* engine)
    : m_engine{engine} {
    srand(time(NULL));

    // Uncomment this code for hot-reload mechanism demostration.
    // constexpr size_t k_target_width{40}, k_target_height{40};

    // constexpr unsigned char r{255}, g{255}, b{50}, a{255};

    // Comment this code for hot-reload mechanism demostration.
    constexpr size_t k_target_width{100}, k_target_height{100};

    constexpr unsigned char r{204}, g{0}, b{102}, a{255};

    m_target_rectangle.width = k_target_width;
    m_target_rectangle.height = k_target_height;
    m_target_rectangle.r = r;
    m_target_rectangle.g = g;
    m_target_rectangle.b = b;
    m_target_rectangle.a = a;
    update_target_coordinates();
  }

  ///////////////////////////////////////////////////////////////////////////////

  void process_event(const event& event) override {
    // Process keyboard events.
    if (event.device == event_from_device::keyboard && event.keyboard_info) {
      switch (*(event.keyboard_info)) {
        case keyboard_event::escape_button_pressed:
          m_is_game_over = true;
          break;

          // TODO(arci): add processing other keyboard keys.
      }
    }
    // Process mouse events.
    else if (event.device == event_from_device::mouse && event.mouse_info) {
      LOG(INFO) << event.mouse_info->x << " " << event.mouse_info->y;

      // Check if user hit the target.
      const float mouse_x = event.mouse_info->x;
      const float mouse_y = event.mouse_info->y;
      if (mouse_x >= m_target_rectangle.left
          && mouse_x <= m_target_rectangle.left + m_target_rectangle.width) {
        if (mouse_y >= m_target_rectangle.top
            && mouse_y <= m_target_rectangle.top + m_target_rectangle.height) {
          // User hit successfully the target. So update it's coordinates.
          update_target_coordinates();
        }
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////////

  void update() override {
    const auto current_time = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds =
        current_time - m_time_target_spawn;

    // Bruuuuh, your time reaction is so bad. You've got a miss.
    if (elapsed_seconds.count() > m_delta_time_respawn) {
      ++m_miss_counter;
      if (m_miss_counter > m_max_allowed_misses) {
        // GG.
        LOG(INFO) << "Game over loser";
        m_is_game_over = true;
        return;
      }

      update_target_coordinates();
    }
  }

  ///////////////////////////////////////////////////////////////////////////////

  void render() override {
    CHECK_NOTNULL(m_engine);

    m_engine->render_rectangle(
        m_target_rectangle.left,
        m_target_rectangle.top,
        m_target_rectangle.width,
        m_target_rectangle.height,
        m_target_rectangle.r,
        m_target_rectangle.g,
        m_target_rectangle.b,
        m_target_rectangle.a);
  }

  ///////////////////////////////////////////////////////////////////////////////

  bool is_game_over() override {
    return m_is_game_over;
  }

  ///////////////////////////////////////////////////////////////////////////////

 private:
  void update_target_coordinates() {
    // Get random value for range [min, max].
    // int num = min + rand() % (max - min + 1);
    // The idea is to generate random coordinates for target
    // which do not exceed window's resolution (1024 x 768).
    // min_x = 0
    // max_x = x_max_allowed_target_coordinate - target_width
    // min_y = 0
    // max_y = y_max_allowed_target_coordinate - target_height

    /* clang-format off */
    m_target_rectangle.left =
        static_cast<float>(
            rand()
            % (x_max_allowed_target_coordinate - 
            static_cast<size_t>(m_target_rectangle.width) + 1));

    
    m_target_rectangle.top =
        static_cast<float>(
            rand()
            % (y_max_allowed_target_coordinate - 
            static_cast<size_t>(m_target_rectangle.height) + 1));
    /* clang-format on */

    m_time_target_spawn = std::chrono::system_clock::now();
  }

  ///////////////////////////////////////////////////////////////////////////////

  iengine* m_engine{nullptr};

  // Time when the target was spawn/respawn. This value will be updated
  // when user will hit the target or m_delta_time_respawn will exceed.
  std::chrono::time_point<std::chrono::system_clock> m_time_target_spawn;
  target_rectangle m_target_rectangle{};

  const size_t x_max_allowed_target_coordinate{1024};
  const size_t y_max_allowed_target_coordinate{768};

  // Time reaction for user to try to hit target. If the delta
  // specified will expire => the current target will update
  // it's coordinates and the `m_miss_counter` value will be
  // changed by '+1'.
  float m_delta_time_respawn{2.f};
  size_t m_miss_counter{};
  const size_t m_max_allowed_misses{20};
  bool m_is_game_over{false};
};

///////////////////////////////////////////////////////////////////////////////

igame* create_game(iengine* engine) {
  CHECK(engine) << "Invalid state for engine when creaing game";
  return new aim_target_shooting_game{engine};
}

///////////////////////////////////////////////////////////////////////////////

void destroy_game(igame* game) {
  CHECK(game) << "Invalid state for game when destroying it";
  delete game;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace arci
