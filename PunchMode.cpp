#include "PunchMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include "read_write_chunk.hpp"
#include <fstream>
#include "data_path.hpp"

PunchMode::PunchMode() {

    static std::mt19937 mt;
    std::vector< std::array<PPU466::Tile, 256> > _v;
    std::vector< PPU466::Palette > _p;

    // load tiles
    std::ifstream tf(data_path("../assets/tile.dat").c_str(), std::ios::binary);
    read_chunk< std::array<PPU466::Tile, 256> >(tf, "tile", &_v);
    tf.close();

    // load palette
    std::ifstream pf(data_path("../assets/palette.dat").c_str(), std::ios::binary);
    read_chunk<PPU466::Palette>(pf, "pale", &_p);
    pf.close();

    ppu.tile_table = _v[0];
    ppu.palette_table[0] = _p[0];
    ppu.palette_table[1] = _p[1];
    ppu.palette_table[2] = _p[2];

    for (auto i = 0; i < ppu.background.size(); i++) {
        ppu.background[i] = 157 | 0b0000001000000000;
    }

    // ppu.tile_table[255].bit0 = {
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    // };
    // ppu.tile_table[255].bit1 = {
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    //     0b00000000,
    // };

    // ppu.tile_table[254].bit0 = {
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    // };

    // ppu.tile_table[254].bit1 = {
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    //     0b11111111,
    // };

    // init player
    player.current_frame = 0;
    player.sprite_offset = 0;
    player.palette_idx = 0;
    player.do_jump = false;
    player.state = PlayerState::GROUND;
    player.direction = 0;

    // create 5 AIs
    for (int i = 0; i < 5; i++) {
        ais.emplace_back();
    }
    
    // AI reaction times
    std::array<float, 5> punch_times = {3.0f, 2.0f, 1.7f, 1.3f, 1.0f};

    // init AIs
    for (int i = 0; i < ais.size(); i++) {
        ais[i].position.y = float(32 * (i+1) + 8);
        ais[i].position.x = (mt()/float(mt.max())) * 240;
        ais[i].direction = 1;
        ais[i].current_frame = 0;
        ais[i].sprite_offset = 6 * (i+1);
        ais[i].palette_idx = 1;
        ais[i].do_jump = false;
        ais[i].state = PlayerState::GROUND;
        ais[i].punch_time_max = punch_times[i];
    }

    for (int i = 0; i < 64; i++) {
        ppu.sprites[i].attributes = 0;
    }

    ppu.background_color = glm::u8vec4(0xff, 0xff, 0xff, 0xff);
    frame_delay = 0;

    // create floors
    for (int i = 0; i < 5; i++) {
        boxes.emplace_back(32 * (i+1));
    }

    for (const auto& box : boxes) {
        uint32_t i = box/8;
        for (int j = 0; j < 64; j++) {
            ppu.background[i * 64 + j] = 156 | 0b0000001000000000;
        }
    }
}

PunchMode::~PunchMode() {
}

bool PunchMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
    if (evt.type == SDL_KEYDOWN) {
        if (evt.key.keysym.sym == SDLK_SPACE) {
            if (player.state == PlayerState::GROUND) {
                player.do_jump = true;
            }
        }
        else if (evt.key.keysym.sym == SDLK_d) {
            player.velocity.x = 30.0f;
            player.direction = 0;
        }
        else if (evt.key.keysym.sym == SDLK_a) {
            player.velocity.x = -30.0f;
            player.direction = 1;
        }
        else if (evt.key.keysym.sym == SDLK_v) {
            if (player.state == PlayerState::GROUND) {
                player.do_punch = true;
            }
        }
    }
    else if (evt.type == SDL_KEYUP) {
        if (evt.key.keysym.sym == SDLK_d) {
            player.velocity.x = 0;
        }
        else if (evt.key.keysym.sym == SDLK_a) {
            player.velocity.x = 0;
        }
    }

    return false;
}

