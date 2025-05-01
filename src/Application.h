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
    GtkEntry *entry_handle;
    Calculator *calc;
} mainApp;


G_MODULE_EXPORT void app_load_ui_from_file(mainApp *_mApp, const char* ui_file_name);
G_MODULE_EXPORT void app_load_theme_from_file(const char *theme_name,gboolean force_dark);
G_MODULE_EXPORT void app_activate_gtk(GtkApplication *_app, gpointer user_data);
G_MODULE_EXPORT int app_main_run(int argc, char **argv);


#endif