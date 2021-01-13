#ifndef BASE_APP_H
#define BASE_APP_H

extern "C"
{
    #include <psp2/types.h>
}

#include <memory>


class Logger;

class BaseApp
{
public:
             BaseApp();
    virtual ~BaseApp();

    Logger* get_logger_ptr()
    {
        return m_logger_ptr.get();
    }

    int getResult();

    virtual void run() = 0;

private:
     /* Private variables */
    SceUID m_sce_piglet_id;
    SceUID m_shacc_cg_id;

    std::unique_ptr<Logger> m_logger_ptr;
 };

#endif /* BASE_APP_H */