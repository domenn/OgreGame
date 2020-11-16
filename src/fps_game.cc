#include "fps_game.hpp"

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreCameraMan.h"
#include "game_setup_component.hpp"
#include "helper_code.hpp"
#include <OgreImGuiInputListener.h>
// This include is necessary. Otherwise we get compile-error with regarding unique_ptr and the destructor.
#include <OgreOverlaySystem.h>
#include <chrono>
#include <thread>

using namespace Ogre;
using namespace OgreBites;

void fpsgame::Improved2dAxisAlignedBox::clip_scene_node_inside_plus_x(Ogre::SceneNode* receiver) const {
  const auto& pos = receiver->getPosition();

  if (pos.x < 0) {
    receiver->setPosition(0, pos.y, pos.z);
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
  obstacle_system_.setup();
}

bool fpsgame::FpsGame::frameStarted(const Ogre::FrameEvent& evt) {
  if (evt.timeSinceLastFrame != 0.f) {
    frames_per_second_.add_frame_time(evt.timeSinceLastFrame);
    frame_time_ = evt.timeSinceLastFrame;
  }
  OgreBites::ApplicationContext::frameStarted(evt);

  fps_game_gui_.frame_started();
  firing_component_.frame();
  obstacle_system_.frame();
  wasd_move();
  // artificially limit FPS, so ridiculously high numbers can't screw with us ...
  if (frames_per_second_.get_fps() > 300) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

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

  const auto& orientation = the_only_camera_node_->getOrientation();
  Ogre::Matrix3 my_matrix_3;
  orientation.ToRotationMatrix(my_matrix_3);
  Ogre::Radian pitch, yaw, roll;
  my_matrix_3.ToEulerAnglesYXZ(yaw, pitch, roll);
  player_pitch_ = pitch.valueRadians();
  player_yaw_ = yaw.valueRadians();
  player_roll_ = roll.valueRadians();

  // Canceled attempt to prevent pitch above 90 degrees and below -90.

  /*if (player_pitch_ > 1.570796f) {
    my_matrix_3.FromEulerAnglesYXZ(yaw, Ogre::Radian(1.570796f), roll);
    Quaternion new_o(my_matrix_3);
    the_only_camera_node_->setOrientation(new_o);
  }
  if (player_pitch_ < -1.570796f) {
    my_matrix_3.FromEulerAnglesYXZ(yaw, Ogre::Radian(-1.570796f), roll);
    Quaternion new_o(my_matrix_3);
    the_only_camera_node_->setOrientation(new_o);
  }*/

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
  imgui_listener_->mouseWheelRolled(evt);
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
    if (evt.keysym.sym == 'q') {
      fps_game_gui_.flip_settings();
    }
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

void fpsgame::FpsGame::move_player(Ogre::Vector3&& direction) {
  move_within_xz(the_only_camera_node_, std::move(direction));
  bounding_box_playing_field_.clip_scene_node_inside_plus_x(the_only_camera_node_);
}

void fpsgame::FpsGame::wasd_move() {
  if (key_latch_['w']) {
    move_player(the_only_camera_node_->getOrientation().zAxis() * -1);
  }
  if (key_latch_['s']) {
    move_player(the_only_camera_node_->getOrientation().zAxis());
  }
  if (key_latch_['a']) {
    move_player(the_only_camera_node_->getOrientation().xAxis() * -1);
  }
  if (key_latch_['d']) {
    move_player(the_only_camera_node_->getOrientation().xAxis());
  }
}

void fpsgame::FpsGame::make_forward(Ogre::Vector3& v) {
  v.y = 0;
  v.normalise();
}

void fpsgame::FpsGame::move_within_xz(Ogre::SceneNode* const node, Ogre::Vector3&& raw_vector) {
  make_forward(raw_vector);
  raw_vector *= move_speed_ * frame_time_;
  node->translate(raw_vector);
}

void fpsgame::FpsGame::generic_remove(Ogre::SceneNode* node, Ogre::Entity* entity) {
  node->detachObject(entity);
  scn_mgr_->destroyEntity(reinterpret_cast<Entity*>(entity));
  scn_mgr_->destroySceneNode(node);
}