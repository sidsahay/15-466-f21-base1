#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>

#include "load_save_png.hpp"


struct ThogMode : Mode {
	ThogMode();
	virtual ~ThogMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	struct Player {
		uint32_t x;
		uint32_t y;
		uint32_t current_frame;
	} player;
	float frame_delay;
	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
