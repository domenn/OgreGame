

#include "fps_game_gui.hpp"

#include "fps_game.hpp"
#include <OgreImGuiInputListener.h>
#include <OgreImGuiOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlaySystem.h>
#include <imgui.h>

ImVec2 halve(const ImVec2& im_vec2) { return ImVec2(im_vec2.x * 0.5f, im_vec2.y * 0.5f); }

ImVec2 imvec2_plus(const ImVec2& left, const ImVec2& right) { return ImVec2(left.x + right.x, left.y + right.y); }
ImVec2 imvec2_minus(const ImVec2& left, const ImVec2& right) { return ImVec2(left.x - right.x, left.y - right.y); }

void fpsgame::FpsGameGui::crosshair() {
  // @ screen resolution 1280x720
  constexpr float crosshair_circle_diameter = 32.f;
  const ImVec2 crosshair_circle_center = halve(ImGui::GetIO().DisplaySize);

  constexpr float crosshair_thickness = 1.8f;
  const float crosshair_circle_radius_outline = crosshair_circle_diameter * 0.5f;
  const ImU32 crosshair_circle_color_outline = ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 0.f, 0.f, 1.f));

  ImGui::GetStyle().WindowPadding = ImVec2(0, 0);
  ImGui::GetStyle().WindowRounding = 0;
  ImGui::GetStyle().WindowBorderSize = 0.0f;

  constexpr float crosshair_lines_size = crosshair_circle_diameter * 1.45f;
  constexpr float crosshair_lines_half_size = crosshair_lines_size * 0.5f;

  const ImVec2 crosshair_circle_window_size = ImVec2(crosshair_lines_size, crosshair_lines_size);
  const ImVec2 crosshair_circle_window_position =
      imvec2_minus(crosshair_circle_center, halve(crosshair_circle_window_size));

  ImGui::SetNextWindowPos(crosshair_circle_window_position);
  ImGui::SetNextWindowSize(crosshair_circle_window_size);

  ImGui::Begin("crosshair_circle",
               nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  // drawList->AddCircleFilled(_reticleCenter, _reticleRadius, _reticleColor, 12);
  drawList->AddCircle(crosshair_circle_center,
                      crosshair_circle_radius_outline,
                      crosshair_circle_color_outline,
                      14,
                      crosshair_thickness);
  drawList->AddLine(
      imvec2_minus(crosshair_circle_center, ImVec2(crosshair_lines_half_size, (crosshair_thickness * 0.5f))),
      imvec2_plus(crosshair_circle_center, ImVec2(crosshair_lines_half_size, -(crosshair_thickness * 0.5f))),
      crosshair_circle_color_outline,
      crosshair_thickness);

  drawList->AddLine(
      imvec2_minus(crosshair_circle_center, ImVec2((crosshair_thickness * 0.5f), crosshair_lines_half_size)),
      imvec2_plus(crosshair_circle_center, ImVec2(-(crosshair_thickness * 0.5f), crosshair_lines_half_size)),
      crosshair_circle_color_outline,
      crosshair_thickness);

  ImGui::End();
}

void fpsgame::FpsGameGui::plane_tools() {
  ImGui::Begin("Plane tools");
  bool changed = ImGui::DragInt("uv_tiling", &game_->plane_parameters_.plane_uv, 1, 1, 90) |
                 ImGui::DragInt("num_tex_coord_sets", &game_->plane_parameters_.num_tex_coord_sets, 1, 1, 32) |
                 ImGui::DragInt("xy_segments", &game_->plane_parameters_.xy_segments, 1, 1, 64) |
                 ImGui::DragInt("wh", &game_->plane_parameters_.wh, 200, 1, 100000);
  ImGui::End();

  if (changed) {
    game_->recreate_plane();
  }
}

void fpsgame::FpsGameGui::info_text() {
  ImVec2 screen_top_left = ImGui::GetIO().DisplaySize;
  screen_top_left.x *= 0.02f;
  screen_top_left.y *= 0.012f;

  ImGui::SetNextWindowPos(screen_top_left);
  ImGui::SetNextWindowSize(ImVec2(800,60));
  ImGui::SetNextWindowBgAlpha(0.6f);

  ImGui::Begin("info_text",
               nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs);

  ImGui::TextColored(ImVec4(0.7f, 0, 0.1f, 1), "CONTROLS: F show/hide cursor; G camera modes; Q show/hide configurator.");
  ImGui::TextColored(ImVec4(0.7f, 0, 0.1f, 1), "Obstacles on screen: ", 0);
  ImGui::TextColored(ImVec4(0.7f, 0, 0.1f, 1), "Score: %d", game_->score_);

  ImGui::End();
}

