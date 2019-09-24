#include "ObserveMode.hpp"
#include "DrawLines.hpp"
#include "LitColorTextureProgram.hpp"
#include "Mesh.hpp"
#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "data_path.hpp"
#include "Sound.hpp"

#include <iostream>
#include <cmath>

Load< SpriteAtlas > trade_font_atlas(LoadTagDefault, []() -> SpriteAtlas const * {
	return new SpriteAtlas(data_path("trade-font"));
});

GLuint meshes_for_lit_color_texture_program = 0;
static Load< MeshBuffer > meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer *ret = new MeshBuffer(data_path("robot.pnct"));
	meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

static Scene *scene_ptr = nullptr;

static Load< Scene > scene(LoadTagLate, []() -> Scene const * {
	Scene *ret = new Scene();
	ret->load(data_path("robot.scene"), [](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		auto &mesh = meshes->lookup(mesh_name);
		scene.drawables.emplace_back(transform);
		Scene::Drawable::Pipeline &pipeline = scene.drawables.back().pipeline;

		pipeline = lit_color_texture_program_pipeline;
		pipeline.vao = meshes_for_lit_color_texture_program;
		pipeline.type = mesh.type;
		pipeline.start = mesh.start;
		pipeline.count = mesh.count;
	});
  scene_ptr = ret;
	return ret;
});

ObserveMode::ObserveMode() {

	assert(scene->cameras.size() && "Observe requires cameras.");

	current_camera = &scene->cameras.front();

  for (const char *c : ObserveModeSettings::names_pivots) {
    // find the transform in the scene
    for (auto it = scene_ptr->transforms.begin(); it != scene->transforms.end(); it++) {
      if (it->name == c) {
        pivots.push_back(&(*it));
        avels.emplace_back(0.0f, 0.0f, 0.0f);
        break;
      }
    }
  }

  for (const char *c : ObserveModeSettings::names_targets) {
    // find the transform in the scene
    for (auto it = scene_ptr->transforms.begin(); it != scene->transforms.end(); it++) {
      if (it->name == c) {
        targets.push_back(&(*it));
        break;
      }
    }
  }

  for (auto it = scene_ptr->transforms.begin(); it != scene->transforms.end(); it++) {
    if (it->name == ObserveModeSettings::name_point) {
      pointer = (&(*it));
      break;
    }
  }

}

ObserveMode::~ObserveMode() {

}

bool ObserveMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_SPACE) {
      int i = 0;
      for (const Scene::Drawable &d : scene->drawables) {
        std::cout << "Drawable " << i << std::endl;
        std::cout << d.transform->name << std::endl;
        glm::vec3 pos = d.transform->position;
        std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;
        i++;
      }
      return true;
    }
		else if (evt.key.keysym.sym == SDLK_LEFT) {
      float xi, yi, r, angle;
      xi = current_camera->transform->position.x;
      yi = current_camera->transform->position.y;
      r = sqrt(xi * xi + yi * yi);
      angle = atan2(yi, xi);
      angle -= 0.05f;
      current_camera->transform->position.x = r * cos(angle);
      current_camera->transform->position.y = r * sin(angle);

      glm::quat rot = glm::quat(cos(angle / 2.0f), 0.0f, 0.0f, sin(angle / 2.0f));
      current_camera->transform->rotation = rot * current_camera->transform->rotation;

      return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
      float xi, yi, r, angle;
      xi = current_camera->transform->position.x;
      yi = current_camera->transform->position.y;
      r = sqrt(xi * xi + yi * yi);
      angle = atan2(yi, xi);
      angle += 0.05f;
      current_camera->transform->position.x = r * cos(angle);
      current_camera->transform->position.y = r * sin(angle);

      glm::quat rot = glm::quat(cos(angle / 2.0f), 0.0f, 0.0f, -sin(angle / 2.0f));
      current_camera->transform->rotation = rot * current_camera->transform->rotation;

      return true;
		} else {
      auto it = ObserveModeSettings::key_mapping.find(evt.key.keysym.sym);
      if (it != ObserveModeSettings::key_mapping.end()) {
        switch (it->second.axis) {
          case ObserveModeSettings::PivotAxis::XP: {
            avels[it->second.index].x += ObserveModeSettings::rotation_accel;
            break;
          }
          case ObserveModeSettings::PivotAxis::XN: {
            avels[it->second.index].x -= ObserveModeSettings::rotation_accel;
            break;
          }
          case ObserveModeSettings::PivotAxis::YP: {
            avels[it->second.index].y += ObserveModeSettings::rotation_accel;
            break;
          }
          case ObserveModeSettings::PivotAxis::YN: {
            avels[it->second.index].y -= ObserveModeSettings::rotation_accel;
            break;
          }
          case ObserveModeSettings::PivotAxis::ZP: {
            avels[it->second.index].z += ObserveModeSettings::rotation_accel;
            break;
          }
          case ObserveModeSettings::PivotAxis::ZN: {
            avels[it->second.index].z -= ObserveModeSettings::rotation_accel;
            break;
          }
        }
      }
    }
	}

	return false;
}

