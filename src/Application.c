#ifdef _MSC_VER
#pragma warning(disable : 4133)
#endif

#include "Application.h"
#include <stdlib.h>
#include <stdio.h>

G_MODULE_EXPORT void app_menu_preferences_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    mainApp *_mApp = (mainApp *)user_data;
    if (!_mApp)
    {
        g_error("Invalid user_data in preferences action");
    }
    app_on_settings_button_clicked(NULL, _mApp); // Reuse settings logic
}

G_MODULE_EXPORT void app_menu_help_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    g_print("Help action activated.\n");
}

G_MODULE_EXPORT void app_menu_about_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    mainApp *_mApp = (mainApp *)user_data;
    if (!_mApp || !_mApp->main_window)
    {
        g_error("Cannot show About dialog: invalid mainApp or main_window pointer.\n");
    }

    const char *developers[] = {"Xor Rcxrcx", NULL}; // Replace with actual developer name(s)

    AdwAboutWindow *about_window = adw_about_window_new();
    adw_about_window_set_application_name(ADW_ABOUT_WINDOW(about_window), "GTK Calculator");
    adw_about_window_set_version(ADW_ABOUT_WINDOW(about_window), "1.2.1");
    adw_about_window_set_developer_name(ADW_ABOUT_WINDOW(about_window), "Xor Rcxrcx");
    adw_about_window_set_developers(ADW_ABOUT_WINDOW(about_window), developers);
    adw_about_window_set_copyright(ADW_ABOUT_WINDOW(about_window), "© 2023 Xor Rcxrcx");
    adw_about_window_set_license_type(ADW_ABOUT_WINDOW(about_window), GTK_LICENSE_LGPL_3_0);
    adw_about_window_set_application_icon(ADW_ABOUT_WINDOW(about_window), "gnome-calculator-symbolic");
    gtk_window_set_transient_for(GTK_WINDOW(about_window), GTK_WINDOW(_mApp->main_window));
    gtk_window_set_modal(GTK_WINDOW(about_window), TRUE);
    gtk_window_present(GTK_WINDOW(about_window));

    g_message("About window shown.\n");
}

G_MODULE_EXPORT void app_load_ui_from_file(mainApp *_mApp, const char *ui_file_name)
{
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

            _mApp->main_window = NULL;
            GObject *appWindow = gtk_builder_get_object(builder, "main_window");
            gtk_window_set_application(GTK_APPLICATION_WINDOW(appWindow), _mApp->adw_app_handle);
            _mApp->main_window = GTK_WINDOW(appWindow);

            //INITIALIZING LABELS
            _mApp->label_handle = NULL;
            GObject *display_label = gtk_builder_get_object(builder,"display_label");
            _mApp->label_handle = GTK_LABEL(display_label);

            _mApp->label_preview = NULL;
            GObject *preview_label = gtk_builder_get_object(builder,"preview_display");
            _mApp->label_preview = GTK_LABEL(preview_label);

            //PREF WINDOW INIT
            _mApp->pref_window = NULL;
            GObject *pref_win_obj = gtk_builder_get_object(builder,"preferences_window");
            _mApp->pref_window = GTK_WINDOW(pref_win_obj);

            //CONNECTING CALC BUTTON CALLBACKS
            GObject *button = NULL;
            button = gtk_builder_get_object(builder,"button_clear");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);

            button = gtk_builder_get_object(builder,"button_backspace");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);

            button = gtk_builder_get_object(builder,"button_divide");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_multiply");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_7");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_8");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_9");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_subtract");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_4");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_5");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_6");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_add");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_1");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_2");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_3");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_equals");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);

            button = gtk_builder_get_object(builder,"button_0");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);
            
            button = gtk_builder_get_object(builder,"button_decimal");
            g_signal_connect(button,"clicked",G_CALLBACK(calc_on_button_click),_mApp);         
            //------------------------------------------------------------------------------------

            /*button = gtk_builder_get_object(builder, "settings_button");
            g_signal_connect(button,"clicked",G_CALLBACK(app_on_settings_button_clicked),_mApp);*/ 

            _mApp->label_fnt_size = NULL;
            GObject *label_fnt = gtk_builder_get_object(builder, "label_fnt_size");
            _mApp->label_fnt_size = GTK_LABEL(label_fnt);
            app_update_font_label_display(_mApp);

            button = gtk_builder_get_object(builder, "button_fnt_increase");
            g_signal_connect(button,"clicked",G_CALLBACK(app_on_increase_font_clicked),_mApp);

            button = gtk_builder_get_object(builder, "button_fnt_decrease");
            g_signal_connect(button,"clicked",G_CALLBACK(app_on_decrease_font_clicked),_mApp);


            //ACTIONS INIT
            const GActionEntry win_actions[] = {
                {"preferences", app_menu_preferences_activated, NULL, NULL, NULL},
                {"help", app_menu_help_activated, NULL, NULL, NULL},
                {"about", app_menu_about_activated, NULL, NULL, NULL}
            };
            // Pass _mApp as user_data, making it available to the action handlers.
            g_action_map_add_action_entries(G_ACTION_MAP(_mApp->main_window), win_actions, G_N_ELEMENTS(win_actions), _mApp);

            //------------
            gtk_window_present(GTK_APPLICATION_WINDOW(appWindow));
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

