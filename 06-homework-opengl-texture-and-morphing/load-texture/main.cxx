#include "engine.hxx"

//
#include <glog/logging.h>

//
#include <memory>

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

    arci::triangle triangle_low {
        { -0.5f, -0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f },
        { 0.5f, -0.5f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f },
        { -0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f },
    };

    arci::triangle triangle_high {
        { -0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f },
        { 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f },
        { 0.5f, -0.5f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f },
    };

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

        engine->render(triangle_low);
        engine->render(triangle_high);

        engine->swap_buffers();
    }

    engine->uninit();

    return EXIT_SUCCESS;
}
