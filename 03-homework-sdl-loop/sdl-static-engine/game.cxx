#include "engine.hxx"

//
#include <glog/logging.h>

//
#include <memory>

///////////////////////////////////////////////////////////////////////////////

int main(int, char** argv) {
  // Initialize google logging library.
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);

  // Create engine.
  std::unique_ptr<arci::iengine, void (*)(arci::iengine*)> engine{
      arci::engine_create(),
      arci::engine_destroy};

  engine->init();

  bool is_running{true};

  while (is_running) {
    arci::event_type event;
    while (engine->process_input(event)) {
      if (event == arci::event_type::exit
          || event == arci::event_type::escape_button_pressed) {
        is_running = false;
        break;
      }
    }
  }

  engine->uninit();

  return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
