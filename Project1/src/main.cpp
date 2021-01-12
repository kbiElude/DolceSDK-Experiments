#include "app.h"

int main(int /* argc */, char* /* argv */[])
{
    std::unique_ptr<App> app_ptr(new App() );

    app_ptr->run();

    return app_ptr->getResult();
}