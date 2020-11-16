#pragma once
#include "firing_component.hpp"
#include <OgreSceneNode.h>

namespace fpsgame {
class FpsGame;

struct BalisticParameters {
  float theta;
  float cos_theta;
  float sin_theta;
  float time_elapsed;
  Ogre::Vector3 initial_position;
  Ogre::Vector3 xz_direction;
};

class FiringComponent {
  friend class FpsGameGui;

  FpsGame* game_;
  float bullet_speed_{24000.f};
  float gravity_{4000.f};
  BalisticParameters balistic_{};
  Ogre::SceneNode* bullet_node_{};
  Ogre::Entity* bullet_physical_item_{};

 public:
  FiringComponent(FpsGame* game);

  void fire();
  void frame();
  void move_bullet();
  float* bullet_speed() { return &bullet_speed_; }
  float* gravity() { return &gravity_; }

  void remove_bullet_from_screen();

  bool collision(const Ogre::Entity* item) const;
  const Ogre::Vector3* bullet_position() const;
  Ogre::Vector3 bullet_center() const;
};
}  // namespace fpsgame