#ifdef _MSC_VER
#pragma warning(disable : 4133)
#endif

#include "Application.h"
#include <stdlib.h>
#include <stdio.h>

// Include the generated GResource header
#include "main_application_resources.h"

gchar *resource_base_path_absolute = "/app/xorrcxrcx/calculator/"; // "/app/xorrcxrcx/calculator/";

G_MODULE_EXPORT gchar* app_append_to_str(const gchar *dest_text, const gchar *text_to_append)
{

    gsize dest_len = strlen(dest_text);
    gsize append_len = strlen(text_to_append);
    // Calculate total size needed: current length + append length + 1 (for null terminator)
    gsize total_size = dest_len + append_len + 1;

    // Allocate memory for the new combined string
    char *new_text_buffer = g_malloc(total_size);
    if (!new_text_buffer)
    {
        g_error("Failed to allocate memory in append_to_display");
        // Allocation failed, cannot proceed
    }
    
    // Copy the current text into the new buffer first.
    // Using strcpy here is safe because we allocated exactly enough space initially.
    strcpy(new_text_buffer, dest_text);

    // Append the new text using g_strlcat for safety.
    // g_strlcat ensures null termination and prevents buffer overflows.
    // The third argument is the total size of the destination buffer.
    g_strlcat(new_text_buffer, text_to_append, total_size);

    // Update the label with the newly created string
    // gtk_label_set_text(GTK_LABEL(_label), new_text_buffer);
    // The caller is now responsible for freeing this memory.
    return new_text_buffer;
}

G_MODULE_EXPORT gchar* app_get_res_path(const gchar *res_name)
{
    if (!resource_base_path_absolute)
    {
        g_warning("During app_get_res_path: resource_base_path_absolute string is NULL.\n");
        return NULL;
    }
    else
    {
        if (res_name)
        {
            gchar *full_path = NULL;
            gchar *res_path_format = NULL;
            res_path_format = app_append_to_str(resource_base_path_absolute, "%s");
            if (!res_path_format)
            {
                g_warning("During app_get_res_path: res_path_format string is NULL.\n");
                return NULL;
            }
            else
            {
                full_path = g_strdup_printf(res_path_format, res_name);
                g_free(res_path_format); // Free res_path_format as it's no longer needed
                if (full_path)
                {
                    return full_path;
                }
                else
                {
                    return NULL;
                }
            }
        }
        else
        {
            g_warning("During app_get_res_path: res_path string arg is NULL.\n");
            return NULL;
        }
    }
}

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
    adw_about_window_set_version(ADW_ABOUT_WINDOW(about_window), "1.3.0");
    adw_about_window_set_developer_name(ADW_ABOUT_WINDOW(about_window), "Xor Rcxrcx");
    adw_about_window_set_developers(ADW_ABOUT_WINDOW(about_window), developers);
    adw_about_window_set_copyright(ADW_ABOUT_WINDOW(about_window), "© 2025 Xor Rcxrcx");
    adw_about_window_set_license_type(ADW_ABOUT_WINDOW(about_window), GTK_LICENSE_LGPL_3_0);
    //adw_about_window_set_application_icon(ADW_ABOUT_WINDOW(about_window), "/app/xorrcxrcx/calculator/app-icon.png");
    gtk_window_set_transient_for(GTK_WINDOW(about_window), GTK_WINDOW(_mApp->main_window));
    gtk_window_set_modal(GTK_WINDOW(about_window), TRUE);
    gtk_window_present(GTK_WINDOW(about_window));

    g_message("About window shown.\n");
}

G_MODULE_EXPORT void app_on_theme_switch_set(GtkSwitch *_switch, gpointer user_data)
{
    AdwStyleManager *style_manager = NULL;
    style_manager = adw_style_manager_get_default();
    if (style_manager)
    {
        if (adw_style_manager_get_dark(style_manager))
        {
            adw_style_manager_set_color_scheme(style_manager, ADW_COLOR_SCHEME_FORCE_LIGHT);
            g_message("App theme set to LIGHT MODE\n");
        }
        else
        {
            adw_style_manager_set_color_scheme(style_manager, ADW_COLOR_SCHEME_FORCE_DARK);
            g_message("App theme set to DARK MODE\n");
        }
    }
    else
    {
        g_warning("Default ADW Style manager handle was not found!\n");
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

    if (_mApp->pref_window)
    {
        if (!_mApp->app_settings_init_finished)
        {
            AdwStyleManager *style_manager = NULL;
            style_manager = adw_style_manager_get_default();
            if (style_manager)
            {
                if (_mApp->dark_mode_switch)
                {
                    if (adw_style_manager_get_dark(style_manager))
                    {
                        gtk_switch_set_active(_mApp->dark_mode_switch, FALSE);
                        g_message("Current app theme is in Dark Mode, setting switch OFF\n");
                    }
                    else
                    {
                        gtk_switch_set_active(_mApp->dark_mode_switch, TRUE);
                        g_message("Current app theme is in Light Mode, setting switch ON\n");
                    }
                }
                else
                {
                    g_warning("Dark mode switch is NULL!\n");
                }
            }
            else
            {
                g_warning("Default ADW Style manager handle was not found!\n");
            }

            g_message("App settings successfully initialized.\n");
            _mApp->app_settings_init_finished = TRUE;
        }
        else
        {
            g_message("App settings were ALREADY initialized.\n");
        }

        gtk_window_set_transient_for(GTK_WINDOW(_mApp->pref_window), GTK_WINDOW(_mApp->main_window));
        gtk_window_present(GTK_WINDOW(_mApp->pref_window));
    }
    else
    {
        g_warning("Preferences window not found!\n");
    }
}

G_MODULE_EXPORT void app_load_css_from_resource(gchar *res_name)
{
    if (!res_name)
    {
        g_error("INVALID res_name string. String is NULL in app_load_css_from_resource");
        return;
    }

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(css_provider, res_name);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider); // The display takes its own reference
    g_message("CSS Style loaded from resource: %s\n", res_name);
}