void ObserveMode::update(float elapsed) {

  if (!targets.empty()) score_elapsed += elapsed;

  // update friction, position
  for (uint32_t i = 0; i < pivots.size(); i++) {

    avels[i] *= ObserveModeSettings::friction;
    glm::quat rot = glm::quat(cos(avels[i].x), sin(avels[i].x), 0.0f, 0.0f);
    rot = glm::quat(cos(avels[i].y), 0.0f, sin(avels[i].y), 0.0f) * rot;
    rot = glm::quat(cos(avels[i].z), 0.0f, 0.0f, sin(avels[i].z)) * rot;
    pivots[i]->rotation = rot * pivots[i]->rotation;

  }

  // collision detection
  glm::vec4 pt_pos = pointer->make_local_to_world() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  auto it = targets.begin();
  for (; it != targets.end(); it++) {
    Scene::Transform *t = *it;
    glm::vec4 tgt_pos = t->make_local_to_world() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 dist = tgt_pos - pt_pos;
    float r2 = dist.x * dist.x + dist.y * dist.y + dist.z * dist.z;
    if (r2 < 1.0f) {
      (*it)->scale = glm::vec3(0.01f, 0.01f, 0.01f);
      break;
    }
  }
  if (it != targets.end()) {
    targets.erase(it);
    std::cout << "Hit target!" << std::endl;
  }
}


void ObserveMode::draw(glm::uvec2 const &drawable_size) {
	//--- actual drawing ---
	glClearColor(0.85f, 0.85f, 0.90f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	const_cast< Scene::Camera * >(current_camera)->aspect = drawable_size.x / float(drawable_size.y);

	scene->draw(*current_camera);

	{ //help text overlay:
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		DrawSprites draw(*trade_font_atlas, glm::vec2(0,0), drawable_size, drawable_size, DrawSprites::AlignPixelPerfect);

    glm::vec2 min, max;
    float x, y;

    std::string time_text = std::to_string(static_cast<int>(score_elapsed));
		draw.get_text_extents(time_text, glm::vec2(0.0f, 0.0f), 1.0f, &min, &max);
		x = std::round(drawable_size.x - 20.0f - (max.x - min.x));
    y = 20.0f;
		draw.draw_text(time_text, glm::vec2(x, y), 1.0f, glm::u8vec4(0x00,0x00,0x00,0xff));
		draw.draw_text(time_text, glm::vec2(x, y), 1.0f, glm::u8vec4(0xff,0x00,0x00,0xff));

    std::string remain_text = "Targets left: " + std::to_string(targets.size());
		draw.get_text_extents(remain_text, glm::vec2(0.0f, 0.0f), 1.0f, &min, &max);
		x = std::round(drawable_size.x - 20.0f - (max.x - min.x));
    y = std::round(drawable_size.y - 20.0f - (max.y - min.y));
		draw.draw_text(remain_text, glm::vec2(x, y), 1.0f, glm::u8vec4(0x00,0x00,0x00,0xff));
		draw.draw_text(remain_text, glm::vec2(x, y), 1.0f, glm::u8vec4(0xff,0x00,0x00,0xff));

	}
}
