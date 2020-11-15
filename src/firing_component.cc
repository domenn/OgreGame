#include "firing_component.hpp"

#include "fps_game.hpp"
#include "simple_random_generator.hpp"
#include <OgreEntity.h>

using namespace Ogre;
using namespace OgreBites;

fpsgame::FiringComponent::FiringComponent(FpsGame* game) : game_(game) {}

void fpsgame::FiringComponent::fire() {
  /* We could have vector of bullets and process each one each frame. But let's simplify - max one bullet on screen.
   * This also limits shoot rate - no need to do timing. */

  if (!bullet_node_) {
    bullet_node_ =
        game_->scn_mgr_->getRootSceneNode()->createChildSceneNode(game_->the_only_camera_node_->getPosition());
    bullet_physical_item_ = game_->scn_mgr_->createEntity(Ogre::SceneManager::PT_SPHERE);
    bullet_node_->attachObject(bullet_physical_item_);
    bullet_node_->setScale(0.5f, 0.5f, 0.5f);

    bullet_node_->setOrientation(Quaternion(game_->the_only_camera_node_->getOrientation()));
  }
}

void fpsgame::FiringComponent::frame() {
  move_bullet();

  if (obstacle_) {
    if (bullet_node_ && obstacle_physical_thing_->getWorldBoundingBox().contains(bullet_node_->getPosition())) {
      generic_remove(obstacle_, obstacle_physical_thing_);
      obstacle_ = nullptr;
      remove_bullet_from_screen();
    }
    // debugging ... reposition obstacle.
    if (game_->fps_game_gui_.experimental_obstacle_changed_) {
      const auto& prev_pos = obstacle_->getPosition();
      obstacle_->setPosition(prev_pos.x, obstacle_height_experimental_, prev_pos.z);
      const auto sc = obstacle_->getScale().x;
      game_->log_->logMessage("Scale is " + std::to_string(sc) +
                              " ;; H:" + std::to_string(obstacle_height_experimental_) +
                              " ;; S/H:" + std::to_string(sc / obstacle_height_experimental_));
    }
  } else {
    create_obstacle();
  }
}

void fpsgame::FiringComponent::move_bullet() {
  if (!bullet_node_) {
    return;
  }
  const auto& orientation = bullet_node_->getOrientation();
  bullet_node_->translate(orientation.zAxis() * (-bullet_speed_));

  if (!game_->bounding_box_playing_field_.contains(bullet_node_->getPosition())) {
    remove_bullet_from_screen();
  }
  // bullet_node->rotate(Vector3::UNIT_X, Radian(gravity_spped_));
}

void fpsgame::FiringComponent::remove_bullet_from_screen() {
  generic_remove(bullet_node_, bullet_physical_item_);
  bullet_node_ = nullptr;
}

void fpsgame::FiringComponent::generic_remove(Ogre::SceneNode* node, Ogre::Entity* entity) {
  node->detachObject(entity);
  game_->scn_mgr_->destroyEntity(reinterpret_cast<Entity*>(entity));
  game_->scn_mgr_->destroySceneNode(node);
}

void fpsgame::FiringComponent::create_obstacle() {
  const auto coord_x = SimpleRandomGenerator::generate(game_->bounding_box_playing_field_.getMinimum().x,
                                                       game_->bounding_box_playing_field_.getMaximum().x);
  const auto coord_z = SimpleRandomGenerator::generate(game_->bounding_box_playing_field_.getMinimum().z,
                                                       game_->bounding_box_playing_field_.getMaximum().z);
  const auto height = SimpleRandomGenerator::generate(obstacle_size_min_, obstacle_size_max_);

  obstacle_ = game_->scn_mgr_->getRootSceneNode()->createChildSceneNode(game_->the_only_camera_node_->getPosition());
  obstacle_physical_thing_ = game_->scn_mgr_->createEntity(Ogre::SceneManager::PT_CUBE);
  obstacle_->attachObject(obstacle_physical_thing_);
  obstacle_->setScale(height, height, height);
  // Experimentally determined the number we have to multiply with to have it be perfectly on the ground.
  obstacle_->setPosition(coord_x, height*50.f, coord_z);
}
