extern "C"
{
    #include <GLES2/gl2.h>
    #include <psp2/kernel/modulemgr.h>
    #include <psp2/kernel/processmgr.h>
    #include <psp2/ctrl.h>
    #include <psp2/libdbg.h>
    #include <psp2/shacccg.h>
}

#include "app.h"
#include "EGL/eglInstance.h"
#include "ES/program.h"
#include "ES/program_create_info.h"
#include "ES/shader.h"
#include "ES/shader_create_info.h"
#include "OS/thread.h"

FullScreenPassApp::FullScreenPassApp()
    :m_must_die(false)
{
    /* Stub */
}

FullScreenPassApp::~FullScreenPassApp()
{
    /* Stub */  
}

int FullScreenPassApp::rendering_thread_entrypoint(void* app_raw_ptr)
{
    FullScreenPassApp* app_ptr           = reinterpret_cast<FullScreenPassApp*>(app_raw_ptr);
    uint32_t           n_frames_rendered = 0;

    app_ptr->m_egl_instance_ptr->bind_to_current_thread();

    /* Set up program */
    std::unique_ptr<Program> program_ptr;

    {
        const char* fs_glsl = "float4 main(        float2 in_uv : TEXCOORD0,\n"
                              "            uniform float  t)\n"
                              "{\n"
                              "    float t_adjusted = sin(t * 3.1415);\n"
                              "\n"
                              "    return float4(t_adjusted * in_uv.x, (1.0 - t_adjusted) * in_uv.y, (in_uv.x + in_uv.y) * 0.5, 1.0);\n"
                              "}\n";

        const char* vs_glsl = "void main(int           in_n_vertex,\n"
                              "          float4 out    out_position : POSITION,\n"
                              "          float2 out    out_uv       : TEXCOORD0)\n"
                              "{\n"
                              "    float x = -1.0 + float( (in_n_vertex & 1) << 2);\n"
                              "    float y = -1.0 + float( (in_n_vertex & 2) << 1);\n"
                              "\n"
                              "    out_uv.x = (x + 1.0) * 0.5;\n"
                              "    out_uv.y = (y + 1.0) * 0.5;\n"
                              "\n"
                              "    out_position = float4(x, y, 0.0, 1.0);\n"
                              "}\n";

        std::unique_ptr<Shader> fs_ptr;
        std::unique_ptr<Shader> vs_ptr;

        {
            auto fs_create_info_ptr = ShaderCreateInfo::create_from_cg("FS",
                                                                       ShaderType::FRAGMENT,
                                                                       fs_glsl);
            SCE_DBG_ASSERT(fs_create_info_ptr != nullptr);

            fs_ptr = Shader::create(std::move(fs_create_info_ptr),
                                    app_ptr->get_logger_ptr() );
            SCE_DBG_ASSERT(fs_ptr != nullptr);
        }

        {
            auto vs_create_info_ptr = ShaderCreateInfo::create_from_cg("VS",
                                                                       ShaderType::VERTEX,
                                                                       vs_glsl);
            SCE_DBG_ASSERT(vs_create_info_ptr != nullptr);

            vs_ptr = Shader::create(std::move(vs_create_info_ptr),
                                    app_ptr->get_logger_ptr() );
            SCE_DBG_ASSERT(vs_ptr != nullptr);
        }

        {
            auto program_create_info_ptr = ProgramCreateInfo::create("Program",
                                                                     vs_ptr.get(),
                                                                     fs_ptr.get() );

            program_ptr = Program::create(std::move(program_create_info_ptr),
                                          app_ptr->get_logger_ptr() );
            SCE_DBG_ASSERT(program_ptr != nullptr);
        }
    }

    static const uint8_t vertex_id_array[] =
    {
        0,
        1,
        2
    };

    ::glVertexAttribPointer    (0,               /* index      */
                                1,               /* size       */
                                GL_UNSIGNED_BYTE,
                                GL_FALSE,        /* normalized */
                                0,               /* stride     */
                                vertex_id_array);
    ::glEnableVertexAttribArray(0);

    while (!app_ptr->m_must_die)
    {
        const float t = static_cast<float>(n_frames_rendered % (90 * 5) ) / (90 * 5 - 1);

        ::glUseProgram(program_ptr->get_program_id() );
        ::glUniform1f (0, /* location */
                       t);
        ::glDrawArrays(GL_TRIANGLES,
                       0, /* first */
                       3); /* count */

        app_ptr->m_egl_instance_ptr->swap_buffers();

        n_frames_rendered++;
    }

    return 0;
}

void FullScreenPassApp::run()
{
    /* Set up an ES context */
    m_egl_instance_ptr = EGLInstance::create(get_logger_ptr(),
                                             false,  /* in_require_depth_buffer   */
                                             false); /* in_require_stencil_buffer */

    SCE_DBG_ASSERT(m_egl_instance_ptr != nullptr);

    /* Spawn a separate renderer thread */
    auto renderer_thread_ptr = Thread::create_and_start("Renderer thread",
                                                       &FullScreenPassApp::rendering_thread_entrypoint,
                                                        this,
                                                        nullptr, /* in_opt_stack_size_ptr */
                                                        0xFF);

    /* Monitor control buttons.. */
    ::sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITAL);

    while (!m_must_die)
    {
        SceCtrlData control_data;

        ::sceCtrlPeekBufferPositive(0, /* port */
                                   &control_data,
                                    1 /* count */);

        if (control_data.buttons != 0)
        {
            m_must_die = true;
        }
    }

    /* Wait for the renderer thread to die */
    renderer_thread_ptr->block_until_done(nullptr);
    renderer_thread_ptr.reset            ();

    /* Done */
}