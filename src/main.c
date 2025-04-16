#include <gtk/gtk.h>
#include "adwaita.h"

// Include windows.h only on Windows Release builds where WinMain is needed
#if defined(_WIN32) && defined(NDEBUG)
#include <windows.h>
#endif

static void activate_app(GtkApplication* app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    GtkWidget *label = gtk_label_new ("Hello World");


    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    gtk_window_set_child (GTK_WINDOW (window), label);
    gtk_widget_set_visible(window, TRUE); // Use this instead
}


// Core application logic (can be called by either main or WinMain)
static int app_main(int argc, char **argv) {
    AdwApplication* app = NULL;

    app = adw_application_new ("com.example.calculator",
                                             G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect((GtkApplication*)app, "activate", G_CALLBACK(activate_app), NULL);
    // Pass argc and argv to g_application_run
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref((GtkApplication*)app);
    return status;
}



#if defined(_WIN32) && defined(NDEBUG)
// Windows Release entry point (uses WinMain, console hidden by linker flag)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // g_application_run handles command line parsing on Windows when passed 0, NULL
    return app_main(0, NULL);
}
#else
// Standard entry point for non-Windows OR Windows Debug builds (uses main, console shown)
int main(int argc, char **argv) {
    return app_main(argc, argv);
}
#endif