#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <string.h>
#include "libadwaita-1/adwaita.h"
#include "Calculator.h"

typedef struct
{
    AdwApplication *adw_app_handle;
    GResource *res_main_handle;
    GtkLabel *label_handle;
    GtkLabel *label_preview;
    GtkSwitch *dark_mode_switch;
    AdwPreferencesWindow *pref_window;
    AdwApplicationWindow *main_window;
    Calculator *calc;
    int global_font_size;
    gchar *res_base_path;
    gboolean app_settings_init_finished;
} mainApp;

//TODO: Helper function for building resourse file path string

G_MODULE_EXPORT void app_menu_preferences_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data);
G_MODULE_EXPORT void app_menu_help_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data);
G_MODULE_EXPORT void app_menu_about_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data);

G_MODULE_EXPORT void app_on_settings_button_clicked(GtkButton *button, gpointer user_data);
//G_MODULE_EXPORT void app_load_ui_from_file(mainApp *_mApp, const char *ui_file_name);
//G_MODULE_EXPORT void app_load_theme_from_file(const char *theme_name, gboolean force_dark);
G_MODULE_EXPORT void app_activate_gtk(GtkApplication *_app, gpointer user_data);
G_MODULE_EXPORT void app_on_theme_switch_set(GtkSwitch* _switch, gpointer user_data);
G_MODULE_EXPORT int app_main_run(int argc, char **argv);

#endif