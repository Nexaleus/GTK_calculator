#include "Calculator.h"
#include "gtk/gtk.h"

G_MODULE_EXPORT void calc_on_button_click(GtkWidget *button, gpointer user_data)
{
    if(GTK_IS_BUTTON(button))
    {
        
    }
    else
    {
        g_warning("TYPE CAST ERROR, WIDGET IS NOT OF TYPE BUTTON");
    }
}