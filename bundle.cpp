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

#include "data_path.hpp"

int main(int argc, char** argv) {
    PPU466::Palette palette;
    std::array<PPU466::Tile, 16 * 16> tile_table;

    // load a player asset
    auto load_asset = [&](std::string file, uint32_t tile_offset) {
        std::string asset_file = file;
        glm::uvec2 size;
        std::vector<glm::u8vec4> data;
        
        // get the image from Blender
        load_png(asset_file, &size, &data, LowerLeftOrigin);
        //std::cout << size.x << " " << size.y << std::endl;

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

        //std::cout << unique_colors.size() << std::endl;
        for (const auto& color : unique_colors) {
            //std::cout << int(color.x) << " " << int(color.y) << " " << int(color.z) << " " << int(color.w) << std::endl;
        }

        // for color differences
        auto sq_color_diff = [](const glm::u8vec4& a, const glm::u8vec4& b) {
            float xd = float(a.x) - float(b.x);
            float yd = float(a.y) - float(b.y);
            float zd = float(a.z) - float(b.z);
            float wd = float(a.w) - float(b.w);

            return xd * xd + yd * yd + zd * zd + wd * wd;
        };

        // Blender render has slightly iffy colours because of sampling, cluster the colors to reduce to 4
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

        //std::cout << "lol\n";
        for (const auto& color : clustered_colors) {
            //std::cout << int(color.x) << " " << int(color.y) << " " << int(color.z) << " " << int(color.w) << std::endl;
        }

        std::map<size_t, uint8_t> color_map;

        // first run sets the palette so each frame uses the same palette
        if (tile_offset == 0) {
            palette = {
                clustered_colors[0],
                clustered_colors[1],
                clustered_colors[2],
                clustered_colors[3],
            };
        }

        // find color mapping
        for (size_t i = 0; i < data.size(); i++) {
            color_map[i] = 0;
            for (uint8_t j = 1; j < 4; j++) {
                if (sq_color_diff(data[i], palette[j]) < 1000.0f) {
                    color_map[i] = j;
                    break;
                }
            }
        }

        // update tile table
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

    // run player asset loading for all Blender renders
    std::string asset_file1 = data_path("../assets/rendered_frames/0001.png");
    std::string asset_file2 = data_path("../assets/rendered_frames/0002.png");
    std::string asset_file3 = data_path("../assets/rendered_frames/0003.png");
    std::string asset_file4 = data_path("../assets/rendered_frames/0004.png");
    std::string asset_file5 = data_path("../assets/rendered_frames/0005.png");
    std::string asset_file6 = data_path("../assets/rendered_frames/0006.png");
    std::string asset_file7 = data_path("../assets/rendered_frames/0007.png");
    std::string asset_file8 = data_path("../assets/rendered_frames/0008.png");
    std::string asset_file9 = data_path("../assets/rendered_frames/0009.png");
    std::string asset_file10 = data_path("../assets/rendered_frames/0010.png");
    std::string asset_file11 = data_path("../assets/rendered_frames/0011.png");
    std::string asset_file12 = data_path("../assets/rendered_frames/0012.png");
    std::string asset_file13 = data_path("../assets/rendered_frames/0013.png");
    std::string asset_file14 = data_path("../assets/rendered_frames/0014.png");
    std::string asset_file15 = data_path("../assets/rendered_frames/0015.png");
    std::string asset_file16 = data_path("../assets/rendered_frames/0016.png");
    std::string asset_file17 = data_path("../assets/rendered_frames/0017.png");
    std::string asset_file18 = data_path("../assets/rendered_frames/0018.png");
    std::string asset_file19 = data_path("../assets/rendered_frames/0019.png");
    std::string asset_file20 = data_path("../assets/rendered_frames/0020.png");
    std::string asset_file21 = data_path("../assets/rendered_frames/0021.png");
    std::string asset_file22 = data_path("../assets/rendered_frames/0022.png");
    std::string asset_file23 = data_path("../assets/rendered_frames/0023.png");
    std::string asset_file24 = data_path("../assets/rendered_frames/0024.png");
    std::string asset_file25 = data_path("../assets/rendered_frames/0025.png");
    std::string asset_file26 = data_path("../assets/rendered_frames/0026.png");

    load_asset(asset_file1, 0);
    load_asset(asset_file2, 6);
    load_asset(asset_file3, 12);
    load_asset(asset_file4, 18);
    load_asset(asset_file5, 24);
    load_asset(asset_file6, 30);
    load_asset(asset_file7, 36);
    load_asset(asset_file8, 42);
    load_asset(asset_file9, 48);
    load_asset(asset_file10, 54);
    load_asset(asset_file11, 60);
    load_asset(asset_file12, 66);
    load_asset(asset_file13, 72);

    load_asset(asset_file14, 78);
    load_asset(asset_file15, 84);
    load_asset(asset_file16, 90);
    load_asset(asset_file17, 96);
    load_asset(asset_file18, 102);
    load_asset(asset_file19, 108);
    load_asset(asset_file20, 114);
    load_asset(asset_file21, 120);
    load_asset(asset_file22, 126);
    load_asset(asset_file23, 132);
    load_asset(asset_file24, 138);
    load_asset(asset_file25, 144);
    load_asset(asset_file26, 150);
    
    // load a 8x8 asset
    PPU466::Palette palette_background;

    auto load_asset_background = [&](std::string file, uint32_t tile_offset) {
        std::string asset_file = file;
        glm::uvec2 size;
        std::vector<glm::u8vec4> data;
        
        // get the image from Blender
        load_png(asset_file, &size, &data, LowerLeftOrigin);
        //std::cout << size.x << " " << size.y << std::endl;

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

        //std::cout << unique_colors.size() << std::endl;
        for (const auto& color : unique_colors) {
            //std::cout << int(color.x) << " " << int(color.y) << " " << int(color.z) << " " << int(color.w) << std::endl;
        }

        // for color differences
        auto sq_color_diff = [](const glm::u8vec4& a, const glm::u8vec4& b) {
            float xd = float(a.x) - float(b.x);
            float yd = float(a.y) - float(b.y);
            float zd = float(a.z) - float(b.z);
            float wd = float(a.w) - float(b.w);

            return xd * xd + yd * yd + zd * zd + wd * wd;
        };

        // Blender render has slightly iffy colours because of sampling, cluster the colors to reduce to 4
        auto cluster_colors = [sq_color_diff](const std::vector<glm::u8vec4>& data, std::vector<glm::u8vec4>* cluster_) {
            auto& cluster = *cluster_;
            for (const auto& pixel : data) {
                bool found = false;
                for (const auto& color : cluster) {
                    if (sq_color_diff(pixel, color) < 100.f) {
                        found = true;
                    }
                }
                if (!found) {
                    cluster.push_back(pixel);
                }
            }
        };

        std::vector<glm::u8vec4> clustered_colors;
        cluster_colors(data, &clustered_colors);
        assert(clustered_colors.size() == 4);
        std::map<size_t, uint8_t> color_map;

        // first run sets the palette so each frame uses the same palette
        palette_background = {
            clustered_colors[0],
            clustered_colors[1],
            clustered_colors[2],
            clustered_colors[3],
        };
    

        // find color mapping
        for (size_t i = 0; i < data.size(); i++) {
            color_map[i] = 0;
            for (uint8_t j = 1; j < 4; j++) {
                if (sq_color_diff(data[i], palette_background[j]) < 100.0f) {
                    color_map[i] = j;
                    //std::cout << "found" << std::endl;
                    break;
                }
            }
        }

        // update tile table
        for (uint8_t i = 0; i < 8; i++) {
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

    std::string asset_file_background = data_path("../assets/background.png");
    load_asset_background(asset_file_background, 156);

    for (int i = 0; i < 4; i++) {
        //std::cout << int(palette_background[i].x) << " " << int(palette_background[i].y) << int(palette_background[i].z) << int(palette_background[i].w) << std::endl;
    }
    // ugh
    std::vector<std::array<PPU466::Tile, 16 * 16>> _v;
    _v.push_back(tile_table);
    std::vector<PPU466::Palette> _p;

    // translucent blue-ish palette for AI
    PPU466::Palette ai_palette = {
        palette[0],
        glm::u8vec4(palette[1].x, palette[1].y, 0xff, 0x88),
        glm::u8vec4(palette[2].x, palette[2].y, 0xff, 0x88),
        glm::u8vec4(palette[3].x, palette[3].y, 0xff, 0x88),
    };

    _p.push_back(palette);
    _p.push_back(ai_palette);
    _p.push_back(palette_background);

    // write binary file for the game to read
    std::ofstream tf(data_path("../assets/tile.dat").c_str(), std::ios::binary);
    write_chunk< std::array<PPU466::Tile, 16 * 16> >("tile", _v, &tf);
    tf.close();

    std::ofstream pf(data_path("../assets/palette.dat").c_str(), std::ios::binary);
    write_chunk<PPU466::Palette>("pale", _p, &pf);
    pf.close();



}