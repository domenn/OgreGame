#pragma once
#include <OgreSceneNode.h>

namespace fpsgame {
class FpsGame;

class FiringComponent {
  FpsGame* game_;
  float bullet_speed_{4.8f};
  float gravity_spped_{-0.011f};
  Ogre::SceneNode* bullet_node_{};
  Ogre::Entity* bullet_physical_item_{};
  Ogre::SceneNode* obstacle_{};

  float obstacle_size_max_{2.f};
  float obstacle_size_min_{0.4f};
  Ogre::Entity* obstacle_physical_thing_{};

 public:
  float obstacle_height_experimental_{2.f};

  FiringComponent(FpsGame* game);

  void fire();
  void frame();
  void move_bullet();
  float* bullet_speed() { return &bullet_speed_; }
  float* gravity_speed() { return &gravity_spped_; }
  float* obstacle_size_max() { return &obstacle_size_max_; }
  float* obstacle_size_min() { return &obstacle_size_min_; }

  void remove_bullet_from_screen();
  void generic_remove(Ogre::SceneNode* node, Ogre::Entity* entity);
  void create_obstacle();
};
}  // namespace fpsgame