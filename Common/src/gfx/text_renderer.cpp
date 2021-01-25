#include "EGL/eglInstance.h"
#include "ES/program.h"
#include "ES/program_create_info.h"
#include "ES/shader.h"
#include "ES/shader_create_info.h"
#include "ES/texture.h"
#include "ES/texture_create_info.h"
#include "gfx/text_renderer.h"
#include "ImageFormats/bmp.h"
#include "io.h"
#include "logger.h"

#include <psp2/libdbg.h>
#include <string.h>
#include <vector>

#define N_CHARACTER_PROPS_PTR_TO_PREALLOC (64)

static const char* g_fs_cg = "float4 main(in         float2    in_uv           : TEXCOORD0,\n"
                             "               uniform sampler2D in_font_texture)\n"
                             "{\n"
                             "    return tex2D(in_font_texture, in_uv);\n"
                             "}\n";
static const char* g_vs_cg = "void main(in          int    in_n_vertex     : TEXCOORD0,\n"
                             "          in          int    in_n_instance   : TEXCOORD1,\n"
                             "              uniform float4 in_src_u1v1u2v2,\n"
                             "              uniform float2 in_dst_wh,\n"
                             "              uniform float2 in_dst_x1y1[64],\n"
                             "          out         float4 out_position    : POSITION,\n"
                             "          out         float2 out_uv          : TEXCOORD0)\n"
                             "{\n"
                             "    out_position = float4(in_n_vertex < 2, (in_n_vertex == 1) || (in_n_vertex == 3), 0, 1);\n"
                             "\n"
                             "    out_uv.xy        = in_src_u1v1u2v2.xy + (in_src_u1v1u2v2.zw - in_src_u1v1u2v2.xy) * out_position.xy;\n"
                             "    out_position.xy *= in_dst_wh;\n"
                             "    out_position.xy += in_dst_x1y1[in_n_instance].xy;\n"
                             "}";

TextRenderer::DATHeader::DATHeader()
{
    memset(this,
           0,
           sizeof(*this) );
}

TextRenderer::TextCharacterProps::TextCharacterProps()
{
    memset(this,
           0,
           sizeof(*this) );
}

void TextRenderer::TextLayer::reset()
{
    for (auto& item_iterator : n_char_to_text_character_props_vec)
    {
        for (auto& item_iterator2 : item_iterator.second)
        {
            available_character_props_ptr_vec_ptr->push_back(item_iterator2);
        }
    }

    n_char_to_text_character_props_vec.clear();
}

TextRenderer::TextRenderer(const EGLInstance* in_egl_instance_ptr,
                           Logger*            in_logger_ptr)
    :m_egl_instance_ptr     (in_egl_instance_ptr),
     m_logger_ptr           (in_logger_ptr),
     m_n_text_layers_created(0)
{
    /* Stub */
}

void TextRenderer::bake_text_character_props_batch()
{
    m_prealloced_character_props_ptr_vec.resize(m_prealloced_character_props_ptr_vec.size() + N_CHARACTER_PROPS_PTR_TO_PREALLOC);

    for (uint32_t n_instance = 0;
                  n_instance < N_CHARACTER_PROPS_PTR_TO_PREALLOC;
                ++n_instance)
    {
        std::unique_ptr<TextCharacterProps> new_item_ptr(new TextCharacterProps() );

        m_available_character_props_ptr_vec.push_back(new_item_ptr.get() );
        m_prealloced_character_props_ptr_vec.at      (n_instance)          = std::move(new_item_ptr);
    }
}

std::unique_ptr<TextRenderer> TextRenderer::create(const EGLInstance* in_egl_instance_ptr,
                                                   Logger*            in_logger_ptr)
{
    std::unique_ptr<TextRenderer> result_ptr;

    result_ptr.reset(
        new TextRenderer(in_egl_instance_ptr,
                         in_logger_ptr)
    );

    if (result_ptr != nullptr)
    {
        if (!result_ptr->init() )
        {
            SCE_DBG_ASSERT(false);

            result_ptr.reset();
        }
    }

    return result_ptr;
}

TextLayerID TextRenderer::create_layer()
{
    TextLayerID result = ++m_n_text_layers_created;

    m_text_layer_map[result] = TextLayer(&m_available_character_props_ptr_vec);

    return result;
}

