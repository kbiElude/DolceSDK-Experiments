#include "app.h"

int main(int /* argc */, char* /* argv */[])
{
    std::unique_ptr<FullScreenPassApp> app_ptr(new FullScreenPassApp() );

    app_ptr->run();

    return app_ptr->getResult();
}