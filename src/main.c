#include "Application.h"

#if defined(_WIN32) && defined(NDEBUG)
// Include windows.h only on Windows Release builds where WinMain is needed
#include <windows.h>
// Windows Release entry point (uses WinMain, console hidden by linker flag)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    //g_application_run handles command line parsing on Windows when passed 0, NULL     
    //g_setenv("GTK_CSD", "0", FALSE);
    //g_setenv("GTK_DEBUG", "interactive", FALSE);

    return app_main_run(0, NULL);
}
#else
// Standard entry point for non-Windows OR Windows Debug builds (uses main, console shown)
int main(int argc, char **argv) {
    //g_setenv("GTK_CSD", "0", FALSE);
    //g_setenv("GTK_DEBUG", "interactive", FALSE);

    return app_main_run(argc, argv);
}
#endif