void fpsgame::FpsGameGui::frame_started() {
  Ogre::ImGuiOverlay::NewFrame();

  // ImGui::ShowDemoWindow();

  if (game_->the_only_camera_node_) {
    const auto& cam_pos = game_->the_only_camera_node_->getPosition();
    camera_position_[0] = cam_pos.x;
    camera_position_[1] = cam_pos.y;
    camera_position_[2] = cam_pos.z;

    const auto& gpos = game_->gun_node_can_be_positioned_->getPosition();
    gun_position_[0] = gpos.x;
    gun_position_[1] = gpos.y;
    gun_position_[2] = gpos.z;

    const auto& orientation = game_->gun_node_can_be_positioned_->getOrientation();
    // std::tie(gun_rotation_[0], gun_rotation_[1], gun_rotation_[2], gun_rotation_[3]) =
    //    std::make_tuple(orientation.w, orientation.x, orientation.y, orientation.z);

    gun_rotation_[0] = orientation.w;
    gun_rotation_[1] = orientation.x;
    gun_rotation_[2] = orientation.y;
    gun_rotation_[3] = orientation.z;
  }
  bool moving_camera{false};
  if (settings_on_) {
    ImGui::Begin("GenericSettings");
    ImGui::DragFloat3("GunPosition", gun_position_, 1, -2600, 2600);
    ImGui::DragFloat4("GunRotation(wxyz)", gun_rotation_, 0.001f, -4, 4);
    moving_camera = ImGui::DragFloat3("CameraPosition", camera_position_, 1, -2600, 2600);
    ImGui::DragFloat("Scale", &gun_scale_, 0.01f, -26, 26);
    ImGui::DragFloat("Movement Speed", &game_->move_speed_, 0.25f, 1, 20);
    ImGui::DragFloat("Bullet speed", game_->firing_component_.bullet_speed(), 1.f, -1, 2600);
    ImGui::DragFloat("Bullet gravity", game_->firing_component_.gravity_speed(), 0.01f, -0.0001f, -0.0000001f);
    ImGui::DragInt("Min obstacles", game_->obstacle_system_.min_obstacles(), 1, 1, 199);
    if (*game_->obstacle_system_.min_obstacles() > *game_->obstacle_system_.max_obstacles()) {
      *game_->obstacle_system_.max_obstacles() = *game_->obstacle_system_.min_obstacles();
    }
    ImGui::DragInt("Max obstacles", game_->obstacle_system_.max_obstacles(), 1, 1, 200);

    ImGui::DragFloat("Obstacle size Max", game_->obstacle_system_.obstacle_size_max(), 0.1f, 0.1f, 6.f);
    ImGui::DragFloat("Obstacle size Min", game_->obstacle_system_.obstacle_size_min(), 0.1f, 0.1f, 6.f);
    experimental_obstacle_changed_ = ImGui::DragFloat(
        "EXPERIMENTAL obstacle height", &game_->obstacle_system_.obstacle_height_experimental_, 1.f, 1.f, 200.f);
    ImGui::End();

    plane_tools();
  }

  crosshair();
  info_text();

  if (game_->gun_node_can_be_positioned_) {
    game_->gun_node_can_be_positioned_->setPosition(gun_position_[0], gun_position_[1], gun_position_[2]);
    game_->gun_node_can_be_positioned_->setOrientation(
        gun_rotation_[0], gun_rotation_[1], gun_rotation_[2], gun_rotation_[3]);
    game_->gun_node_can_be_positioned_->setScale(gun_scale_, gun_scale_, gun_scale_);

    // gun_node_can_be_positioned_->getChild(0)->setPosition(
    //    experimental_position_[0], experimental_position_[1], experimental_position_[2]);
  }

  if (moving_camera) {
    game_->the_only_camera_node_->setPosition(camera_position_[0], camera_position_[1], camera_position_[2]);
  }
}

void fpsgame::FpsGameGui::setup_imgui(Ogre::SceneManager* scene_manager) {
  scene_manager->addRenderQueueListener(Ogre::OverlaySystem::getSingletonPtr());
  auto* imgui_overlay = new Ogre::ImGuiOverlay();
  imgui_overlay->setZOrder(300);
  imgui_overlay->show();
  Ogre::OverlayManager::getSingleton().addOverlay(imgui_overlay);  // now owned by overlaymgr

  game_->imgui_listener_.reset(new OgreBites::ImGuiInputListener());
}

fpsgame::FpsGameGui::FpsGameGui(fpsgame::FpsGame* fps_game_inst) : game_(fps_game_inst) {}