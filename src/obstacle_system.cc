#include "obstacle_system.hpp"

#include "firing_component.hpp"
#include "fps_game.hpp"
#include "simple_random_generator.hpp"
#include <OgreEntity.h>
#include <OgreMaterial.h>
#include <OgreSubEntity.h>

using namespace fpsgame;

void ObstacleSystem::create_obstacle() {
  // Player is limited to +x and obstacles to -x
  static int obj_ctr{0};
  const auto coord_x = SimpleRandomGenerator::generate(game_->bounding_box_playing_field_.getMinimum().x, -1);
  const auto coord_z = SimpleRandomGenerator::generate(game_->bounding_box_playing_field_.getMinimum().z,
                                                       game_->bounding_box_playing_field_.getMaximum().z);
  const auto height = SimpleRandomGenerator::generate(obstacle_size_min_, obstacle_size_max_);

  auto& obstacle = obstacles_.emplace_back(ObstacleOgrePtrs{
      game_->scn_mgr_->getRootSceneNode()->createChildSceneNode(game_->the_only_camera_node_->getPosition()),
      game_->scn_mgr_->createEntity("cube" + std::to_string(obj_ctr), "cube.mesh")});
  const auto new_mat_name = "clonedMat" + std::to_string(obj_ctr++);
  auto material_ptr = Ogre::MaterialManager::getSingleton()
                          .getByName("Examples/Chrome", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME)
                          ->clone(new_mat_name);

  auto [cr, cg, cb] = std::make_tuple(SimpleRandomGenerator::generate(0.1f, 0.95f),
                                      SimpleRandomGenerator::generate(0.1f, 0.95f),
                                      SimpleRandomGenerator::generate(0.1f, 0.95f));
  material_ptr->setDiffuse(cr, cg, cb, 0);
  material_ptr->setAmbient(cr, cg, cb);

  obstacle.physical_thing_->setMaterial(material_ptr);

  obstacle.node_->attachObject(obstacle.physical_thing_);
  obstacle.node_->setScale(height, height, height);
  // Experimentally determined the number we have to multiply with to have it be perfectly on the ground.
  obstacle.node_->setPosition(coord_x, height * 50.f, coord_z);
}

void ObstacleSystem::remove_obstacle(std::vector<ObstacleOgrePtrs>::iterator obstacle) {
  // Shared ptr copy - I am aware of it.
  const auto material = obstacle->physical_thing_->getSubEntities()[0]->getMaterial();
  game_->generic_remove(obstacle->node_, obstacle->physical_thing_);
  obstacles_.erase(obstacle);
  Ogre::MaterialManager::getSingleton().remove(material);
}

void ObstacleSystem::make_obstacles() {
  const auto how_many =
      SimpleRandomGenerator::random_integer<int>(min_obstacles_, max_obstacles_+1) - static_cast<int>(obstacles_.size());
  game_->log_->logMessage("We have " + std::to_string(obstacles_.size()) + "; Generating " + std::to_string(how_many));
  for (int i = 0; i < how_many; ++i) {
    create_obstacle();
  }
}

ObstacleSystem::ObstacleSystem(FpsGame* game) : game_(game), guns_(&game->firing_component_) {}

void ObstacleSystem::frame() {
  auto colided = std::find_if(obstacles_.begin(), obstacles_.end(), [this](const auto& obstacle) {
    return guns_->collision(obstacle.physical_thing_);
  });
  if (colided != obstacles_.end()) {
    remove_obstacle(colided);
    guns_->remove_bullet_from_screen();
    make_obstacles();
  }
}

void ObstacleSystem::setup() { make_obstacles(); }
