#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#include <memory>


class FontRenderer
{
public:
    /* Public functions */
    static std::unique_ptr<FontRenderer> create();

private:
    /* Private functions */
    FontRenderer();

    bool init();

    /* Private variables */
};

#endif /* FONT_RENDERER_H */