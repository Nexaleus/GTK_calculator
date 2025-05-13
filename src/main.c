#include "Application.h"

/*
// Custom log handler to show a dialog for fatal errors
static void
show_error_dialog_handler(const gchar *log_domain,
                          GLogLevelFlags log_level,
                          const gchar *message,
                          gpointer user_data)
{
  // Only handle fatal application errors that would terminate the app
  if ((log_level & G_LOG_LEVEL_ERROR) && (log_level & G_LOG_FLAG_FATAL))
  {
    if (gtk_is_initialized()) // Check if GTK is ready
    {
      GtkWidget *dialog = gtk_message_dialog_new(NULL,             // No transient parent
                                                 GTK_DIALOG_MODAL, // Make it modal
                                                 GTK_MESSAGE_ERROR,
                                                 GTK_BUTTONS_CLOSE,
                                                 "Application Error");
      gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
                                               "A critical error occurred and the application must close.\n\nDetails:\n%s",
                                               message);
      gtk_window_set_title(GTK_WINDOW(dialog), "Error");

      gtk_widget_show(dialog); // Show the dialog

      // Process some events to allow the dialog to appear before abort()
      // This is a heuristic. A small number of iterations should be enough.
      // Ensure default context exists and process pending events.
      GMainContext *context = g_main_context_default();
      if (context)
      {
        for (int i = 0; i < 50; ++i)
        { // Loop a limited number of times
          if (!g_main_context_pending(context))
          {
            break; // Exit loop if no more events are pending
          }
          g_main_context_iteration(context, FALSE); // FALSE for non-blocking
        }
      }
      // GLib will call abort() after this handler returns for fatal errors.
      // No need to explicitly destroy the dialog as the application is terminating.
      return;
    }
    // If GTK not initialized, fall through to default handler call below
  }

  // For all other cases (not a fatal error we handle with a dialog,
  // or GTK not initialized for such an error), call the original default log handler
  // to maintain standard behavior (e.g., console output for warnings/debug messages).
  g_log_default_handler(log_domain, log_level, message, user_data);
}*/

#if defined(_WIN32) && defined(NDEBUG)
// Include windows.h only on Windows Release builds where WinMain is needed
#include <windows.h>
// Windows Release entry point (uses WinMain, console hidden by linker flag)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //g_log_set_default_handler(show_error_dialog_handler, NULL); //for later release

    // g_setenv("GTK_CSD", "0", FALSE);
    // g_setenv("GTK_DEBUG", "interactive", FALSE);

    return app_main_run(0, NULL);
}
#else
// Standard entry point for non-Windows OR Windows Debug builds (uses main, console shown)
int main(int argc, char **argv)
{
    //g_log_set_default_handler(show_error_dialog_handler, NULL); //for later release

    // g_setenv("GTK_CSD", "1", FALSE);
    // g_setenv("GTK_DEBUG", "interactive", FALSE);

    return app_main_run(argc, argv);
}
#endif