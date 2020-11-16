#include "obstacle_system.hpp"

#include "firing_component.hpp"
#include "fps_game.hpp"
#include "helper_code.hpp"
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
  const auto how_many = SimpleRandomGenerator::random_integer<int>(min_obstacles_, max_obstacles_ + 1) -
                        static_cast<int>(obstacles_.size());
  game_->log_->logMessage("We have " + std::to_string(obstacles_.size()) + "; Generating " + std::to_string(how_many));
  for (int i = 0; i < how_many; ++i) {
    create_obstacle();
  }
}

ObstacleSystem::ObstacleSystem(FpsGame* game) : game_(game), guns_(&game->firing_component_) {}

void ObstacleSystem::score(std::vector<ObstacleOgrePtrs>::const_iterator obstacle) {
  std::ostringstream oss;

  const auto bullet_center3 = guns_->bullet_center();
  // Make 2d vectors. Ignore x coordinate for centerness caluclation.
  const Ogre::Vector2 block_center(obstacle->node_->getPosition().z, obstacle->node_->getPosition().y);
  const Ogre::Vector2 bullet_center(bullet_center3.z, bullet_center3.y);

  // Up to 100 points based on distance from block-pane (front) center. Center of pane scores more.
  const auto& bounding_box_corner =
      obstacle->physical_thing_->getWorldBoundingBox().getCorner(Ogre::AxisAlignedBox::FAR_RIGHT_TOP);

  const Ogre::Vector2 one_of_corners(bounding_box_corner.z, bounding_box_corner.y);
  auto max_distance = one_of_corners.distance(block_center);
  const auto scoring1 = 100.f - (100.f * (block_center.distance(bullet_center) / max_distance));
  game_->score_ += static_cast<int>(scoring1);

  // Far away blocks score more. Negative x means further away.
  const auto scoring2 =
      -1.f * obstacle->node_->getPosition().x / static_cast<float>(game_->plane_parameters_.wh / 2) * 100.f;
  game_->score_ += static_cast<int>(scoring2);

  // Larger blocks score less.
  // Prevent division by zero. It is theoretically possible with floats, we get infinity, but we don't want to deal with
  // that.
  auto scoring3 = floats_close(obstacle_size_min_, obstacle_size_max_)
                      ? 0
                      : 100.f - ((obstacle->node_->getScale().x - obstacle_size_min_) /
                                 (obstacle_size_max_ - obstacle_size_min_) * 100.f);
  // We may have changed the settings (to resize expected cube sizes). So, numbers wouldn't make any sense. Let's clamp
  // them.
  if (scoring3 < 0) {
    scoring3 = 0;
  }
  if (scoring3 > 100) {
    scoring3 = 100;
  }
  game_->score_ += static_cast<int>(scoring3);

  oss << "Block is on " << vec_to_str(block_center) << " and bullet is " << bullet_center << "; scale is "
      << obstacle->node_->getScale().x << "; Offcenter: " << block_center.distance(bullet_center)
      << "\n   OffcenterScore: " << scoring1 << "\n   DistanceFromPlayerScore: " << scoring2
      << "\n   SizeScore: " << scoring3;
  game_->log_->logMessage(oss.str());
}

void ObstacleSystem::frame() {
  auto colided = std::find_if(obstacles_.begin(), obstacles_.end(), [this](const auto& obstacle) {
    return guns_->collision(obstacle.physical_thing_);
  });
  if (colided != obstacles_.end()) {
    score(colided);
    remove_obstacle(colided);
    guns_->remove_bullet_from_screen();
    make_obstacles();
  }
}

void ObstacleSystem::setup() { make_obstacles(); }
