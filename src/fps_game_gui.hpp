#pragma once
#include <imgui.h>

namespace Ogre {
class SceneManager;
}

namespace fpsgame {

class FpsGame;

class FpsGameGui {
  fpsgame::FpsGame* const game_;

  float camera_position_[3]{};
  float gun_position_[3]{0, -56, -360};
  float gun_rotation_[4]{0.772f, 0.083f, -0.63f, -0.021f};
  float gun_scale_{1.0f};
  bool settings_on_{false};

  void crosshair();

 public:
  bool experimental_obstacle_changed_{false};

  [[nodiscard]] const float* camera_position() const { return camera_position_; }
  [[nodiscard]] const float* gun_position() const { return gun_position_; }
  [[nodiscard]] const float* gun_rotation() const { return gun_rotation_; }

  void plane_tools();
  void info_text();
  void dbg_text();
  // void balistics();
  void frame_started();
  void setup_imgui(Ogre::SceneManager* scene_manager);
  void flip_settings() { settings_on_ = !settings_on_; }

  FpsGameGui(fpsgame::FpsGame* fps_game_inst);
};

}  // namespace fpsgame
