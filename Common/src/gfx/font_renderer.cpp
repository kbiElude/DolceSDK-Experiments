#include "gfx/font_renderer.h"
#include <psp2/libdbg.h>

FontRenderer::FontRenderer()
{
    /* Stub */
}

std::unique_ptr<FontRenderer> FontRenderer::create()
{
    std::unique_ptr<FontRenderer> result_ptr;

    result_ptr.reset(
        new FontRenderer()
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

bool FontRenderer::init()
{
    bool result = false;

    // ..

    result = true;
//end:
    return result;
}
