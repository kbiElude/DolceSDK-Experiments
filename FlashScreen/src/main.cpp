#include "app.h"

int main(int /* argc */, char* /* argv */[])
{
    std::unique_ptr<FlashScreenApp> app_ptr(new FlashScreenApp() );

    app_ptr->run();

    return app_ptr->getResult();
}