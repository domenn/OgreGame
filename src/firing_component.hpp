#pragma once
#include <OgreSceneNode.h>

namespace fpsgame {
class FpsGame;

class FiringComponent {
  FpsGame* game_;
  float bullet_speed_{14.8f};
  float gravity_spped_{-0.011f};
  Ogre::SceneNode* bullet_node_{};
  Ogre::Entity* bullet_physical_item_{};

 public:


  FiringComponent(FpsGame* game);

  void fire();
  void frame();
  void move_bullet();
  float* bullet_speed() { return &bullet_speed_; }
  float* gravity_speed() { return &gravity_spped_; }

  void remove_bullet_from_screen();
  
  bool collision(const Ogre::Entity* item) const;
};
}  // namespace fpsgame