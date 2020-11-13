#pragma once
#include <OgreApplicationContext.h>
#include <OgreRenderTargetListener.h>
#include <memory>

namespace OgreBites {
class CameraMan;
class ImGuiInputListener;
}  // namespace OgreBites

namespace fpsgame {
class FpsGame : public OgreBites::ApplicationContext, public OgreBites::InputListener {
  float move_speed_{2.0f};
  bool mouse_grab_{true};

  float camera_position_[3]{};
  float gun_position_[3]{0, -56, -360};
  float gun_rotation_[4]{0.772f, 0.083f, -0.63f, 0.0f};
  float experimental_position_[3]{0.0f, 1.f, 0.0f};
  float gun_scale_{1.0f};

  Ogre::Log* log_{};

  Ogre::Entity* player_entity_{};
  Ogre::Camera* the_only_camera_{};
  Ogre::SceneNode* the_only_camera_node_{};
  std::unique_ptr<OgreBites::CameraMan> camera_man_{};
  std::unique_ptr<Ogre::OverlaySystem> overlay_system_{};
  std::unique_ptr<OgreBites::ImGuiInputListener> imgui_listener_{};
  Ogre::SceneNode* gun_node_can_be_positioned_{};

 public:
  FpsGame();
  ~FpsGame() override;

  void setup_tutorial();
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

  static void make_forward(Ogre::Vector3& v);
  void move_within_xz(Ogre::SceneNode* node, Ogre::Vector3&& raw_vector);
  void setup_imgui(Ogre::SceneManager* scene_manager);
  // bool frameStarted(const Ogre::FrameEvent& evt) override;
};
}  // namespace fpsgame
