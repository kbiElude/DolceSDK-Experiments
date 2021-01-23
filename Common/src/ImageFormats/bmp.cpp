#include "ImageFormats/bmp.h"
#include "io.h"
#include "logger.h"
#include "utils.h"
#include <string.h>

#include <psp2/libdbg.h>

BMP::BMPFileHeader::BMPFileHeader()
{
    memset(this,
           sizeof(*this),
           0);
}

BMP::BMP(Logger* in_logger_ptr)
    :m_logger_ptr(in_logger_ptr)
{
    /* Stub */
}

std::unique_ptr<BMP> BMP::create(const char* in_file_name_ptr,
                                 Logger*     in_logger_ptr)
{
    std::unique_ptr<BMP> result_ptr;

    result_ptr.reset(
        new BMP(in_logger_ptr)
    );

    if (result_ptr != nullptr)
    {
        if (!result_ptr->init(in_file_name_ptr) )
        {
            SCE_DBG_ASSERT(false);

            result_ptr.reset();
        }
    }

    return result_ptr;
}

bool BMP::init(const char* in_file_name_ptr)
{
    uint32_t file_size = 0;
    bool     result    = false;

    file_size = IO::get_file_size(in_file_name_ptr);

    if (file_size == 0)
    {
        SCE_DBG_ASSERT(file_size != 0);
        goto end;
    }

    {
        std::vector<uint8_t> file_data (file_size);
        const BMPFileHeader* header_ptr(nullptr);

        if (IO::read_file(in_file_name_ptr,
                          file_size,
                         &file_data[0]) == false)
        {
            SCE_DBG_ASSERT(false);

            goto end;
        }

        header_ptr = reinterpret_cast<const BMPFileHeader*>(file_data.data() );

        /* 1. Verify endianness */
        if (header_ptr->type != 16973 /* BM */)
        {
            SCE_DBG_ASSERT(false);

            goto end;
        }

        /* 2. Some sanity checks.. */
        if (header_ptr->compression != 0)
        {
            SCE_DBG_ASSERT(header_ptr->compression == 0);

            goto end;
        }

        if (header_ptr->n_bits != 24)
        {
            SCE_DBG_ASSERT(header_ptr->n_bits == 24);

            goto end;
        }

        /* 3. Cache bitmap data */
        m_extents[0] = header_ptr->width;
        m_extents[1] = header_ptr->height;

        m_data.resize(m_extents[0] * m_extents[1] * 3 /* rgb */);

        memcpy(&m_data[0],
               &file_data.at(header_ptr->bitmap_data_start_offset),
               m_data.size() );
    }

    result = true;
end:
    return result;
}
