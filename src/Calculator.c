#include "Calculator.h"
#include "gtk/gtk.h"
#include "Application.h"

G_MODULE_EXPORT void calc_on_button_click(GtkWidget *button, gpointer user_data)
{
    mainApp *mApp = NULL;
    mApp = (mainApp *)user_data;
    if(!mApp)
    {
        g_error("MAINAPP POINTER IS NULL\n");
    }

    printf("Button was clicked\n");

    if (GTK_IS_BUTTON(button) && gtk_button_get_label(GTK_BUTTON(button)) != NULL)
    {
        const char *label = gtk_button_get_label(GTK_BUTTON(button));

        printf("Button with label: '%s' was clicked\n", label);
        GtkEntryBuffer *buffer = NULL;
        buffer = gtk_entry_get_buffer(mApp->entry_handle);
        if(!buffer)
        {
            g_error("ENTRY BUFFER POINTER IS NULL\n");
        }

        if(g_strcmp0(label,"C") == 0 || g_strcmp0(label,"CE") == 0)
        {
            gtk_entry_buffer_set_text(buffer,"0",(int)strlen(label));
            printf("ENTRY BUFFER WAS CLEARED\n");
        }
        else
        {
            gtk_entry_buffer_set_text(buffer,label,(int)strlen(label));
        }        
    }
    else
    {
        g_warning("TYPE CAST ERROR, WIDGET IS NOT OF TYPE BUTTON\n");
    }
}