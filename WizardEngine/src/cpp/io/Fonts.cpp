//
// Created by mecha on 28.03.2022.
//

#include <io/Fonts.h>
#include <io/Logger.h>
#include <io/BitmapFile.h>
#include <core/filesystem.h>
#include <math/Math.h>
#include <platform/graphics/TextureBuffer.h>

namespace engine::io {

    using namespace graphics;

    u32 Fonts::create(
            const char* fontPath,
            const u32 &fontSize,
            const char* bitmapPath,
            const char* widthsPath
    ) {
        FT_Library lib;
        FT_Error error;
        FT_Face face;
        FT_UInt glyphIndex;

        // init freetype
        error = FT_Init_FreeType(&lib);
        if (error != FT_Err_Ok) {
            ENGINE_ERR("Fonts : failed to load FreeType library : error {0}", error);
            return false;
        }

        // load font
        error = FT_New_Face(lib, fontPath, 0, &face);
        if (error == FT_Err_Unknown_File_Format) {
            ENGINE_ERR("Fonts : failed to open a file {0} , unknown file format!", fontPath);
            return false;
        } else if (error) {
            ENGINE_ERR("Fonts : failed to open a file {0} , error : {1}", fontPath, error);
            return false;
        }

        // set font size
        error = FT_Set_Pixel_Sizes(face, 0, fontSize);
        if (error) {
            ENGINE_ERR("Fonts : failed to set a font size : {0}, error : {1}", fontSize, error);
        }

        // create bitmap font
        int imageWidth = (fontSize + 2) * 16;
        int imageHeight = (fontSize + 2) * 8;

        // create a buffer for the bitmap
        auto* buffer = new uint8_t[imageWidth * imageHeight * 4];
        memset(buffer, 0,imageWidth * imageHeight * 4);

        // create an array to save the character widths
        int* widths = new int[128];

        // we need to find the character that goes below the baseline by the biggest value
        int maxUnderBaseline = 0;
        for (int i = 32; i < 127; ++i) {
            // get the glyph index from character code
            glyphIndex = FT_Get_Char_Index(face, i);

            // load the glyph image into the slot
            error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
            if (error) {
                ENGINE_ERR("Fonts : failed to load a glyph, error : {0}", error);
                continue;
            }

            // get the glyph metrics
            const FT_Glyph_Metrics& glyphMetrics = face->glyph->metrics;

            // find the character that reaches below the baseline by the biggest value
            int glyphHang = (glyphMetrics.horiBearingY - glyphMetrics.height) / 64;
            if (glyphHang < maxUnderBaseline) {
                maxUnderBaseline = glyphHang;
            }
        }

        Characters characters;
        // draw all characters
        for (int i = 0; i < 128; ++i) {
            // get the glyph index from character code
            glyphIndex = FT_Get_Char_Index(face, i);

            // load the glyph image into the slot
            error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
            if (error) {
                ENGINE_ERR("Fonts : failed to load a glyph, error : {0}", error);
                continue;
            }

            // convert to an anti-aliased bitmap
            error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            if (error) {
                ENGINE_ERR("Fonts : failed to render a glyph, error : {0}", error);
            }

            // save the character width
            auto w = face->glyph->metrics.width / 64;
            auto h = face->glyph->metrics.height / 64;
            vec2f bearing = { static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top) };
            int advance = face->glyph->advance.x;
            widths[i] = w;
            // find the tile position where we have to draw the character
            int x = (i % 16) * (fontSize + 2);
            int y = (i / 16) * (fontSize + 2);
            x += 1; // 1 pixel padding from the left side of the tile
            y += (fontSize + 2) - face->glyph->bitmap_top + maxUnderBaseline - 1;
            // store metrics data into Character struct
            vec2f imageSize = { static_cast<float>(imageWidth), static_cast<float>(imageHeight) };
            auto* charVertices = new BatchCharVertex[4] {
                    BatchCharVertex { CharVertex {
                        { 0, 0 },
                        (vec2f { static_cast<f32>(x), static_cast<f32>(y) } * 2.0f - 1.0f) / (imageSize * 2.0f)
                    } },
                    BatchCharVertex { CharVertex {
                        { 0, 0 },
                        (vec2f { static_cast<float>(x + w), static_cast<float>(y) } * 2.0f - 1.0f) / (imageSize * 2.0f)
                    } },
                    BatchCharVertex { CharVertex {
                        { 0, 0 },
                        (vec2f { static_cast<float>(x + w), static_cast<float>(y + h) } * 2.0f - 1.0f) / (imageSize * 2.0f)
                    } },
                    BatchCharVertex { CharVertex {
                        { 0, 0 },
                        (vec2f { static_cast<float>(x), static_cast<float>(y + h) } * 2.0f - 1.0f) / (imageSize * 2.0f)
                    } }
            };
            auto charVertexData = array<BatchCharVertex> { charVertices, 0, 4 };
            auto charVertexDataComponent = VertexDataComponent<BatchCharVertex>();
            charVertexDataComponent.vertexData = charVertexData;
            auto character = Character {
                charVertexDataComponent,
                vec2f { static_cast<float>(w), static_cast<float>(h) } / imageSize,
                bearing / imageSize,
                (float)advance / (float)imageWidth
            };
            // map char and appropriate Character struct
            characters.insert(std::pair<char, Character>(i, character));
            ENGINE_INFO("Fonts : mapping char '{0}' into Character : ", i);
            for (int j = 0; j < charVertexData.size; j++) {
                ENGINE_INFO("CharVertex[x : {0}, y : {1}]",
                            charVertices[j].vertex.uv.x(),
                            charVertices[j].vertex.uv.y());
            }
            ENGINE_INFO("Character[size : {0} , {1}]", character.size.x(), character.size.y());
            // draw the character
            const FT_Bitmap& bitmap = face->glyph->bitmap;
            for (int xx = 0; xx < bitmap.width; ++xx) {
                for (int yy = 0; yy < bitmap.rows; ++yy) {
                    uint8_t r = bitmap.buffer[(yy * (bitmap.width) + xx)];
                    int z = (y + yy) * imageWidth * 4 + (x + xx) * 4;
                    buffer[z] = r;
                    buffer[z + 1] = r;
                    buffer[z + 2] = r;
                    buffer[z + 3] = 255;
                }
            }
        }
        // write into .bmp file
        int paddedSize = 0;
        BitmapFile::write(bitmapPath, { imageWidth, imageHeight, buffer, &paddedSize });
        delete[] buffer;
        // map font file path with characters map
        TextureData td = TextureFile::read(bitmapPath);
        u32 fontTextureId = TextureBuffer::upload(td);
        fonts.insert(std::pair<u32, Characters>(fontTextureId, characters));
        // write into widths .txt file
        filesystem::write(widthsPath, widths, 128);
        delete[] widths;
        // shutdown freetype
        error = FT_Done_FreeType(lib);
        if (error != FT_Err_Ok) {
            ENGINE_ERR("Fonts : failed to close FreeType library, error : {0}", error);
            return fontTextureId;
        }

        return fontTextureId;
    }

    Characters &Fonts::getFont(const u32& id) {
        return fonts[id];
    }

    Character &Fonts::getCharacter(const u32& id, const char &c) {
        return fonts[id][c];
    }

    void Fonts::clear() {
        fonts.clear();
    }

    bool Fonts::exists(const u32& id) {
        return fonts.find(id) != fonts.end();
    }

}