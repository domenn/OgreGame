#pragma once
#include "fps_game.hpp"

using namespace Ogre;
using namespace OgreBites;

namespace fpsgame {
class GameSetupComponent {
 public:
  
  static void setup(FpsGame* fps_game) {
    fps_game->addInputListener(fps_game);

    // get a pointer to the already created root
    Root* root = fps_game->getRoot();
    fps_game->log_ = Ogre::LogManager::getSingleton().createLog("FpsGame", true, true, false);
    auto * scn_mgr = fps_game->scn_mgr_ = root->createSceneManager();

    // register our scene with the RTSS
    RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scn_mgr);

    // -- tutorial section start --
    fps_game->the_only_camera_node_ = scn_mgr->getRootSceneNode()->createChildSceneNode();
    fps_game->the_only_camera_ = scn_mgr->createCamera("myCam");
    fps_game->camera_man_.reset(new CameraMan(fps_game->the_only_camera_node_));  // create a default camera controller

    // scnMgr->setSkyBox(true, "Examples/SceneCubeMap2");
    scn_mgr->setSkyBox(true, "Examples/EarlyMorningSkyBox");

    //! [cameraposition]
    fps_game->the_only_camera_node_->setPosition(200, 300, 400);
    fps_game->the_only_camera_node_->lookAt(Vector3(0, 0, 0), Node::TransformSpace::TS_WORLD);
    //! [cameraposition]

    //! [cameralaststep]
    fps_game->the_only_camera_->setNearClipDistance(5);
    fps_game->the_only_camera_node_->attachObject(fps_game->the_only_camera_);
    //! [cameralaststep]

    //! [addviewport]
    Viewport* vp = fps_game->getRenderWindow()->addViewport(fps_game->the_only_camera_);
    //! [addviewport]

    //! [viewportback]
    vp->setBackgroundColour(ColourValue(0, 0, 0));
    //! [viewportback]

    //! [cameraratio]
    fps_game->the_only_camera_->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
    //! [cameraratio]

    //! [lightingsset]
    scn_mgr->setAmbientLight(ColourValue(0.8f, 0.8f, 0.8f));
    scn_mgr->setShadowTechnique(ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
    //! [lightingsset]

    fps_game->player_entity_ = scn_mgr->createEntity("Ak-47.mesh");
    // player_entity_->setMaterialName("Ak-47.material");
    fps_game->player_entity_->setMaterialName("DomenMat/AK");
    fps_game->player_entity_->setCastShadows(true);

    // Const cast is safe here. Call delegates to memcpy which expects const anyway, so there are no modifications.
    fps_game->gun_node_can_be_positioned_ = fps_game->the_only_camera_node_->createChildSceneNode(
        "PlayerChildNodeOfCamera",
        Vector3{79, -131, -220},
        Quaternion{const_cast<Real*>(fps_game->fps_game_gui_.gun_rotation())});
    fps_game->gun_node_can_be_positioned_->attachObject(fps_game->player_entity_);

    fps_game->plane_node_ = scn_mgr->getRootSceneNode()->createChildSceneNode();
    fps_game->recreate_plane();
    // -- tutorial section end --
    fps_game->setWindowGrab(fps_game->mouse_grab_);
    fps_game->fps_game_gui_.setup_imgui(scn_mgr);
  }
};
}  // namespace fpsgame