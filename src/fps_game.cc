#include "fps_game.hpp"

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreCameraMan.h"
#include "OgreRTShaderSystem.h"

using namespace Ogre;
using namespace OgreBites;

fpsgame::FpsGame::FpsGame() : OgreBites::ApplicationContext("FpsGame") {}
fpsgame::FpsGame::~FpsGame() = default;

void fpsgame::FpsGame::setup() {
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

  //! [ninja]
  player_entity_ = scnMgr->createEntity("ninja.mesh");
  player_entity_->setCastShadows(true);

  player_node_ = scnMgr->getRootSceneNode()->createChildSceneNode();
  player_node_->attachObject(player_entity_);
  //! [ninja]

  //! [plane]
  Plane plane(Vector3::UNIT_Y, 0);
  //! [plane]

  //! [planedefine]
  MeshManager::getSingleton().createPlane(
      "ground", RGN_DEFAULT, plane, 1500, 1500, 20, 20, true, 1, 5, 5, Vector3::UNIT_Z);
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
}

bool fpsgame::FpsGame::keyPressed(const OgreBites::KeyboardEvent& evt) {
  if (evt.keysym.sym == SDLK_ESCAPE) {
    getRoot()->queueEndRendering();
  }
  if (evt.keysym.sym == 'w') {
    //    player_node_->translate()
    // the_only_camera_->
    camera_man_->setStyle(static_cast<CameraStyle>((camera_man_->getStyle() + 1) % 3));
    log_->logMessage("Style set to: " + std::to_string(camera_man_->getStyle()));
  }
  if (evt.keysym.sym == 'a') {
    setWindowGrab(true);
  }
  if (evt.keysym.sym == 's') {
    setWindowGrab(false);
  }
  if (evt.keysym.sym == 'd') {
    log_->logMessage("Pressed w");
  }
  camera_man_->keyPressed(evt);
  return true;
}

bool fpsgame::FpsGame::mouseMoved(const OgreBites::MouseMotionEvent& evt) {
  camera_man_->mouseMoved(evt);
  return true;
}

bool fpsgame::FpsGame::mousePressed(const OgreBites::MouseButtonEvent& evt) {
  camera_man_->mousePressed(evt);
  return true;
}

bool fpsgame::FpsGame::mouseReleased(const OgreBites::MouseButtonEvent& evt) {
  camera_man_->mouseReleased(evt);
  return true;
}

bool fpsgame::FpsGame::frameRenderingQueued(const Ogre::FrameEvent& evt) { return true; }

bool fpsgame::FpsGame::keyReleased(const OgreBites::KeyboardEvent& evt) {
  camera_man_->keyReleased(evt);
  return true;
}
