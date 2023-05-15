#include "engine.hxx"
#include "input-data.hxx"
//
#include <glog/logging.h>

//
#include <chrono>
#include <cmath>
#include <fstream>
#include <memory>
#include <string_view>

class morphing_helper final
{
public:
    morphing_helper() = default;
    morphing_helper(const morphing_helper&) = delete;
    morphing_helper(morphing_helper&&) = delete;
    morphing_helper& operator=(const morphing_helper&) = delete;
    morphing_helper& operator=(morphing_helper&&) = delete;

    arci::triangle get_morphed_triangle(const arci::triangle& from,
                                        const arci::triangle& to,
                                        const float tau)
    {
        return arci::triangle(
            get_morphed_vertex(from.vertices.at(0), to.vertices.at(0), tau),
            get_morphed_vertex(from.vertices.at(1), to.vertices.at(1), tau),
            get_morphed_vertex(from.vertices.at(2), to.vertices.at(2), tau));
    }

    float get_tau() const
    {
        constexpr float pi = 3.14159;
        static auto start_time = std::chrono::steady_clock::now();
        const auto cur_time = std::chrono::steady_clock::now();
        const std::chrono::duration<float> elapsed_seconds
            = cur_time - start_time;
        const float dt = elapsed_seconds.count();
        const float real_sinus = std::sin(pi * dt);
        return (real_sinus + 1) / 2.f;
    }

private:
    arci::vertex get_morphed_vertex(const arci::vertex& from,
                                    const arci::vertex& to,
                                    const float tau)
    {
        arci::vertex result = from;
        float new_y = from.y + (to.y - from.y) * tau;
        result.y = new_y;
        return result;
    }
};

int main(int, char** argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    // Create engine.
    std::unique_ptr<arci::iengine, void (*)(arci::iengine*)> engine {
        arci::engine_create(),
        arci::engine_destroy
    };

    engine->init();

    morphing_helper helper;

    bool loop_continue { true };

    while (loop_continue)
    {
        arci::event event {};

        while (engine->process_input(event))
        {
            if (event.is_quitting)
            {
                loop_continue = false;
                break;
            }
            else
            {
                if (event.keyboard_info)
                {
                    // Do nothing for now.
                }
            }
        }

        const float tau = helper.get_tau();

        for (const arci::triangle& triangle : arci::triangles)
        {
            engine->render(helper.get_morphed_triangle(
                triangle, arci::line, tau));
        }

        engine->swap_buffers();
    }

    engine->uninit();

    return EXIT_SUCCESS;
}
