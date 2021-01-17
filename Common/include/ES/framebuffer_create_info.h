#ifndef FRAMEBUFFER_CREATE_INFO_H
#define FRAMEBUFFER_CREATE_INFO_H

#include <GLES2/gl2.h>
#include <memory>

enum class AttachmentObjectType
{
    NONE,
    RENDERBUFFER,
    TEXTURE,
};

enum class AttachmentType
{
    COLOR_0,
    DEPTH,
    STENCIL
};

enum class CMTextureFace
{
    NEGATIVE_X,
    NEGATIVE_Y,
    NEGATIVE_Z,
    POSITIVE_X,
    POSITIVE_Y,
    POSITIVE_Z,

    UNKNOWN
};

class Renderbuffer;
class Texture;

class FramebufferCreateInfo
{
public:
    /* Public functions */
    ~FramebufferCreateInfo();

    std::unique_ptr<FramebufferCreateInfo> create();

    void get_bound_attachment_object_type(const AttachmentType& in_attachment_type,
                                          AttachmentObjectType* out_attachment_object_type_ptr) const;
    void get_bound_attachment_props      (const AttachmentType& in_attachment_type,
                                          const Renderbuffer**  out_rb_ptr_ptr)                 const;
    void get_bound_attachment_props      (const AttachmentType& in_attachment_type,
                                          const Texture**       out_texture_ptr_ptr,
                                          uint32_t*             out_n_mip_ptr,
                                          CMTextureFace*        out_cube_map_texture_face_ptr) const;

    void set_attachment(const AttachmentType& in_attachment_type,
                        const Texture*        in_texture_ptr,
                        const uint32_t&       in_n_mip,
                        const CMTextureFace&  in_opt_cube_map_texture_face = CMTextureFace::UNKNOWN);

    void set_attachment(const AttachmentType& in_attachment_type,
                        const Renderbuffer*   in_renderbuffer_ptr);

private:
    /* Private type definitions */
    struct AttachmentProps
    {
        AttachmentObjectType object_type;

        union
        {
            const Renderbuffer* rb_ptr;
            const Texture*      texture_ptr;
        };

        uint32_t      n_mip;
        CMTextureFace texture_face;

        AttachmentProps()
            :n_mip       (UINT32_MAX),
             object_type (AttachmentObjectType::NONE),
             rb_ptr      (nullptr),
             texture_face(CMTextureFace::UNKNOWN)
        {
            /* Stub */
        }
    };

    /* Private functions */
    FramebufferCreateInfo();

    /* Private variables */
    AttachmentProps m_color_attachment0;
    AttachmentProps m_depth_attachment;
    AttachmentProps m_stencil_attachment;
};

#endif /* FRAMEBUFFER_CREATE_INFO_H */