void PunchMode::update(float elapsed) {
    frame_delay += elapsed;
    
    // horrible AI code
    for (auto& ai : ais) {
        // face the player
        if (fabs(ai.position.y - player.position.y) < 0.5f && ai.state != PlayerState::DEATH_2) {
            ai.direction = ai.position.x >= player.position.x;
        }

        // start punch timer if player is close
        if (fabs(ai.position.y - player.position.y) < 0.5f && ai.state != PlayerState::DEATH_2 && (!ai.is_punching)) {
            if (fabs(player.position.x - ai.position.x) < 20.0f) {
                ai.is_punching = true;
                ai.punch_time = ai.punch_time_max;
            }
        }

        // update punch timer
        if (ai.is_punching) {
            ai.punch_time -= elapsed;
        }

        // if timer runs out, do punch
        if (ai.is_punching && ai.punch_time <= 0.0f) {
            ai.do_punch = true;
            ai.is_punching = false;
        }
    }

    player.update(elapsed, gravity, boxes);

    for (auto& ai : ais) {
        ai.update(elapsed, gravity, boxes);
    }

    // graphics are done at 10fps to match low FPS animation
    if (frame_delay >= 0.1f) {
        frame_delay = 0;
        player.advance_frame(&ppu);
        for (auto& ai : ais) {
            ai.advance_frame(&ppu);
        }
    }

    // hit detection - VERY BUGGY
    for (auto& ai : ais) {
        if (player.state == PlayerState::PUNCH_1) {
            auto hitx = player.direction ? player.position.x - 8 : player.position.x + 8;
            if (hitx >= ai.position.x && hitx <= (ai.position.x + 16) && fabs(ai.position.y - player.position.y) < 1.0f) {
                ai.do_hit = true;
            }
        }

        if (ai.state == PlayerState::PUNCH_1) {
            auto hitx = ai.direction ? ai.position.x - 8 : ai.position.x + 8;
            if (hitx >= player.position.x && hitx <= (player.position.x + 16) && fabs(ai.position.y - player.position.y) < 1.0f) {
                player.do_hit = true;
            }
        }
    }
}

void PunchMode::Player::update(float elapsed, const glm::vec2& gravity, const std::vector<uint32_t>& boxes) {
    // state machines to run player updates and animations
    switch (state) {
        case PlayerState::GROUND:
            if (do_jump) {
                state = PlayerState::JUMP_WINDUP;
                velocity.y = 80.0f;
                do_jump = false;
            }
            else if (do_punch) {
                state = PlayerState::PUNCH_1;
                // velocity.x = 0;
                // velocity.y = 0;
                do_punch = false;
            }
            else if (do_hit) {
                state = PlayerState::DEATH_1;
                velocity.x = 0;
                velocity.y = 0;
                do_hit = false;
            }
            position += velocity * elapsed;
            break;

        case PlayerState::JUMP_WINDUP:
            //state = PlayerState::JUMPING; // transition done at next frame
            break;

        case PlayerState::JUMPING:
            velocity += gravity * elapsed;    
            position += velocity * elapsed;
            break;
    }
    
    do_jump = false;
    do_punch = false;
    do_hit = false;

    // handle collisions
    switch (state) {
        case PlayerState::JUMPING:
            if (position.y <= 0.0f) {
                position.y = 0.0f;
                velocity.y = 0.0f;
                state = PlayerState::LANDING;
            }
            else {
                for (const auto& box : boxes) {
                    auto lower_y = position.y;
                    auto x = position.x + 8;
                    if (lower_y >= box && lower_y <= (box + 8) && velocity.y <= 0.0f) {
                        position.y = float(box + 8);
                        velocity.y = 0;
                        state = PlayerState::LANDING;
                        break;
                    }
                }
            }
            break;
    }
}

void PunchMode::Player::advance_frame(PPU466* ppu_) {
    auto& ppu = *ppu_;

    // state machine for animations
    switch (state) {
        case PlayerState::GROUND:
            if (velocity.x == 0) {
                current_frame = 9;
            }
            else {
                current_frame = (current_frame + 1) % 5;
            }
            break;

        case PlayerState::JUMP_WINDUP:
            current_frame = 5;
            state = PlayerState::JUMPING;
            break;

        case PlayerState::JUMPING:
            if (velocity.y >= 0.0f) {
                current_frame = 6;
            }
            else {
                current_frame = 7;
            }
            break;

        case PlayerState::LANDING:
            current_frame = 8;
            state = PlayerState::GROUND;
            break;
        
        case PlayerState::PUNCH_1:
            current_frame = 10;
            state = PlayerState::PUNCH_2;
            break;
        
        case PlayerState::PUNCH_2:
            current_frame = 10;
            state = PlayerState::GROUND;
            break;

        case PlayerState::DEATH_1:
            current_frame = 11;
            state = PlayerState::DEATH_2;
            break;
        
        case PlayerState::DEATH_2:
            current_frame = 12;
            break;
    }

    // set properties of the player sprite collection (6 sprites per player model)
    for (auto i = 0; i < 3; i++) {
        for (auto j = 0; j < 2; j++) {
            auto n = sprite_offset + i * 2 + j;
            ppu.sprites[n].x = uint32_t(position.x) + j * 8;
            ppu.sprites[n].y = uint32_t(position.y) + i * 8;
            ppu.sprites[n].index = n + 6 * (current_frame + direction * 13) - sprite_offset; // all players have same sprites
            ppu.sprites[n].attributes = palette_idx;
        }
    }
}

void PunchMode::draw(glm::uvec2 const &drawable_size) {
	ppu.draw(drawable_size);
}
