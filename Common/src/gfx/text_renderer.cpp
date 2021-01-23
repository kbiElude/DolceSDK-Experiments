#include "EGL/eglInstance.h"
#include "ES/texture.h"
#include "ES/texture_create_info.h"
#include "gfx/text_renderer.h"
#include "ImageFormats/bmp.h"
#include "io.h"
#include "logger.h"

#include <psp2/libdbg.h>
#include <string.h>
#include <vector>

TextRenderer::DATHeader::DATHeader()
{
    memset(this,
           0,
           sizeof(*this) );
}

TextRenderer::TextRenderer(const EGLInstance* in_egl_instance_ptr,
                           Logger*            in_logger_ptr)
    :m_egl_instance_ptr(in_egl_instance_ptr),
     m_logger_ptr      (in_logger_ptr)
{
    /* Stub */
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
    }

    /* NOTE: A flush should not be too costly, given the renderer is initialized at pre-init time. */
    result = (::glGetError() == GL_NO_ERROR);
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