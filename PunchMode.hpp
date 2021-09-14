#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>

#include "load_save_png.hpp"


struct PunchMode : Mode {
	PunchMode();
	virtual ~PunchMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	enum class PlayerState {
		GROUND,
		JUMP_WINDUP,
		JUMPING,
		LANDING,
		PUNCH_1,
		PUNCH_2,
		DEATH_1,
		DEATH_2
	};
	
	struct Player {
		uint32_t current_frame;
		uint32_t palette_idx;
		uint32_t sprite_offset;
		uint32_t direction;
		glm::vec2 position = glm::vec2(0, 0);
		glm::vec2 velocity = glm::vec2(0, 0);
		float punch_time;
		float punch_time_max;
		bool is_punching;
		bool do_jump;
		bool do_punch;
		bool do_hit;
		PlayerState state;
		void update(float elapsed, const glm::vec2& gravity, const std::vector<uint32_t>& boxes);
		void advance_frame(PPU466* ppu);
	} player;

	std::vector<Player> ais;

	float frame_delay;
	glm::vec2 gravity = glm::vec2(0, -70.0f);

	std::vector<uint32_t> boxes;
	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
