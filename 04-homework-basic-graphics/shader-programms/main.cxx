#include "engine.hxx"
#include "shader-programm.hxx"
//
#include <glog/logging.h>

//
#include <memory>

int main(int, char** argv) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  // Create engine.
  std::unique_ptr<arci::iengine, void (*)(arci::iengine*)> engine{
      arci::engine_create(),
      arci::engine_destroy};

  engine->init();

  bool loop_continue{true};

  while (loop_continue) {
    arci::event event{};

    while (engine->process_input(event)) {
      if (event.is_quitting) {
        loop_continue = false;
        break;
      } else {
        // game->process_event(event);
      }
    }

    // game->update();

    // game->render();
  }

  engine->uninit();

  return EXIT_SUCCESS;
}
