#pragma once

#include "Mode.hpp"
#include "Scene.hpp"
#include "Sound.hpp"

#include <unordered_map>

namespace ObserveModeSettings {

  const char* const names_pivots[5] = {
    "pivot.000",
    "pivot.001",
    "pivot.002",
    "pivot.003",
    "pivot.004",
  };

  const char * const names_targets[2] = {
    "target.000",
    "target.001",
  };

  const char * const name_point = "pointer";

  struct PivotAxis {
    uint32_t index;
    enum {
      XP, XN, YP, YN, ZP, ZN
    } axis;
  };

  const std::unordered_map< SDL_Keycode, const PivotAxis > key_mapping = {
    {SDLK_q, {0, PivotAxis::XP}},
    {SDLK_w, {0, PivotAxis::XN}},
    {SDLK_a, {0, PivotAxis::YP}},
    {SDLK_s, {0, PivotAxis::YN}},
    {SDLK_z, {0, PivotAxis::ZP}},
    {SDLK_x, {0, PivotAxis::ZN}},
    {SDLK_e, {1, PivotAxis::XP}},
    {SDLK_r, {1, PivotAxis::XN}},
    {SDLK_d, {1, PivotAxis::YP}},
    {SDLK_f, {1, PivotAxis::YN}},
    {SDLK_c, {1, PivotAxis::ZP}},
    {SDLK_v, {1, PivotAxis::ZN}},
    {SDLK_t, {2, PivotAxis::XP}},
    {SDLK_y, {2, PivotAxis::XN}},
    {SDLK_g, {2, PivotAxis::YP}},
    {SDLK_h, {2, PivotAxis::YN}},
    {SDLK_b, {2, PivotAxis::ZP}},
    {SDLK_n, {2, PivotAxis::ZN}},
    {SDLK_u, {3, PivotAxis::XP}},
    {SDLK_i, {3, PivotAxis::XN}},
    {SDLK_j, {3, PivotAxis::YP}},
    {SDLK_k, {3, PivotAxis::YN}},
    {SDLK_m, {3, PivotAxis::ZP}},
    {SDLK_COMMA, {3, PivotAxis::ZN}},
    {SDLK_o, {4, PivotAxis::XP}},
    {SDLK_p, {4, PivotAxis::XN}},
    {SDLK_l, {4, PivotAxis::YP}},
    {SDLK_SEMICOLON, {4, PivotAxis::YN}},
    {SDLK_PERIOD, {4, PivotAxis::ZP}},
    {SDLK_SLASH, {4, PivotAxis::ZN}}
  };

  const float rotation_accel = 0.01f;
  const float friction = 0.9f;

};

struct ObserveMode : Mode {
	ObserveMode();
	virtual ~ObserveMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	Scene::Camera const *current_camera = nullptr;

  std::vector< Scene::Transform * > pivots;
  std::vector< glm::vec3 > avels;

  std::vector< Scene::Transform * > targets;
  Scene::Transform *pointer;

  float score_elapsed = 0.0f;

};
