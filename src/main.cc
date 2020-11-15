#include "fps_game.hpp"
#include <OgreRoot.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  try {
    fpsgame::FpsGame app;
    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();
  } catch (const std::exception& e) {
    std::cerr << "Error occurred during execution: " << e.what() << '\n';
    return 1;
  }

  return 0;
}

//! [starter]
