#include "fps_game.hpp"

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreCameraMan.h"
#include "game_setup_component.hpp"
#include "helper_code.hpp"
#include <OgreImGuiInputListener.h>
// This include is necessary. Otherwise we get compile-error with regarding unique_ptr and the destructor.
#include <OgreOverlaySystem.h>

using namespace Ogre;
using namespace OgreBites;

void fpsgame::Improved2dAxisAlignedBox::clip_scene_node(Ogre::SceneNode* receiver) const {
  const auto& pos = receiver->getPosition();
  // return(v.x >= mMinimum.x  &&  v.x <= mMaximum.x  &&
  //    v.y >= mMinimum.y  &&  v.y <= mMaximum.y  &&
  //    v.z >= mMinimum.z  &&  v.z <= mMaximum.z);

  if (pos.x < mMinimum.x) {
    receiver->setPosition(mMinimum.x, pos.y, pos.z);
  } else if (pos.x > mMaximum.x) {
    receiver->setPosition(mMaximum.x, pos.y, pos.z);
  }

  if (pos.z < mMinimum.z) {
    receiver->setPosition(pos.x, pos.y, mMinimum.z);
  } else if (pos.z > mMaximum.z) {
    receiver->setPosition(pos.x, pos.y, mMaximum.z);
  }
}

fpsgame::FpsGame::FpsGame() : OgreBites::ApplicationContext("FpsGame") {}
fpsgame::FpsGame::~FpsGame() = default;

void fpsgame::FpsGame::setup() {
  OgreBites::ApplicationContext::setup();
  GameSetupComponent::setup(this);
}

bool fpsgame::FpsGame::frameStarted(const Ogre::FrameEvent& evt) {
  OgreBites::ApplicationContext::frameStarted(evt);

  fps_game_gui_.frame_started();
  firing_component_.frame();
  wasd_move();

  return true;
}

bool fpsgame::FpsGame::keyPressed(const OgreBites::KeyboardEvent& evt) {
  if (imgui_listener_->keyPressed(evt)) {
    return true;
  }
  if (evt.keysym.sym == SDLK_ESCAPE) {
    getRoot()->queueEndRendering();
  }
  if (evt.keysym.sym >= 'a' && evt.keysym.sym <= 'w') {
    if (!key_latch_[evt.keysym.sym]) {
      key_latch_[evt.keysym.sym] = true;
    }
  }
  if (evt.keysym.sym == SDLK_SPACE) {
    // Prevent space held. Only fire on press, then not anymore until release.
    if (!key_latch_[evt.keysym.sym]) {
      key_latch_[evt.keysym.sym] = true;
      firing_component_.fire();
    }
  }
  if (evt.keysym.sym == 'f') {
    mouse_grab_ = !mouse_grab_;
    setWindowGrab(mouse_grab_);
  }
  if (evt.keysym.sym == 'g') {
    auto new_style = (camera_man_->getStyle() + 1) % 3;
    camera_man_->setStyle(static_cast<CameraStyle>(new_style));
    log_->logMessage("Camera style is now " + std::to_string(new_style));
  }

  camera_man_->keyPressed(evt);
  return true;
}

bool fpsgame::FpsGame::mouseMoved(const OgreBites::MouseMotionEvent& evt) {
  imgui_listener_->mouseMoved(evt);
  camera_man_->mouseMoved(evt);
  const auto& camera_direction = the_only_camera_node_->getOrientation();
  const auto& direction_vector = camera_direction.zAxis() * -1;

  return true;
}

bool fpsgame::FpsGame::mousePressed(const OgreBites::MouseButtonEvent& evt) {
  imgui_listener_->mousePressed(evt);
  camera_man_->mousePressed(evt);
  return true;
}

bool fpsgame::FpsGame::mouseReleased(const OgreBites::MouseButtonEvent& evt) {
  imgui_listener_->mouseReleased(evt);
  camera_man_->mouseReleased(evt);
  return true;
}

bool fpsgame::FpsGame::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt) {
  move_speed_ += static_cast<float>(evt.y) * 1.2f;
  log_->logMessage("Speed is now " + std::to_string(move_speed_));
  camera_man_->mouseWheelRolled(evt);
  return true;
}

bool fpsgame::FpsGame::frameRenderingQueued(const Ogre::FrameEvent& evt) { return true; }

bool fpsgame::FpsGame::keyReleased(const OgreBites::KeyboardEvent& evt) {
  if (imgui_listener_->keyReleased(evt)) {
    return true;
  }
  if (evt.keysym.sym >= SDLK_SPACE && evt.keysym.sym <= 'w') {
    key_latch_[evt.keysym.sym] = false;
  }

  camera_man_->keyReleased(evt);
  return true;
}

void fpsgame::FpsGame::recreate_plane() {
  if (plane_entity_) {
    plane_node_->detachObject(static_cast<unsigned short>(0));
    MeshManager::getSingleton().remove("ground");
    delete plane_entity_;
  }

  Plane plane(Vector3::UNIT_Y, 0);

  MeshManager::getSingleton().createPlane("ground",
                                          RGN_DEFAULT,
                                          plane,
                                          plane_parameters_.wh,
                                          plane_parameters_.wh,
                                          plane_parameters_.xy_segments,
                                          plane_parameters_.xy_segments,
                                          true,
                                          plane_parameters_.num_tex_coord_sets,
                                          plane_parameters_.plane_uv,
                                          plane_parameters_.plane_uv,
                                          Vector3::UNIT_Z);
  plane_entity_ = scn_mgr_->createEntity("ground");
  plane_node_->attachObject(plane_entity_);
  plane_entity_->setCastShadows(false);
  plane_entity_->setMaterialName("Examples/Rockwall");

  const auto plane_extremes = plane_parameters_.wh / 2;

  bounding_box_playing_field_ =
      AxisAlignedBox(-plane_extremes, -10.f, -plane_extremes, plane_extremes, 3000, plane_extremes);
}

void fpsgame::FpsGame::wasd_move() {
  if (key_latch_['w']) {
    move_within_xz(the_only_camera_node_, (the_only_camera_node_->getOrientation().zAxis() * -1));
    bounding_box_playing_field_.clip_scene_node(the_only_camera_node_);
  }
  if (key_latch_['s']) {
    move_within_xz(the_only_camera_node_, (the_only_camera_node_->getOrientation().zAxis()));
    bounding_box_playing_field_.clip_scene_node(the_only_camera_node_);
  }
  if (key_latch_['a']) {
    move_within_xz(the_only_camera_node_, (the_only_camera_node_->getOrientation().xAxis() * -1));
    bounding_box_playing_field_.clip_scene_node(the_only_camera_node_);
  }
  if (key_latch_['d']) {
    move_within_xz(the_only_camera_node_, (the_only_camera_node_->getOrientation().xAxis()));
    bounding_box_playing_field_.clip_scene_node(the_only_camera_node_);
  }
}

void fpsgame::FpsGame::make_forward(Ogre::Vector3& v) {
  v.y = 0;
  v.normalise();
}

void fpsgame::FpsGame::move_within_xz(Ogre::SceneNode* const node, Ogre::Vector3&& raw_vector) {
  make_forward(raw_vector);
  raw_vector *= move_speed_;
  node->translate(raw_vector);
}