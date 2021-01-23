extern "C"
{
    #include <psp2/kernel/iofilemgr.h>
    #include <psp2/libdbg.h>
}

#include <string.h>

#include "io.h"

uint32_t IO::get_file_size(const char* in_file_name_ptr)
{
    SceIoStat io_stat;
    uint32_t  result  = 0;

    if (::sceIoGetstat(in_file_name_ptr,
                      &io_stat) < 0)
    {
        SCE_DBG_ASSERT(false);

        goto end;
    }

    result = io_stat.st_size;
end:
    return result;
}

bool IO::read_file(const char*     in_file_name_ptr,
                   const uint32_t& in_n_data_bytes,
                   void*           out_data_ptr)
{
    const auto file_id     = ::sceIoOpen(in_file_name_ptr,
                                         SCE_O_RDONLY,
                                         0777); /* mode */
    int        result_call = SCE_OK;
    bool       result      = false;

    if (file_id < 0)
    {
        SCE_DBG_ASSERT(file_id >= 0);

        goto end;
    }

    result_call = ::sceIoRead(file_id,
                              out_data_ptr,
                              in_n_data_bytes);

    result = (result_call == in_n_data_bytes);
end:
    return result;
}

void IO::write_file(const char*     in_file_name_ptr,
                    const void*     in_data_ptr,
                    const uint32_t& in_n_data_bytes)
{
    const auto file_id    = ::sceIoOpen(in_file_name_ptr,
                                        SCE_O_CREAT | SCE_O_TRUNC | SCE_O_WRONLY,
                                        0777); /* mode */
    int        result     = SCE_OK;

    if (file_id < 0)
    {
        SCE_DBG_ASSERT(file_id >= 0);

        goto end;
    }

    result = ::sceIoWrite(file_id,
                          in_data_ptr,
                          in_n_data_bytes);

    if (result != in_n_data_bytes)
    {
        SCE_DBG_ASSERT(result == in_n_data_bytes);

        goto end;
    }

end:
    if (file_id >= 0)
    {
        ::sceIoClose(file_id);
    }
}