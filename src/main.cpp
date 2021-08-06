#include <qapplication.h>

#include "window.h"
//#include "viewer.h"



int main(int argc, char** argv)
{
    // Read command lines arguments.
    QApplication application(argc, argv);

    // Window widget
    Window window;
    // Render the window
    window.show();

    // Run main loop.
    return application.exec();

    return 0;
}