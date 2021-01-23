#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <memory>

class EGLInstance;
class Logger;
class Texture;

class TextRenderer
{
public:
    /* Public functions */
    static std::unique_ptr<TextRenderer> create(const EGLInstance* in_egl_instance_ptr,
                                                Logger*            in_logger_ptr);

private:
    /* Private type definitions */
    #pragma pack(push)
    #pragma pack(1)

    struct DATHeader
    {
        uint32_t image_width;
        uint32_t image_height;
        uint32_t cell_width;
        uint32_t cell_height;
        uint8_t  n_start_character;
        uint8_t  character_widths[256];

        DATHeader();
    };
    #pragma pack(pop)

    struct FontProperties
    {
        float u1v1[2];
        float u2v2[2];

        FontProperties()
        {
            u1v1[0] = 0.0f;
            u1v1[1] = 0.0f;
            u2v2[0] = 0.0f;
            u2v2[1] = 0.0f;
        }
    };

    /* Private functions */
    TextRenderer(const EGLInstance* in_egl_instance_ptr,
                 Logger*            in_logger_ptr);

    bool init();

    /* Private variables */
    const EGLInstance* const m_egl_instance_ptr;
    Logger*            const m_logger_ptr;

    std::vector<FontProperties> m_font_properties_vec;
    std::unique_ptr<Texture>    m_font_texture_ptr;
};

#endif /* TEXT_RENDERER_H */