void TextRenderer::delete_layer(const TextLayerID& in_layer_id)
{
    auto iterator = m_text_layer_map.find(in_layer_id);

    if (iterator != m_text_layer_map.end() )
    {
        m_text_layer_map.erase(iterator);
    }
}

bool TextRenderer::init()
{
    static const char* font_bitmap_file_name = "app0:/meiryo_36.bmp";
    static const char* font_data_file_name   = "app0:/meiryo_36.dat";

    std::unique_ptr<BMP> font_bmp_ptr = nullptr;
    bool                 result       = false;

    {
        const auto           font_data_file_size(IO::get_file_size(font_data_file_name) );
        std::vector<uint8_t> font_data_u8_vec   (font_data_file_size);

        /* Load font bitmap */
        font_bmp_ptr = BMP::create(font_bitmap_file_name,
                                   m_logger_ptr);

        if (font_bmp_ptr == nullptr)
        {
            m_logger_ptr->log(false, /* in_flush_and_wait */
                              "Failed to load font bitmap.\n");

            SCE_DBG_ASSERT(font_bmp_ptr != nullptr);

            goto end;
        }

        /* Read font data */
        if (font_data_file_size == 0)
        {
            m_logger_ptr->log(false, /* in_flush_and_wait */
                              "Font data file is zero-sized.\n");

            SCE_DBG_ASSERT(font_data_file_size != 0);

            goto end;
        }

        if (!IO::read_file(font_data_file_name,
                           font_data_file_size,
                          &font_data_u8_vec[0]) )
        {
            m_logger_ptr->log(false, /* in_flush_and_wait */
                              "Failed to load font data file.\n");

            SCE_DBG_ASSERT(false);

            goto end;
        }

        /* Pre-calc & cache font properties */
        {
            const DATHeader* dat_header_ptr       = reinterpret_cast<const DATHeader*>(font_data_u8_vec.data() );
            const auto       n_characters_per_row = dat_header_ptr->image_width / dat_header_ptr->cell_width;

            m_font_properties_vec.resize(0xFF);

            for (uint8_t n_character = dat_header_ptr->n_start_character;
                         n_character < 0xFF;
                       ++n_character)
            {
                auto&      current_character_props = m_font_properties_vec.at(n_character - dat_header_ptr->n_start_character);
                const auto x1                      = (n_character % n_characters_per_row) * dat_header_ptr->cell_width;
                const auto y1                      = (n_character / n_characters_per_row) * dat_header_ptr->cell_height;

                current_character_props.u1v1[0] = static_cast<float>(x1)                                                 / static_cast<float>(dat_header_ptr->image_width);
                current_character_props.u1v1[1] = static_cast<float>(y1)                                                 / static_cast<float>(dat_header_ptr->image_height);
                current_character_props.u2v2[0] = static_cast<float>(x1 + dat_header_ptr->character_widths[n_character]) / static_cast<float>(dat_header_ptr->image_width);
                current_character_props.u2v2[1] = static_cast<float>(y1 + dat_header_ptr->cell_height)                   / static_cast<float>(dat_header_ptr->image_height);
            }
        }

        /* Create a GL texture for the font bitmap */
        {
            const DATHeader* dat_header_ptr = reinterpret_cast<const DATHeader*>(font_data_u8_vec.data() );

            auto texture_create_info_ptr = TextureCreateInfo::create(TextureType::_2D,
                                                                     TextureFormat::BGRA8,
                                                                     1, /* in_n_mips */
                                                                     font_bmp_ptr->get_extents_wh()[0],
                                                                     font_bmp_ptr->get_extents_wh()[1],
                                                                     TextureMagFilter::LINEAR,
                                                                     TextureMinFilter::LINEAR,
                                                                     TextureWrap::CLAMP_TO_EDGE,
                                                                     TextureWrap::CLAMP_TO_EDGE);
            SCE_DBG_ASSERT(texture_create_info_ptr != nullptr);

            m_font_texture_ptr = Texture::create(m_egl_instance_ptr,
                                                 std::move(texture_create_info_ptr) );

            if (m_font_texture_ptr == nullptr)
            {
                m_logger_ptr->log(false, /* in_flush_and_wait */
                                  "Failed to create font texture.\n");

                SCE_DBG_ASSERT(m_font_texture_ptr != nullptr);

                goto end;
            }

            /* Fill the texture with bitmap data */
            ::glPixelStorei  (GL_UNPACK_ALIGNMENT,
                              1);
            ::glBindTexture  (GL_TEXTURE_2D,
                              m_font_texture_ptr->get_texture_id() );
            ::glTexSubImage2D(GL_TEXTURE_2D,
                              0, /* level   */
                              0, /* xoffset */
                              0, /* yoffset */
                              font_bmp_ptr->get_extents_wh()[0],
                              font_bmp_ptr->get_extents_wh()[1],
                              GL_RGB,            /* format - this should actually be BGR *but* we don't care, it's a font texture */
                              GL_UNSIGNED_BYTE,
                              font_bmp_ptr->get_bgr888_data() );
        }

        /* Init shaders */
        {
            std::unique_ptr<Shader> fs_ptr;
            std::unique_ptr<Shader> vs_ptr;
            {
                auto fs_create_info_ptr = ShaderCreateInfo::create_from_cg("Text Renderer FS",
                                                                           ShaderType::FRAGMENT,
                                                                           g_fs_cg);

                fs_ptr = Shader::create(std::move(fs_create_info_ptr),
                                        m_logger_ptr);
            }

            {
                auto vs_create_info_ptr = ShaderCreateInfo::create_from_cg("Text Renderer VS",
                                                                           ShaderType::VERTEX,
                                                                           g_vs_cg);

                vs_ptr = Shader::create(std::move(vs_create_info_ptr),
                                        m_logger_ptr);
            }

            {
                auto program_create_info_ptr = ProgramCreateInfo::create("Text Renderer program",
                                                                         vs_ptr.get(),
                                                                         fs_ptr.get() );

                m_program_ptr = Program::create(std::move(program_create_info_ptr),
                                                m_logger_ptr);

                SCE_DBG_ASSERT(m_program_ptr != nullptr);
            }
        }
    }

    /* NOTE: A flush should not be too costly, given the renderer is initialized at pre-init time. */
    result = (::glGetError() == GL_NO_ERROR);

    /* Finally, prealloc character props struct instances for use at runtime */
    bake_text_character_props_batch();

end:
    if (result)
    {
        m_logger_ptr->log(false, /* in_flush_and_wait */
                          "Font renderer initialized successfully.\n");
    }
    else
    {
        m_logger_ptr->log(true, /* in_flush_and_wait */
                          "Font renderer failed to initialize.\n");
    }

    return result;
}

