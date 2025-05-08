#ifdef _MSC_VER
#pragma warning(disable : 4133)
#endif

#include "Application.h"
#include <stdlib.h>
#include <stdio.h>

// Include the generated GResource header
#include "main_application_resources.h"

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
    adw_about_window_set_application_icon(ADW_ABOUT_WINDOW(about_window), "gnome-calculator-symbolic");
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

G_MODULE_EXPORT void app_update_font_label_display(mainApp *_mApp)
{
    if (!_mApp || _mApp->global_font_size < 2 || !_mApp->label_fnt_size)
    {
        g_warning("INVALID mainApp/label_fnt_size pointer!\n");
    }
    else
    {
        char display_label_buffer[5];
        snprintf(display_label_buffer, sizeof(display_label_buffer), "%d", _mApp->global_font_size);
        gtk_label_set_text(_mApp->label_fnt_size, display_label_buffer);
    }
}

G_MODULE_EXPORT void app_on_increase_font_clicked(GtkButton *button, gpointer user_data)
{
    mainApp *_mApp = NULL;
    _mApp = (mainApp *)user_data;
    if (_mApp)
    {
        if (_mApp->global_font_size < 24)
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
        if (_mApp->global_font_size > 6)
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

    if (_mApp->pref_window)
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
                    g_message("Current app theme is in Dark Mode, setting switch ON\n");
                }
                else
                {
                    gtk_switch_set_active(_mApp->dark_mode_switch, TRUE);
                    g_message("Current app theme is in Light Mode, setting switch OFF\n");
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

        gtk_window_set_transient_for(GTK_WINDOW(_mApp->pref_window), GTK_WINDOW(_mApp->main_window));
        gtk_window_present(GTK_WINDOW(_mApp->pref_window));
    }
    else
    {
        g_warning("Preferences window not found!\n");
    }
}

G_MODULE_EXPORT void app_activate_gtk(GtkApplication *_app, gpointer user_data)
{
    mainApp *_mApp = (mainApp *)user_data;
    if (!_mApp)
    {
        g_error("INVALID user_data in app_activate_gtk");
        return;
    }

    
    // Load CSS
    /*GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(css_provider, "/app/xorrcxrcx/calculator/style.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider); // The display takes its own reference
    g_message("CSS loaded from resource: /app/xorrcxrcx/calculator/style.css\n");*/


    // Load UI
    GtkBuilder *builder = gtk_builder_new_from_resource("/app/xorrcxrcx/calculator/ui-layout-main.ui");
    if (!builder)
    {
        g_error("Failed to load UI XML from resource: /app/xorrcxrcx/calculator/ui-layout-main.ui");
        return;
    }
    g_message("UI loaded from resource: /app/xorrcxrcx/calculator/ui-layout-main.ui\n");

    _mApp->main_window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    if (!_mApp->main_window) {
        g_error("Failed to get 'main_window' from builder.");
        g_object_unref(builder);
        return;
    }
    gtk_window_set_application(GTK_WINDOW(_mApp->main_window), _app);


    // INITIALIZING LABELS
    _mApp->label_handle = GTK_LABEL(gtk_builder_get_object(builder, "display_label"));
    _mApp->label_preview = GTK_LABEL(gtk_builder_get_object(builder, "preview_display"));

    // PREF WINDOW INIT
    _mApp->pref_window = GTK_WINDOW(gtk_builder_get_object(builder, "preferences_window"));
    
    // CONNECTING CALC BUTTON CALLBACKS
    GObject *button = NULL; // Re-declare or ensure it's in scope if needed
#define CONNECT_BUTTON(id) \
    button = gtk_builder_get_object(builder, id); \
    if (button) g_signal_connect(button, "clicked", G_CALLBACK(calc_on_button_click), _mApp); \
    else g_warning("Button '%s' not found in UI.", id)

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
        g_warning("Switch 'dark_mode_switch' not found in UI.");
    }
    

    _mApp->label_fnt_size = GTK_LABEL(gtk_builder_get_object(builder, "label_fnt_size"));
    if (_mApp->label_fnt_size) {
         app_update_font_label_display(_mApp);
    } else {
        g_warning("Label 'label_fnt_size' not found in UI.");
    }

#define CONNECT_SETTINGS_BUTTON(id, callback) \
    button = gtk_builder_get_object(builder, id); \
    if (button) g_signal_connect(button, "clicked", G_CALLBACK(callback), _mApp); \
    else g_warning("Button '%s' not found in UI.", id)

    CONNECT_SETTINGS_BUTTON("button_fnt_increase", app_on_increase_font_clicked);
    CONNECT_SETTINGS_BUTTON("button_fnt_decrease", app_on_decrease_font_clicked);
#undef CONNECT_SETTINGS_BUTTON

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

G_MODULE_EXPORT int app_main_run(int argc, char **argv)
{
    mainApp *mApp = NULL;
    mApp = (mainApp *)malloc(sizeof(mainApp));
    if (!mApp)
    {
        g_error("Failed to allocate memory for mainApp structure.");
    }
    mApp->res_base_path = NULL;
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
    
    mApp->res_base_path = "/app/xorrcxrcx/calculator/";

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