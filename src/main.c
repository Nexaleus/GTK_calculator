#include "Application.h"

// Include windows.h only on Windows Release builds where WinMain is needed
#if defined(_WIN32) && defined(NDEBUG)
#include <windows.h>
#endif


/*
//G_MODULE_EXPORT void (alternative CALLBACK function declaration when compiling for Windows)
static void load_ui_from_file(GtkApplication *app, const char* ui_file_name)
{
    //GError *load_error = NULL;
    g_print("----------------------------------\nAttempting to load UI XML FILE: '%s'\n\n", ui_file_name);
    gchar *relative_path = NULL;
    gchar *absolute_path = NULL;
    
    //"res/<ui_file_name>"
    relative_path = g_build_filename("res", "ui", ui_file_name, NULL);
    if(relative_path)
    {
        g_print("Connected relative path: %s\n",relative_path);

        absolute_path = g_canonicalize_filename(relative_path,NULL);
        g_free(relative_path);
    }
    else
    {
        g_error("Could not get RELATIVE file path used for loading main UI structure.\n");      
    }

    if (absolute_path)
    {
        if (g_file_test(absolute_path, G_FILE_TEST_EXISTS))
        {
            GtkBuilder *builder = gtk_builder_new_from_file(absolute_path);

            GObject *appWindow = gtk_builder_get_object(builder,"app_window");
            gtk_window_set_application(GTK_WINDOW (appWindow),app);         

            gtk_window_present(GTK_WINDOW(appWindow));
            g_object_unref(builder);
        }
        else
        {
            g_error("FILE NOT FOUND AT: %s", absolute_path);
        }
        g_free(absolute_path);
    }
    else
    {
        g_error("Could not get ABSOLUTE file path used for loading main UI structure.\n");     
    }
}

// Function to load a theme from the share/themes directory
static void load_theme_from_file(const char *theme_name,gboolean force_dark) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();

    g_print("Attempting to load theme: '%s'\n\n", theme_name);
    gchar *relative_path = NULL;
    gchar *absolute_path = NULL;
    
    //"share/themes/<theme_name>/gtk-4.0/gtk.css"
    gchar *filename_gtk = NULL;
    filename_gtk = force_dark ? "gtk-dark.css" : "gtk.css";
    g_print("Theme source file name: %s\n", filename_gtk);
    
    relative_path = g_build_filename("share","themes",theme_name,"gtk-4.0",filename_gtk,NULL);
    if(relative_path)
    {
        g_print("Connected relative path: %s\n",relative_path);

        absolute_path = g_canonicalize_filename(relative_path,NULL);
        g_free(relative_path);
    }
    else
    {
        g_warning("Could not get relative file path.\n");
    }


    gchar *currentDir = NULL;
    currentDir = g_get_current_dir();
    if(currentDir)
    {
        g_print("Current Working Directory: %s\n", currentDir);
        g_free(currentDir);
    }
    else
    {
        g_warning("Could not get current working directory.\n");
    }


    if(absolute_path)
    {
        g_print("Absolute file path: %s\n\n", absolute_path);
        g_print("Attempting to load theme from: %s\n", absolute_path); // Debug print

    // Check if the file exists before attempting to load
    if (g_file_test(absolute_path, G_FILE_TEST_IS_REGULAR)) {
        gtk_css_provider_load_from_path(provider, absolute_path);

        // Apply the CSS provider to the default display
        gtk_style_context_add_provider_for_display(display,
                                                   GTK_STYLE_PROVIDER(provider),
                                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        g_print("Theme '%s' loaded successfully.\n", theme_name);
    } else {
        g_warning("Theme CSS file not found or not accessible: %s", absolute_path);
    }

    // The provider is referenced by the display, so we can unref it here 
    g_object_unref(provider); 
    g_free(absolute_path);
    }
    else
    {
        g_warning("Could not get absolute file path.\n");
    }
}


static void activate_app(GtkApplication* app, gpointer user_data) {
    load_ui_from_file(app, "gtk_test.ui");
    load_theme_from_file("Windows11_Round_Dark", TRUE);
}


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
*/

#if defined(_WIN32) && defined(NDEBUG)
// Windows Release entry point (uses WinMain, console hidden by linker flag)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    //g_application_run handles command line parsing on Windows when passed 0, NULL     
    g_setenv("GTK_CSD", "0", FALSE);
    //g_setenv("GTK_DEBUG", "interactive", FALSE);

    return app_main_run(0, NULL);
}
#else
// Standard entry point for non-Windows OR Windows Debug builds (uses main, console shown)
int main(int argc, char **argv) {


    g_setenv("GTK_CSD", "0", FALSE);
    g_setenv("GTK_DEBUG", "interactive", FALSE);

    return app_main_run(argc, argv);
}
#endif