#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <memory>
#include <unordered_map>


class EGLInstance;
class Logger;
class Texture;

typedef uint32_t TextLayerID;

class TextRenderer
{
public:
    /* Public functions */
    static std::unique_ptr<TextRenderer> create(const EGLInstance* in_egl_instance_ptr,
                                                Logger*            in_logger_ptr);

    TextLayerID create_layer();
    void        delete_layer(const TextLayerID& in_layer_id);
    bool        render_layer(const TextLayerID& in_layer_id);
    void        reset_layer (const TextLayerID& in_layer_id);

    void insert_text(const TextLayerID& in_layer_id,
                     const float*       in_x1y1_normalized_ptr,
                     const char*        in_text_string_ptr);

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

    struct TextCharacterProps
    {
        float dst_u1v1[2];
        float src_u1v1[2];
        float src_u2v2[2];

        TextCharacterProps();
    };

    struct TextLayer
    {
        std::vector<TextCharacterProps*>*                              available_character_props_ptr_vec_ptr;
        std::unordered_map<uint8_t, std::vector<TextCharacterProps*> > n_char_to_text_character_props_vec;

        TextLayer()
            :available_character_props_ptr_vec_ptr(nullptr)
        {
            /* Stub */
        }

        TextLayer(std::vector<TextCharacterProps*>* in_available_character_props_ptr_vec_ptr)
            :available_character_props_ptr_vec_ptr(in_available_character_props_ptr_vec_ptr)
        {
            /* Stub */
        }

        void reset();
    };

    /* Private functions */
    TextRenderer(const EGLInstance* in_egl_instance_ptr,
                 Logger*            in_logger_ptr);

    void bake_text_character_props_batch();
    bool init                           ();

    /* Private variables */
    const EGLInstance* const m_egl_instance_ptr;
    Logger*            const m_logger_ptr;

    std::vector<FontProperties> m_font_properties_vec;
    std::unique_ptr<Texture>    m_font_texture_ptr;

    std::vector<TextCharacterProps*>                  m_available_character_props_ptr_vec;
    uint32_t                                          m_n_text_layers_created;
    std::vector<std::unique_ptr<TextCharacterProps> > m_prealloced_character_props_ptr_vec;
    std::unordered_map<TextLayerID, TextLayer>        m_text_layer_map;
};

#endif /* TEXT_RENDERER_H */