G_MODULE_EXPORT void app_build_and_present_ui(mainApp *_mApp, gchar* res_name)
{
    if(!_mApp)
    {
        g_error("INVALID mainApp pointer in app_build_and_present_ui");
        return;
    }
    if(!res_name)
    {
        g_error("INVALID res_name string. String is NULL in app_build_and_present_ui");
        return;
    }

    GtkBuilder *builder = gtk_builder_new_from_resource(res_name);
    if (!builder)
    {
        g_error("Failed to load UI XML from resource: %s",res_name);
        return;
    }
    g_message("UI loaded from resource: %s\n", res_name);

    _mApp->main_window = NULL;
    _mApp->main_window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    if (!_mApp->main_window) {
        g_error("Failed to get 'main_window' from builder.");
        g_object_unref(builder);
        return;
    }

    gtk_window_set_application(GTK_WINDOW(_mApp->main_window), _mApp->adw_app_handle);

    // INITIALIZING LABELS
    _mApp->label_handle = GTK_LABEL(gtk_builder_get_object(builder, "display_label"));
    _mApp->label_preview = GTK_LABEL(gtk_builder_get_object(builder, "preview_display"));

    // PREF WINDOW INIT
    _mApp->pref_window = GTK_WINDOW(gtk_builder_get_object(builder, "preferences_window"));
    
    // CONNECTING CALC BUTTON CALLBACKS
    GObject *button = NULL;
#define CONNECT_BUTTON(id) \
    button = gtk_builder_get_object(builder, id); \
    if (button) g_signal_connect(button, "clicked", G_CALLBACK(calc_on_button_click), _mApp); \
    else g_error("Button id:'%s' not found in UI.", id)

    CONNECT_BUTTON("button_clear");
    CONNECT_BUTTON("button_backspace");
    CONNECT_BUTTON("button_divide");
    CONNECT_BUTTON("button_multiply");
    CONNECT_BUTTON("button_7");
    CONNECT_BUTTON("button_8");
    CONNECT_BUTTON("button_9");
    CONNECT_BUTTON("button_subtract");
    CONNECT_BUTTON("button_4");
    CONNECT_BUTTON("button_5");
    CONNECT_BUTTON("button_6");
    CONNECT_BUTTON("button_add");
    CONNECT_BUTTON("button_1");
    CONNECT_BUTTON("button_2");
    CONNECT_BUTTON("button_3");
    CONNECT_BUTTON("button_equals");
    CONNECT_BUTTON("button_0");
    CONNECT_BUTTON("button_decimal");
#undef CONNECT_BUTTON
    //------------------------------------------------------------------------------------

    _mApp->dark_mode_switch = GTK_SWITCH(gtk_builder_get_object(builder, "dark_mode_switch"));
    if (_mApp->dark_mode_switch) {
        g_signal_connect(_mApp->dark_mode_switch, "state-set", G_CALLBACK(app_on_theme_switch_set), NULL);
    } else {
        g_error("Switch id:'dark_mode_switch' not found in UI XML file.");
        return;
    }

    // ACTIONS INIT
    const GActionEntry win_actions[] = {
        {"preferences", app_menu_preferences_activated, NULL, NULL, NULL},
        {"help", app_menu_help_activated, NULL, NULL, NULL},
        {"about", app_menu_about_activated, NULL, NULL, NULL}};
    g_action_map_add_action_entries(G_ACTION_MAP(_mApp->main_window), win_actions, G_N_ELEMENTS(win_actions), _mApp);

    //------------
    gtk_window_present(GTK_WINDOW(_mApp->main_window));
    g_object_unref(builder);
}


G_MODULE_EXPORT void app_activate_gtk(GtkApplication *_app, gpointer user_data)
{
    mainApp *_mApp = (mainApp *)user_data;
    if (!_mApp)
    {
        g_error("INVALID user_data in app_activate_gtk");
        return;
    }

    //app_load_css_from_resource(app_get_res_path("style.css"));
    app_build_and_present_ui(_mApp, app_get_res_path("ui-layout-main.ui"));
}

G_MODULE_EXPORT int app_main_run(int argc, char **argv)
{
    mainApp *mApp = NULL;
    mApp = (mainApp *)malloc(sizeof(mainApp));
    if (!mApp)
    {
        g_error("Failed to allocate memory for mainApp structure.");
        return EXIT_FAILURE;
    }

    adw_init(); //use or not?

    mApp->app_settings_init_finished = FALSE;
    mApp->res_main_handle = NULL;
    mApp->res_main_handle = resources_get_resource();
    if(mApp->res_main_handle)
    {
        g_resources_register(mApp->res_main_handle);
    }
    else
    {
         g_error("Failed to get Main Resource handle!");
    }
    
    mApp->adw_app_handle = NULL;
    mApp->adw_app_handle = adw_application_new("app.xorrcxrcx.calculator", G_APPLICATION_DEFAULT_FLAGS);
    mApp->calc = NULL;
    mApp->calc = (Calculator *)malloc(sizeof(Calculator));
    if (!mApp->calc)
    {
        g_error(" Failed to allocate memory for Calculator structure.");
    }
    
    mApp->calc->cState = CSTATE_RESET;
    mApp->calc->just_calculated = FALSE;
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