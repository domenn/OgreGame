#pragma once

namespace Ogre {
class SceneManager;
}

namespace fpsgame {

class FpsGame;

class FpsGameGui {
  fpsgame::FpsGame* const fps_game_inst_;

  float camera_position_[3]{};
  float gun_position_[3]{0, -56, -360};
  float gun_rotation_[4]{0.772f, 0.083f, -0.63f, -0.021f};
  float gun_scale_{1.0f};

  void crosshair();

 public:
  bool experimental_obstacle_changed_{false};

  [[nodiscard]] const float* camera_position() const { return camera_position_; }
  [[nodiscard]] const float* gun_position() const { return gun_position_; }
  [[nodiscard]] const float* gun_rotation() const { return gun_rotation_; }

  void plane_tools();
  void frame_started();
  void setup_imgui(Ogre::SceneManager* scene_manager);

  FpsGameGui(fpsgame::FpsGame* fps_game_inst);
};

}  // namespace fpsgame
