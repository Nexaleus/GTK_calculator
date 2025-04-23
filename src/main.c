#include <gtk/gtk.h>
#include <glib/gstdio.h> // Needed for g_build_filename
#include <string.h>     // Needed for strcat

// Include windows.h only on Windows Release builds where WinMain is needed
#if defined(_WIN32) && defined(NDEBUG)
#include <windows.h>
#endif

// Function to load a theme from the share/themes directory
static void load_theme_from_share(const char *theme_name) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();

    char *theme_base_path = "share/themes/"; // Relative path to themes directory
    char *css_file = "/gtk-4.0/gtk.css";
    char *full_path = NULL;

    // Construct the full path: share/themes/<theme_name>/gtk-4.0/gtk.css
    // Note: g_build_filename handles path separators correctly across platforms.
    full_path = g_build_filename(theme_base_path, theme_name, css_file, NULL);

    g_print("Attempting to load theme from: %s\n", full_path); // Debug print

    // Check if the file exists before attempting to load
    if (g_file_test(full_path, G_FILE_TEST_IS_REGULAR)) {
        gtk_css_provider_load_from_path(provider, full_path);

        // Apply the CSS provider to the default display
        gtk_style_context_add_provider_for_display(display,
                                                   GTK_STYLE_PROVIDER(provider),
                                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        g_print("Theme '%s' loaded successfully.\n", theme_name);
    } else {
        g_warning("Theme CSS file not found or not accessible: %s", full_path);
    }
    // Free the constructed path
    g_free(full_path);

    // The provider is referenced by the display, so we can unref it here 
    g_object_unref(provider);    
}


static void activate_app(GtkApplication* app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
 
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
    load_theme_from_share("Windows10");

    gtk_widget_set_visible(window, TRUE); // Use this instead
}



/*void btn_hide_icon(GtkButton* btn,gpointer user_data)
{

}*/


// Core application logic (can be called by either main or WinMain)
static int app_main(int argc, char **argv) {
    GtkApplication* app = NULL;

    app = gtk_application_new("app.xorrcxrcx.calculator", G_APPLICATION_DEFAULT_FLAGS);

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
    //g_setenv("GTK_CSD", "0", FALSE);
    g_setenv("GTK_DEBUG", "interactive", FALSE);

    return app_main(0, NULL);
}
#else
// Standard entry point for non-Windows OR Windows Debug builds (uses main, console shown)
int main(int argc, char **argv) {
    //g_setenv("GTK_CSD", "0", FALSE);
    g_setenv("GTK_DEBUG", "interactive", FALSE);

    return app_main(argc, argv);
}
#endif