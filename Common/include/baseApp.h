#ifndef BASE_APP_H
#define BASE_APP_H

extern "C"
{
    #include <psp2/types.h>
}

class BaseApp
{
public:
             BaseApp();
    virtual ~BaseApp();

    int  getResult();

protected:
    virtual void run() = 0;

private:
     /* Private variables */
    SceUID m_sce_piglet_id;
    SceUID m_shacc_cg_id;
 };

#endif /* BASE_APP_H */