void TextRenderer::insert_text(const TextLayerID& in_layer_id,
                               const float*       in_x1y1_normalized_ptr,
                               const char*        in_text_string_ptr)
{
    auto&      layer_props  = m_text_layer_map.at(in_layer_id);
    const auto n_characters = strlen(in_text_string_ptr);

    for (uint32_t n_character = 0;
                  n_character < n_characters;
                ++n_character)
    {
        const auto          character      = in_text_string_ptr[n_character];
        TextCharacterProps* char_props_ptr = nullptr;
        const auto&         font_props     = m_font_properties_vec.at(character);

        if (m_available_character_props_ptr_vec.size() == 0)
        {
            bake_text_character_props_batch();
        }

        char_props_ptr = m_available_character_props_ptr_vec.back();
        SCE_DBG_ASSERT(char_props_ptr != nullptr);

        m_available_character_props_ptr_vec.pop_back();

        char_props_ptr->src_u1v1[0] = font_props.u1v1       [0];
        char_props_ptr->src_u1v1[1] = font_props.u1v1       [1];
        char_props_ptr->src_u2v2[0] = font_props.u2v2       [0];
        char_props_ptr->src_u2v2[1] = font_props.u2v2       [1];
        char_props_ptr->dst_u1v1[0] = in_x1y1_normalized_ptr[0];
        char_props_ptr->dst_u1v1[1] = in_x1y1_normalized_ptr[1];

        layer_props.n_char_to_text_character_props_vec[character].push_back(char_props_ptr);
    }
}

void TextRenderer::reset_layer(const TextLayerID& in_layer_id)
{
    auto iterator = m_text_layer_map.find(in_layer_id);

    if (iterator != m_text_layer_map.end() )
    {
        iterator->second.reset();
    }
}