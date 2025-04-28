#include "Application.h"
#include <stdlib.h>

//G_MODULE_EXPORT void (alternative CALLBACK function declaration when compiling for Windows)
G_MODULE_EXPORT void app_load_ui_from_file(GtkApplication *_app, const char *ui_file_name)
{
    // GError *load_error = NULL;
    g_print("----------------------------------\nAttempting to load UI XML FILE: '%s'\n\n", ui_file_name);
    gchar *relative_path = NULL;
    gchar *absolute_path = NULL;

    //"res/<ui_file_name>"
    relative_path = g_build_filename("res", "ui", ui_file_name, NULL);
    if (relative_path)
    {
        g_print("Connected relative path: %s\n", relative_path);

        absolute_path = g_canonicalize_filename(relative_path, NULL);
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

            GObject *appWindow = gtk_builder_get_object(builder, "app_window");
            gtk_window_set_application(GTK_WINDOW(appWindow), _app);

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

G_MODULE_EXPORT void app_load_theme_from_file(const char *theme_name, gboolean force_dark)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();

    g_print("Attempting to load theme: '%s'\n\n", theme_name);
    gchar *relative_path = NULL;
    gchar *absolute_path = NULL;

    //"share/themes/<theme_name>/gtk-4.0/gtk.css"
    gchar *filename_gtk = NULL;
    filename_gtk = force_dark ? "gtk-dark.css" : "gtk.css";
    g_print("Theme source file name: %s\n", filename_gtk);

    relative_path = g_build_filename("share", "themes", theme_name, "gtk-4.0", filename_gtk, NULL);
    if (relative_path)
    {
        g_print("Connected relative path: %s\n", relative_path);

        absolute_path = g_canonicalize_filename(relative_path, NULL);
        g_free(relative_path);
    }
    else
    {
        g_warning("Could not get relative file path.\n");
    }

    gchar *currentDir = NULL;
    currentDir = g_get_current_dir();
    if (currentDir)
    {
        g_print("Current Working Directory: %s\n", currentDir);
        g_free(currentDir);
    }
    else
    {
        g_warning("Could not get current working directory.\n");
    }

    if (absolute_path)
    {
        g_print("Absolute file path: %s\n\n", absolute_path);
        g_print("Attempting to load theme from: %s\n", absolute_path); // Debug print

        // Check if the file exists before attempting to load
        if (g_file_test(absolute_path, G_FILE_TEST_IS_REGULAR))
        {
            gtk_css_provider_load_from_path(provider, absolute_path);

            // Apply the CSS provider to the default display
            gtk_style_context_add_provider_for_display(display,
                                                       GTK_STYLE_PROVIDER(provider),
                                                       GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
            g_print("Theme '%s' loaded successfully.\n", theme_name);
        }
        else
        {
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

G_MODULE_EXPORT void app_activate_gtk(GtkApplication *_app, gpointer user_data)
{
    app_load_ui_from_file(_app, "gtk_test.ui");
    app_load_theme_from_file("Windows11_Round_Dark", TRUE);
}

G_MODULE_EXPORT int app_main_run(int argc, char **argv)
{
    mainApp *mApp = NULL;
    mApp = (mainApp *)malloc(sizeof(mainApp));
    if(!mApp)
    {
        fprintf(stderr, "Error: Failed to allocate memory for mainApp structure...\n");
        return 1; // Indicate failure
    }

    mApp->gtk_handle = NULL;
    mApp->gtk_handle = gtk_application_new("app.xorrcxrcx.calculator", G_APPLICATION_DEFAULT_FLAGS);
    mApp->calc = NULL;
    mApp->calc = (Calculator *)malloc(sizeof(Calculator));
    if(!mApp->calc)
    {
        fprintf(stderr, "Error: Failed to allocate memory for Calculator structure...\n");
        return 1; // Indicate failure
    }

    mApp->calc->cState = CSTATE_RESET;
    g_signal_connect(mApp->gtk_handle, "activate", G_CALLBACK(app_activate_gtk), NULL);

    //MAIN APP LOOP
    int status = g_application_run(G_APPLICATION(mApp->gtk_handle), argc, argv);

    //MEMORY DEALLOCATION
    g_object_unref(mApp->gtk_handle);
    free(mApp->calc);
    free(mApp);
    return status;
}