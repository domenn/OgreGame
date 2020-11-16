#include "firing_component.hpp"

#include "fps_game.hpp"
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

void fpsgame::FiringComponent::frame() { move_bullet(); }

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