G_MODULE_EXPORT void app_update_font_label_display(mainApp *_mApp)
{
    if(!_mApp || _mApp->global_font_size < 2 || !_mApp->label_fnt_size)
    {
        g_warning("INVALID mainApp/label_fnt_size pointer!\n");
    }
    else
    {
        char display_label_buffer[5];
        snprintf(display_label_buffer, sizeof(display_label_buffer), "%d",_mApp->global_font_size);
        gtk_label_set_text(_mApp->label_fnt_size,display_label_buffer);
    }
}

G_MODULE_EXPORT void app_on_increase_font_clicked(GtkButton *button, gpointer user_data)
{
    mainApp *_mApp = NULL;
    _mApp = (mainApp *)user_data;
    if (_mApp)
    {
        if(_mApp->global_font_size < 24)
        {       
            _mApp->global_font_size++;
            app_update_font_label_display(_mApp);
        }
    }
    else
    {
        g_warning("INVALID mainApp pointer!\n");
    }
}

G_MODULE_EXPORT void app_on_decrease_font_clicked(GtkButton *button, gpointer user_data)
{
    mainApp *_mApp = NULL;
    _mApp = (mainApp *)user_data;
    if (_mApp)
    {
        if(_mApp->global_font_size > 6)
        {
            _mApp->global_font_size--;
            app_update_font_label_display(_mApp);
        }
    }
    else
    {
        g_warning("INVALID mainApp pointer!\n");
    }
}

G_MODULE_EXPORT void app_on_settings_button_clicked(GtkButton *button, gpointer user_data)
{
    mainApp *_mApp = NULL;
    _mApp = (mainApp *)user_data;
    if (!_mApp)
    {
        g_error("INVALID TYPE CONVERSION TO MAINAPP POINTER");
    }

    if(_mApp->pref_window)
    {
        gtk_window_set_transient_for(GTK_WINDOW(_mApp->pref_window), GTK_WINDOW(_mApp->main_window));
        gtk_window_present(GTK_WINDOW(_mApp->pref_window));
    }
    else
    {
        g_warning("Preferences window not found!\n");
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
            g_warning("Theme CSS file not found or not accessible: %s\n", absolute_path);
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
    mainApp *_mApp = NULL;
    _mApp = (mainApp *)user_data;
    if (!_mApp)
    {
        g_error("INVALID TYPE CONVERSION TO MAINAPP POINTER");
    }

    app_load_ui_from_file(_mApp, "gtk_new_UI_layout_PREFERENCES.ui");
}

G_MODULE_EXPORT int app_main_run(int argc, char **argv)
{
    mainApp *mApp = NULL;
    mApp = (mainApp *)malloc(sizeof(mainApp));
    if (!mApp)
    {
        /*fprintf(stderr, "Error: Failed to allocate memory for mainApp structure...\n");
        return 1;*/
        g_error("Failed to allocate memory for mainApp structure.");
    }

    //adw_init();
    mApp->adw_app_handle = NULL;
    mApp->adw_app_handle = adw_application_new("app.xorrcxrcx.calculator", G_APPLICATION_DEFAULT_FLAGS);
    mApp->calc = NULL;
    mApp->calc = (Calculator *)malloc(sizeof(Calculator));
    if (!mApp->calc)
    {
        /*fprintf(stderr, "Error: Failed to allocate memory for Calculator structure...\n");
        return 1; // Indicate failure*/
        g_error(" Failed to allocate memory for Calculator structure.");
    }

    mApp->calc->cState = CSTATE_RESET;
    mApp->global_font_size = 11;
    g_signal_connect(mApp->adw_app_handle, "activate", G_CALLBACK(app_activate_gtk), mApp);

    // MAIN APP LOOP
    int status = g_application_run(G_APPLICATION(mApp->adw_app_handle), argc, argv);

    // MEMORY DEALLOCATION
    g_object_unref(mApp->adw_app_handle);
    free(mApp->calc);
    free(mApp);
    return status;
}