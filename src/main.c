#include <gtk/gtk.h>
//#include "adwaita.h"

// Include windows.h only on Windows Release builds where WinMain is needed
#if defined(_WIN32) && defined(NDEBUG)
#include <windows.h>
#endif

static void activate_app(GtkApplication* app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    GtkWidget *label = gtk_label_new("THIS IS A LABEL");

    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
    gtk_window_set_child(GTK_WINDOW(window), label);

    gtk_widget_set_visible(window, TRUE); // Use this instead
}


// Core application logic (can be called by either main or WinMain)
static int app_main(int argc, char **argv) {
    //AdwApplication* app = NULL;

    GtkApplication* app = NULL;

    //GtkApplication* app = NULL;
    //GtkCssProvider *provider = NULL;
    //GdkDisplay *display = NULL;

    // Initialize libadwaita (important for AdwApplication)
    //adw_init();

    app = gtk_application_new("app.xorrcxrcx.calculator", G_APPLICATION_DEFAULT_FLAGS);

/* Theme Loading Logic (Currently Broken)
    // Load custom CSS theme AFTER AdwApplication is created
    provider = gtk_css_provider_new();
    // Note: This assumes 'res/custom-theme.css' is accessible relative
    // to the directory where the application is run.
    // For more robust deployment, consider using GResource.
    // This version of the function doesn't report errors via GError.
    char* themePath = "NULL";

#if defined(NDEBUG)
    themePath = "res/custom-theme.css";
#else
    themePath = "../../res/custom-theme.css";
#endif

//themePath = "../../res/custom-themeETWYWJFURU";
    gtk_css_provider_load_from_path(provider, themePath);
    // Apply the CSS provider to the default display
    // We assume loading succeeded as this function variant doesn't report errors.
    display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display,
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    // The provider is referenced by the display, so we can unref it here
    g_object_unref(provider);
*/

    g_signal_connect(app, "activate", G_CALLBACK(activate_app), NULL);

    // Pass argc and argv to g_application_run
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}



#if defined(_WIN32) && defined(NDEBUG)
// Windows Release entry point (uses WinMain, console hidden by linker flag)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // g_application_run handles command line parsing on Windows when passed 0, NULL
    g_setenv("GTK_CSD", "0", FALSE);
    g_setenv("GTK_DEBUG", "interactive", FALSE);

    return app_main(0, NULL);
}
#else
// Standard entry point for non-Windows OR Windows Debug builds (uses main, console shown)
int main(int argc, char **argv) {
    g_setenv("GTK_CSD", "0", FALSE);
    g_setenv("GTK_DEBUG", "interactive", FALSE);

    return app_main(argc, argv);
}
#endif