#pragma once
#include "firing_component.hpp"
#include "fps_game_gui.hpp"
#include "frames_per_second.hpp"
#include "obstacle_system.hpp"
#include <OgreApplicationContext.h>
#include <OgreRenderTargetListener.h>
#include <memory>

namespace OgreBites {
class CameraMan;
class ImGuiInputListener;
}  // namespace OgreBites

namespace fpsgame {

struct PlaneParameters {
  int plane_uv{18};
  int num_tex_coord_sets{1};
  int xy_segments{2};
  int wh{16000};
};

class Improved2dAxisAlignedBox : public Ogre::AxisAlignedBox {
 public:
  // enum class IntersectsWhere { N, E, S, W };

  fpsgame::Improved2dAxisAlignedBox& operator=(AxisAlignedBox&& axis_aligned_box) noexcept {
    Ogre::AxisAlignedBox::operator=(std::move(axis_aligned_box));
    return *this;
  }

  // We simplify further by limiting player to +x half and obstacles to +x half
  void clip_scene_node_inside_plus_x(Ogre::SceneNode* receiver) const;
};

class FpsGame : public OgreBites::ApplicationContext, public OgreBites::InputListener {
  friend class FpsGameGui;
  friend class GameSetupComponent;
  friend class FiringComponent;
  friend class ObstacleSystem;

  // Waste of memory. But who cares, this is a practice assignment, and PCs have "limitless" anyway ;)
  bool key_latch_['w' + 1]{};

  float move_speed_{1080.f};
  float frame_time_{0.008333f};
  float player_pitch_{0};
  float player_yaw_{0};
  float player_roll_{0};
  bool mouse_grab_{true};
  int score_{0};

  FpsGameGui fps_game_gui_{this};
  FiringComponent firing_component_{this};
  ObstacleSystem obstacle_system_{this};
  FramesPerSecond frames_per_second_{};
  Ogre::Log* log_{};

  Ogre::Entity* player_entity_{};
  Ogre::Camera* the_only_camera_{};
  Ogre::SceneNode* the_only_camera_node_{};
  std::unique_ptr<OgreBites::CameraMan> camera_man_{};
  std::unique_ptr<Ogre::OverlaySystem> overlay_system_{};
  std::unique_ptr<OgreBites::ImGuiInputListener> imgui_listener_{};
  Ogre::SceneNode* gun_node_can_be_positioned_{};
  Ogre::SceneManager* scn_mgr_{};
  Ogre::SceneNode* plane_node_{};
  Ogre::Entity* plane_entity_{};
  PlaneParameters plane_parameters_{};
  Improved2dAxisAlignedBox bounding_box_playing_field_{};

 public:
  FpsGame();
  ~FpsGame() override;

  void setup() override;

  bool frameStarted(const Ogre::FrameEvent& evt) override;
  // void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) override;

  bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
  bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
  bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
  bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;
  bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt) override;
  bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;
  bool keyReleased(const OgreBites::KeyboardEvent& evt) override;
  void recreate_plane();
  void move_player(Ogre::Vector3&& direction);
  void wasd_move();

  static void make_forward(Ogre::Vector3& v);
  void move_within_xz(Ogre::SceneNode* node, Ogre::Vector3&& raw_vector);
  void generic_remove(Ogre::SceneNode* node, Ogre::Entity* entity);
  // bool frameStarted(const Ogre::FrameEvent& evt) override;
};
}  // namespace fpsgame
