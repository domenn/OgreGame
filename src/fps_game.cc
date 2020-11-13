#include "fps_game.hpp"

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreCameraMan.h"
#include "OgreRTShaderSystem.h"
#include "helper_code.hpp"
#include <OgreImGuiInputListener.h>
#include <OgreImGuiOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlaySystem.h>

using namespace Ogre;
using namespace OgreBites;

fpsgame::FpsGame::FpsGame() : OgreBites::ApplicationContext("FpsGame") {}
fpsgame::FpsGame::~FpsGame() = default;

void fpsgame::FpsGame::setup() {
  //// Make overlay system before I initialize root in setup() method.
  // overlay_system_.reset(new Ogre::OverlaySystem());
  // do not forget to call the base first
  ApplicationContext::setup();
  addInputListener(this);

  // get a pointer to the already created root
  Root* root = getRoot();
  log_ = Ogre::LogManager::getSingleton().createLog("FpsGame", true, true, false);
  SceneManager* scnMgr = root->createSceneManager();

  // register our scene with the RTSS
  RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
  shadergen->addSceneManager(scnMgr);

  // -- tutorial section start --
  the_only_camera_node_ = scnMgr->getRootSceneNode()->createChildSceneNode();
  the_only_camera_ = scnMgr->createCamera("myCam");
  camera_man_.reset(new CameraMan(the_only_camera_node_));  // create a default camera controller

  // scnMgr->setSkyBox(true, "Examples/SceneCubeMap2");
  scnMgr->setSkyBox(true, "Examples/EarlyMorningSkyBox");

  //! [cameraposition]
  the_only_camera_node_->setPosition(200, 300, 400);
  the_only_camera_node_->lookAt(Vector3(0, 0, 0), Node::TransformSpace::TS_WORLD);
  //! [cameraposition]

  //! [cameralaststep]
  the_only_camera_->setNearClipDistance(5);
  the_only_camera_node_->attachObject(the_only_camera_);
  //! [cameralaststep]

  //! [addviewport]
  Viewport* vp = getRenderWindow()->addViewport(the_only_camera_);
  //! [addviewport]

  //! [viewportback]
  vp->setBackgroundColour(ColourValue(0, 0, 0));
  //! [viewportback]

  //! [cameraratio]
  the_only_camera_->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
  //! [cameraratio]

  //! [lightingsset]
  scnMgr->setAmbientLight(ColourValue(0.8f, 0.8f, 0.8f));
  scnMgr->setShadowTechnique(ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
  //! [lightingsset]

  player_entity_ = scnMgr->createEntity("Ak-47.mesh");
  // player_entity_->setMaterialName("Ak-47.material");
  player_entity_->setMaterialName("DomenMat/AK");
  player_entity_->setCastShadows(true);

  // gun_node_can_be_positioned_ =
  //    scnMgr->getRootSceneNode()->createChildSceneNode("PlayerChildNodeOfCamera", Vector3{0, 1, 3});

  gun_node_can_be_positioned_ = the_only_camera_node_->createChildSceneNode("PlayerChildNodeOfCamera",
      Vector3{79,-131,-220},Quaternion{gun_rotation_});
  gun_node_can_be_positioned_->attachObject(player_entity_);

  // Entity* ninjaEntity = scnMgr->createEntity("ninja.mesh");
  // ninjaEntity->setCastShadows(true);
  // gun_node_can_be_positioned_->createChildSceneNode("testingNinja", Vector3{300, 15,
  // -12})->attachObject(ninjaEntity);

  //! [plane]
  Plane plane(Vector3::UNIT_Y, 0);
  //! [plane]

  //! [planedefine]
  MeshManager::getSingleton().createPlane(
      "ground", RGN_DEFAULT, plane, 150000, 150000, 20, 20, true, 1, 500, 500, Vector3::UNIT_Z);
  //! [planedefine]

  //! [planecreate]
  Entity* groundEntity = scnMgr->createEntity("ground");
  scnMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
  //! [planecreate]

  //! [planenoshadow]
  groundEntity->setCastShadows(false);
  //! [planenoshadow]

  //! [planesetmat]
  groundEntity->setMaterialName("Examples/Rockwall");
  //! [planesetmat]
  // -- tutorial section end --
  setWindowGrab(mouse_grab_);
  setup_imgui(scnMgr);
}

bool fpsgame::FpsGame::frameStarted(const Ogre::FrameEvent& evt) {
  OgreBites::ApplicationContext::frameStarted(evt);
  ImGuiOverlay::NewFrame();

  // ImGui::ShowDemoWindow();

  if (the_only_camera_node_) {
    const auto& cam_pos = the_only_camera_node_->getPosition();
    camera_position_[0] = cam_pos.x;
    camera_position_[1] = cam_pos.y;
    camera_position_[2] = cam_pos.z;

    const auto& gpos = gun_node_can_be_positioned_->getPosition();
    gun_position_[0] = gpos.x;
    gun_position_[1] = gpos.y;
    gun_position_[2] = gpos.z;

    const auto& orientation = gun_node_can_be_positioned_->getOrientation();
    // std::tie(gun_rotation_[0], gun_rotation_[1], gun_rotation_[2], gun_rotation_[3]) =
    //    std::make_tuple(orientation.w, orientation.x, orientation.y, orientation.z);

    gun_rotation_[0] = orientation.w;
    gun_rotation_[1] = orientation.x;
    gun_rotation_[2] = orientation.y;
    gun_rotation_[3] = orientation.z;
  }

  ImGui::Begin("Positions");
  ImGui::DragFloat3("GunPosition", gun_position_, 1, -2600, 2600);
  ImGui::DragFloat4("GunRotation(wxyz)", gun_rotation_, 0.001f, -4, 4);
  ImGui::DragFloat3("Experimental", experimental_position_, 1, -2600, 2600);
  const auto moving_camera = ImGui::DragFloat3("CameraPosition", camera_position_, 1, -2600, 2600);
  ImGui::DragFloat("Scale", &gun_scale_, 0.01f, -26, 26);
  ImGui::End();

  if (gun_node_can_be_positioned_) {
    gun_node_can_be_positioned_->setPosition(gun_position_[0], gun_position_[1], gun_position_[2]);
    gun_node_can_be_positioned_->setOrientation(gun_rotation_[0], gun_rotation_[1], gun_rotation_[2], gun_rotation_[3]);
    gun_node_can_be_positioned_->setScale(gun_scale_, gun_scale_, gun_scale_);

    // gun_node_can_be_positioned_->getChild(0)->setPosition(
    //    experimental_position_[0], experimental_position_[1], experimental_position_[2]);
  }

  if (moving_camera) {
    the_only_camera_node_->setPosition(camera_position_[0], camera_position_[1], camera_position_[2]);
  }

  return true;
}

bool fpsgame::FpsGame::keyPressed(const OgreBites::KeyboardEvent& evt) {
  imgui_listener_->keyPressed(evt);
  if (evt.keysym.sym == SDLK_ESCAPE) {
    getRoot()->queueEndRendering();
  }
  if (evt.keysym.sym == 'w') {
    move_within_xz(the_only_camera_node_, (the_only_camera_node_->getOrientation().zAxis() * -1));
  }
  if (evt.keysym.sym == 'f') {
    mouse_grab_ = !mouse_grab_;
    setWindowGrab(mouse_grab_);
  }
  if (evt.keysym.sym == 's') {
    move_within_xz(the_only_camera_node_, (the_only_camera_node_->getOrientation().zAxis()));
  }
  if (evt.keysym.sym == 'd') {
    log_->logMessage("Pressed w");
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

  log_->logMessage("Direction is: " + fpsgame::vec_to_str(direction_vector));
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
  camera_man_->keyReleased(evt);
  return true;
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

void fpsgame::FpsGame::setup_imgui(Ogre::SceneManager* scene_manager) {
  scene_manager->addRenderQueueListener(OverlaySystem::getSingletonPtr());
  auto* imgui_overlay = new ImGuiOverlay();
  imgui_overlay->setZOrder(300);
  imgui_overlay->show();
  Ogre::OverlayManager::getSingleton().addOverlay(imgui_overlay);  // now owned by overlaymgr

  imgui_listener_.reset(new OgreBites::ImGuiInputListener());
  // mListenerChain = InputListenerChain({mTrayMgr.get(), mImguiListener.get(), mCameraMan.get()});

  // mTrayMgr->showCursor();
  // mCameraMan->setStyle(OgreBites::CS_ORBIT);
  // mCameraMan->setYawPitchDist(Degree(0), Degree(0), 15);

  // SceneNode* lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
  // lightNode->setPosition(0, 10, 15);
  // lightNode->attachObject(mSceneMgr->createLight("MainLight"));

  // Entity* ent = mSceneMgr->createEntity("Sinbad.mesh");
  // SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
  // node->attachObject(ent);
}
