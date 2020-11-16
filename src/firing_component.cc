#include "firing_component.hpp"

#include "fps_game.hpp"
#include "helper_code.hpp"
#include <OgreEntity.h>

using namespace Ogre;
using namespace OgreBites;

fpsgame::FiringComponent::FiringComponent(FpsGame* game) : game_(game) {}

void fpsgame::FiringComponent::fire() {
  /* We could have vector of bullets and process each one each frame. But let's simplify - max one bullet on screen.
   * This also limits shoot rate - no need to do timing. */

  if (!bullet_node_) {
    balistic_.initial_position = game_->the_only_camera_node_->getPosition();
    bullet_node_ = game_->scn_mgr_->getRootSceneNode()->createChildSceneNode(balistic_.initial_position);
    bullet_physical_item_ = game_->scn_mgr_->createEntity(Ogre::SceneManager::PT_SPHERE);
    bullet_node_->attachObject(bullet_physical_item_);
    bullet_node_->setScale(0.5f, 0.5f, 0.5f);

    bullet_node_->setOrientation(game_->the_only_camera_node_->getOrientation());
    balistic_.theta = game_->player_pitch_;
    balistic_.cos_theta = cosf(balistic_.theta);
    balistic_.sin_theta = sinf(balistic_.theta);
    balistic_.time_elapsed = 0;
    balistic_.xz_direction = -bullet_node_->getOrientation().zAxis();
    balistic_.xz_direction.y = 0;
    balistic_.xz_direction.normalise();
  }
  game_->log_->logMessage("NEW FIRE direction " + vec_to_str(balistic_.xz_direction));
}

void fpsgame::FiringComponent::frame() { move_bullet(); }

void fpsgame::FiringComponent::move_bullet() {
  if (!bullet_node_) {
    return;
  }
  balistic_.time_elapsed += game_->frame_time_;

  const auto displacement_x = bullet_speed_ * balistic_.time_elapsed * balistic_.cos_theta;
  const auto displacement_y = bullet_speed_ * balistic_.time_elapsed * balistic_.sin_theta -
                              0.5f * gravity_ * (balistic_.time_elapsed * balistic_.time_elapsed);

  const auto new_position_x = balistic_.initial_position + (balistic_.xz_direction * displacement_x);
  const auto new_position_y = balistic_.initial_position.y + displacement_y;

  bullet_node_->setPosition(new_position_x.x, new_position_y, new_position_x.z);

  if (!game_->bounding_box_playing_field_.contains(bullet_node_->getPosition())) {
    remove_bullet_from_screen();
  }
}

void fpsgame::FiringComponent::remove_bullet_from_screen() {
  game_->generic_remove(bullet_node_, bullet_physical_item_);
  bullet_node_ = nullptr;
}

bool fpsgame::FiringComponent::collision(const Ogre::Entity* const item) const {
  return bullet_node_ && item->getWorldBoundingBox().contains(bullet_node_->getPosition());
}

const Ogre::Vector3* fpsgame::FiringComponent::bullet_position() const {
  assert(bullet_node_);
  return &bullet_node_->getPosition();
}

Ogre::Vector3 fpsgame::FiringComponent::bullet_center() const {
  assert(bullet_node_);
  return bullet_physical_item_->getWorldBoundingSphere().getCenter();
}
