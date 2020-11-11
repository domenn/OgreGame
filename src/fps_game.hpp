#pragma once
#include <OgreApplicationContext.h>
#include <memory>

namespace OgreBites {
class CameraMan;
}

namespace fpsgame {
class FpsGame : public OgreBites::ApplicationContext, public OgreBites::InputListener {
  Ogre::Log* log_{};

  Ogre::Entity* player_entity_{};
  Ogre::SceneNode* player_node_{};
  Ogre::Camera* the_only_camera_{};
  Ogre::SceneNode* the_only_camera_node_{};
  std::unique_ptr<OgreBites::CameraMan> camera_man_{};

 public:
  FpsGame();
  ~FpsGame() override;

  void setup() override;

  bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
  bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
  bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
  bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;
  bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;
  bool keyReleased(const OgreBites::KeyboardEvent& evt) override;
  // bool frameStarted(const Ogre::FrameEvent& evt) override;
};
}  // namespace fpsgame
