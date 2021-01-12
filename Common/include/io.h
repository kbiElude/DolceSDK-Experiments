#ifndef IO_H
#define IO_H

#include <stdio.h>

namespace IO
{
    void write_file(const char*     in_file_name_ptr,
                    const void*     in_data_ptr,
                    const uint32_t& in_n_data_bytes);
}

#endif