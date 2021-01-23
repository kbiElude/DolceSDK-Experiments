#ifndef BMP_H
#define BMP_H

#include <memory>
#include <vector>

class Logger;


class BMP
{
public:
    /* Public functions */
    static std::unique_ptr<BMP> create(const char* in_file_name_ptr,
                                       Logger*     in_logger_ptr);

    const uint32_t* get_extents_wh() const
    {
        return m_extents;
    }

    const uint8_t* get_bgr888_data() const
    {
        return m_data.data();
    }

private:
    /* Private type definitions */
    #pragma pack(push)
    #pragma pack(1)

    struct BMPFileHeader
    {
        uint16_t type;
        uint32_t header_size;
        uint16_t reserved1;
        uint16_t reserved2;
        uint32_t bitmap_data_start_offset;

        uint32_t size;
        int32_t  width;
        int32_t  height;
        uint16_t n_planes;
        uint16_t n_bits;
        uint32_t compression;
        uint32_t size_image;
        int32_t  x_pels_per_meter;
        int32_t  y_pels_per_meter;
        uint32_t clr_used;
        uint32_t clr_important;

        BMPFileHeader();
    };
    #pragma pack(pop)

    /* Private functions */
    BMP(Logger* in_logger_ptr);

    bool init(const char* in_file_name_ptr);

    /* Private variables */
    Logger* m_logger_ptr;

    uint32_t             m_extents[2];
    std::vector<uint8_t> m_data;
};

#endif /* BMP_H */