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

Load< Sound::Sample > noise(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("cold-dunes.opus"));
});

Load< SpriteAtlas > trade_font_atlas(LoadTagDefault, []() -> SpriteAtlas const * {
	return new SpriteAtlas(data_path("trade-font"));
});

GLuint meshes_for_lit_color_texture_program = 0;
static Load< MeshBuffer > meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer *ret = new MeshBuffer(data_path("robot.pnct"));
	meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

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
	return ret;
});

ObserveMode::ObserveMode() {
	assert(scene->cameras.size() && "Observe requires cameras.");

	current_camera = &scene->cameras.front();

  for (const char *c : Settings::names_pivots) {
    // find the transform in the scene
    for (Scene::Transform *t : scene->transforms) {
      if (t->name == c) {
        pivots.emplace_back(t);
        break;
      }
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
      //current_camera->transform->rotation.x = -current_camera->transform->position.x;
      //current_camera->transform->rotation.y = -current_camera->transform->position.y;
      //current_camera->transform->rotation.z = -current_camera->transform->position.z;
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
      //current_camera->transform->rotation.x = -current_camera->transform->position.x;
      //current_camera->transform->rotation.y = -current_camera->transform->position.y;
      //current_camera->transform->rotation.z = -current_camera->transform->position.z;
			return true;
		} else {
      auto it = Settings::key_mapping.find(evt.key.keysym.sym);
      if (it != Settings::key_mapping.end()) {
        Scene::Transform *t = pivots[it->second.index];
        glm::quat rot;
        switch (it->second.axis) {
          case Settings::PivotAxis::XP: {
            
            break;
          }
          case Settings::PivotAxis::XN: {

            break;
          }
          case Settings::PivotAxis::YP: {

            break;
          }
          case Settings::PivotAxis::YN: {

            break;
          }
          case Settings::PivotAxis::ZP: {

            break;
          }
          case Settings::PivotAxis::ZN: {

            break;
          }
        }
      }
    }
	}

	return false;
}

void ObserveMode::update(float elapsed) {

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
		DrawSprites draw(*trade_font_atlas, glm::vec2(0,0), glm::vec2(320, 200), drawable_size, DrawSprites::AlignPixelPerfect);

		std::string help_text = "--- SWITCH CAMERAS WITH LEFT/RIGHT ---";
		glm::vec2 min, max;
		draw.get_text_extents(help_text, glm::vec2(0.0f, 0.0f), 1.0f, &min, &max);
		float x = std::round(160.0f - (0.5f * (max.x + min.x)));
		draw.draw_text(help_text, glm::vec2(x, 1.0f), 1.0f, glm::u8vec4(0x00,0x00,0x00,0xff));
		draw.draw_text(help_text, glm::vec2(x, 2.0f), 1.0f, glm::u8vec4(0xff,0xff,0xff,0xff));
	}
}
