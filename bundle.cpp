#include <iostream>
#include "PPU466.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>
#include <array>
#include <fstream>

#include "load_save_png.hpp"
#include "read_write_chunk.hpp"

int main(int argc, char** argv) {
    PPU466::Palette palette;
    std::array<PPU466::Tile, 16 * 16> tile_table;

    auto load_asset = [&](std::string file, uint32_t tile_offset) {
        std::string asset_file = file;
        glm::uvec2 size;
        std::vector<glm::u8vec4> data;
        
        load_png(asset_file, &size, &data, LowerLeftOrigin);
        std::cout << size.x << " " << size.y << std::endl;

        std::vector<glm::u8vec4> unique_colors;

        for (const auto& pixel : data) {
            bool found = false;
            for (const auto& color : unique_colors) {
                if (pixel == color) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                unique_colors.push_back(pixel);
            }
        }

        std::cout << unique_colors.size() << std::endl;
        for (const auto& color : unique_colors) {
            std::cout << int(color.x) << " " << int(color.y) << " " << int(color.z) << " " << int(color.w) << std::endl;
        }

        auto sq_color_diff = [](const glm::u8vec4& a, const glm::u8vec4& b) {
            float xd = float(a.x) - float(b.x);
            float yd = float(a.y) - float(b.y);
            float zd = float(a.z) - float(b.z);
            float wd = float(a.w) - float(b.w);

            return xd * xd + yd * yd + zd * zd + wd * wd;
        };

        // the blender render has slightly iffy colours because of sampling, cluster the colors
        auto cluster_colors = [sq_color_diff](const std::vector<glm::u8vec4>& data, std::vector<glm::u8vec4>* cluster_) {
            auto& cluster = *cluster_;
            for (const auto& pixel : data) {
                bool found = false;
                for (const auto& color : cluster) {
                    if (sq_color_diff(pixel, color) < 1000.f) {
                        found = true;
                    }
                }
                if (!found) {
                    cluster.push_back(pixel);
                }
            }
        };

        std::vector<glm::u8vec4> clustered_colors;
        clustered_colors.emplace_back(0x00, 0x00, 0x00, 0x00);
        cluster_colors(data, &clustered_colors);
        assert(clustered_colors.size() == 4);

        std::cout << "lmao\n";
        for (const auto& color : clustered_colors) {
            std::cout << int(color.x) << " " << int(color.y) << " " << int(color.z) << " " << int(color.w) << std::endl;
        }

        std::map<size_t, uint8_t> color_map;

        if (tile_offset == 0) {
            palette = {
                clustered_colors[0],
                clustered_colors[1],
                clustered_colors[2],
                clustered_colors[3],
            };
        }

        for (size_t i = 0; i < data.size(); i++) {
            color_map[i] = 0;
            for (uint8_t j = 1; j < 4; j++) {
                if (sq_color_diff(data[i], palette[j]) < 1000.0f) {
                    color_map[i] = j;
                    break;
                }
            }
        }

        for (uint8_t i = 0; i < 24; i++) {
            for (uint8_t j = 0; j < 16; j++) {
                uint8_t c = color_map[i*size.x + j];
                uint8_t jj = j % 8;
                uint8_t ii = i % 8;
                uint32_t n = tile_offset + (i/8) * 2 + (j/8);
                tile_table[n].bit0[ii] &= ~(1u << jj);
                tile_table[n].bit1[ii] &= ~(1u << jj);
                tile_table[n].bit0[ii] |= (c & 1) << jj;
                tile_table[n].bit1[ii] |= ((c >> 1) & 1) << jj;
            }
        }
    };

    std::string asset_file1 = "assets/rendered_frames/0001.png";
    std::string asset_file2 = "assets/rendered_frames/0002.png";
    std::string asset_file3 = "assets/rendered_frames/0003.png";
    std::string asset_file4 = "assets/rendered_frames/0004.png";
    std::string asset_file5 = "assets/rendered_frames/0005.png";
    load_asset(asset_file1, 0);
    load_asset(asset_file2, 6);
    load_asset(asset_file3, 12);
    load_asset(asset_file4, 18);
    load_asset(asset_file5, 24);

    // ugh
    std::vector<std::array<PPU466::Tile, 16 * 16>> _v;
    _v.push_back(tile_table);
    std::vector<PPU466::Palette> _p;
    _p.push_back(palette);

    std::ofstream tf("assets/tile.dat", std::ios::binary);
    write_chunk< std::array<PPU466::Tile, 16 * 16> >("tile", _v, &tf);
    tf.close();

    std::ofstream pf("assets/palette.dat", std::ios::binary);
    write_chunk<PPU466::Palette>("pale", _p, &pf);
    pf.close();

}