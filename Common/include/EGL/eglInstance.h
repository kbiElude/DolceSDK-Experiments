#ifndef EGL_INSTANCE_H
#define EGL_INSTANCE_H

extern "C"
{
    /* This hack is needed for VS Code's Intellisense to correctly parse EGL Platform header.. */
    #if defined(__vita__)
        #undef __unix__
    #endif

    #include <EGL/egl.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
}

#include <memory>
#include <vector>

class Logger;

struct EXTDrawInstancedEntrypoints
{
    PFNGLDRAWARRAYSINSTANCEDEXTPROC   glDrawArraysInstancedEXT;
    PFNGLDRAWELEMENTSINSTANCEDEXTPROC glDrawElementsInstancedEXT;

    EXTDrawInstancedEntrypoints()
    {
        glDrawArraysInstancedEXT   = nullptr;
        glDrawElementsInstancedEXT = nullptr;
    }
};

struct EXTInstancedArraysEntrypoints
{
    PFNGLDRAWARRAYSINSTANCEDEXTPROC   glDrawArraysInstancedEXT;
    PFNGLDRAWELEMENTSINSTANCEDEXTPROC glDrawElementsInstancedEXT;
    PFNGLVERTEXATTRIBDIVISOREXTPROC   glVertexAttribDivisorEXT;

    EXTInstancedArraysEntrypoints()
    {
        glDrawArraysInstancedEXT   = nullptr;
        glDrawElementsInstancedEXT = nullptr;
        glVertexAttribDivisorEXT   = nullptr;
    }
};

struct EXTTextureStorageEntrypoints
{
    PFNGLTEXSTORAGE2DEXTPROC glTexStorage2DEXT;

    EXTTextureStorageEntrypoints()
    {
        glTexStorage2DEXT = nullptr;
    }
};

struct SCEPigletShaderBinaryEntrypoints
{
    PFNGLPIGLETGETPROGRAMBINARYSCEPROC glPigletGetShaderBinarySCE;

    SCEPigletShaderBinaryEntrypoints()
    {
        glPigletGetShaderBinarySCE = nullptr;
    }
};

struct SCETextureResourceEntrypoints
{
    PFNGLMAPTEXTURERESOURCESCEPROC   glMapTextureResourceSCE;
    PFNGLTEXIMAGERESOURCESCEPROC     glTexImageResourceSCE;
    PFNGLUNMAPTEXTURERESOURCESCEPROC glUnmapTextureResourceSCE;

    SCETextureResourceEntrypoints()
    {
        glMapTextureResourceSCE   = nullptr;
        glTexImageResourceSCE     = nullptr;
        glUnmapTextureResourceSCE = nullptr;
    }
};

class EGLInstance
{
public:
    ~EGLInstance();

    static std::unique_ptr<EGLInstance> create(Logger*     in_logger_ptr,
                                               const bool& in_require_depth_buffer,
                                               const bool& in_require_stencil_buffer);

    bool bind_to_current_thread();
    void swap_buffers          ();

    const EXTDrawInstancedEntrypoints*      get_ext_draw_instanced_entrypoints_ptr      () const;
    const EXTInstancedArraysEntrypoints*    get_ext_instanced_arrays_entrypoints_ptr    () const;
    const EXTTextureStorageEntrypoints*     get_ext_texture_storage_entrypoints_ptr     () const;
    const SCEPigletShaderBinaryEntrypoints* get_sce_piglet_shader_binary_entrypoints_ptr() const;
    const SCETextureResourceEntrypoints*    get_sce_texture_resource_entrypoints_ptr    () const;

private:
    /* Private type definitions */
    struct EGLConfigProps
    {
        EGLConfig egl_config;
        EGLint    egl_config_id;
        EGLint    n_alpha_bits;
        EGLint    n_blue_bits;
        EGLint    n_depth_bits;
        EGLint    n_green_bits;
        EGLint    n_red_bits;
        EGLint    n_stencil_bits;

        EGLConfigProps()
        {
            egl_config         = nullptr;
            egl_config_id      = 0;
            n_alpha_bits       = 0;
            n_blue_bits        = 0;
            n_depth_bits       = 0;
            n_green_bits       = 0;
            n_red_bits         = 0;
            n_stencil_bits     = 0;
        }
    };

    struct ExtensionEntrypoint
    {
        const char* func_name_ptr;
        void**      func_ptr_ptr;

        ExtensionEntrypoint()
        {
            func_name_ptr = nullptr;
            func_ptr_ptr  = nullptr;
        }

        ExtensionEntrypoint(const char* in_func_name_ptr,
                            void**      in_func_ptr_ptr)
        {
            func_name_ptr = in_func_name_ptr;
            func_ptr_ptr  = in_func_ptr_ptr;
        }
    };

    /* Private functions */
    EGLInstance(Logger* in_logger_ptr);

    bool init                      (const bool&                in_require_depth_buffer,
                                    const bool&                in_require_stencil_buffer);
    bool init_extension_entrypoints(const ExtensionEntrypoint* in_ext_entrypoint_ptr,
                                    const uint32_t&            in_n_ext_entrypoints);

    /* Private variables */
    EGLDisplay                  m_display;
    const EGLConfigProps*       m_egl_config_ptr;
    std::vector<EGLConfigProps> m_egl_config_vec;
    EGLContext                  m_egl_context;
    EGLSurface                  m_egl_surface;
    Logger*                     m_logger_ptr;

    EXTDrawInstancedEntrypoints      m_entrypoints_gl_ext_draw_instanced;
    EXTInstancedArraysEntrypoints    m_entrypoints_gl_ext_instanced_arrays;
    EXTTextureStorageEntrypoints     m_entrypoints_gl_ext_texture_storage;
    SCEPigletShaderBinaryEntrypoints m_entrypoints_gl_sce_piglet_shader_binary;
    SCETextureResourceEntrypoints    m_entrypoints_gl_sce_texture_resource_entrypoints;

    const char* m_gl_extensions_ptr;
    bool        m_never_bound;
};

#endif /* EGL_INSTANCE_H */