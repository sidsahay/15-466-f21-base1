#include "ThogMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include "read_write_chunk.hpp"
#include <fstream>

ThogMode::ThogMode() {

    std::vector< std::array<PPU466::Tile, 256> > _v;
    std::vector< PPU466::Palette > _p;

    std::ifstream tf("assets/tile.dat", std::ios::binary);
    read_chunk< std::array<PPU466::Tile, 256> >(tf, "tile", &_v);
    tf.close();

    std::ifstream pf("assets/palette.dat", std::ios::binary);
    read_chunk<PPU466::Palette>(pf, "pale", &_p);
    pf.close();

    ppu.tile_table = _v[0];
    ppu.palette_table[0] = _p[0];

    for (auto i = 0; i < ppu.background.size(); i++) {
        ppu.background[i] = 63;
    }

    ppu.tile_table[63].bit0 = {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    };
    ppu.tile_table[63].bit1 = {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    };
    player.x = 0;
    player.y = 0;
    for (auto frame = 0; frame < 5; frame++) {
        for (auto i = 0; i < 3; i++) {
            for (auto j = 0; j < 2; j++) {
                auto n = i * 2 + j + frame * 6;
                ppu.sprites[n].x = player.x + j * 8;
                ppu.sprites[n].y = player.y + i * 8;
                ppu.sprites[n].index = n;
                ppu.sprites[n].attributes = 0;
            }
        }
    }

    ppu.background_color = glm::u8vec4(0xff, 0xff, 0xff, 0xff);
    player.current_frame = 0;
    frame_delay = 0;
}

ThogMode::~ThogMode() {
}

bool ThogMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
    return false;
}

void ThogMode::update(float elapsed) {
    frame_delay += elapsed;
    if (frame_delay >= 0.1f) {
        frame_delay = 0;
        player.current_frame = (player.current_frame + 1) % 5;
        for (auto frame = 0; frame < 5; frame++) {
            for (auto i = 0; i < 3; i++) {
                for (auto j = 0; j < 2; j++) {
                    auto n = i * 2 + j + frame * 6;
                    if (frame == player.current_frame) {
                        ppu.sprites[n].x = player.x + j * 8;
                        ppu.sprites[n].y = player.y + i * 8;
                    }
                    else {
                        ppu.sprites[n].x = 200;
                        ppu.sprites[n].y = 240;
                    }
                }
            }
        }
    }
}

void ThogMode::draw(glm::uvec2 const &drawable_size) {

	ppu.draw(drawable_size);
}
