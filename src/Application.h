#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <gtk/gtk.h>
#include <glib/gstdio.h> // Needed for g_build_filename
#include <string.h>     // Needed for strcat


struct MAIN_APPLICATION
{
    GtkApplication *gtk_handle;


} app;


G_MODULE_EXPORT void app_load_ui_from_file(GtkApplication *_app, const char* ui_file_name);
G_MODULE_EXPORT void app_load_theme_from_file(const char *theme_name,gboolean force_dark);
G_MODULE_EXPORT void app_activate_gtk(GtkApplication *_app, gpointer user_data);
G_MODULE_EXPORT int app_main_run(int argc, char **argv);


#endif