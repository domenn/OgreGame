#pragma once
#include <OgreSceneNode.h>

namespace fpsgame {
class FpsGame;
class FiringComponent;

struct ObstacleOgrePtrs {
  Ogre::SceneNode* node_{};
  Ogre::Entity* physical_thing_{};
};

class ObstacleSystem {
  FpsGame* const game_;
  FiringComponent* const guns_;
  std::vector<ObstacleOgrePtrs> obstacles_{};

  float obstacle_size_max_{2.f};
  float obstacle_size_min_{0.4f};
  int max_obstacles_{24};
  int min_obstacles_{8};

  void create_obstacle();
  void remove_obstacle(std::vector<ObstacleOgrePtrs>::iterator obstacle);
  void make_obstacles();

 public:
  float obstacle_height_experimental_{2.f};
  ObstacleSystem(FpsGame* game);

  // Call after frame() of firing component, so bullet has already been moved.
  void frame();
  float* obstacle_size_max() { return &obstacle_size_max_; }
  float* obstacle_size_min() { return &obstacle_size_min_; }
  int* max_obstacles() { return &max_obstacles_; }
  int* min_obstacles() { return &min_obstacles_; }
  void setup();
};

}  // namespace fpsgame