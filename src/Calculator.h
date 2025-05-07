#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include "gtk/gtk.h"
#include <glib/gstdio.h>

typedef enum {CSTATE_RESET, CSTATE_INPUT_OP1, CSTATE_INPUT_OPERATOR, CSTATE_INPUT_OP2}CSTATE; //maybe add CSTATE_ERROR state for division by zero?

typedef struct _Calculator Calculator;

struct _Calculator
{
    double op1,op2;
    char operator; 
    CSTATE cState;
    gboolean just_calculated;
};

G_MODULE_EXPORT double perform_calculation(Calculator *calc);
G_MODULE_EXPORT void calc_on_button_click(GtkWidget *button, gpointer user_data);
G_MODULE_EXPORT void update_display(GtkLabel *_label, const char *text);
G_MODULE_EXPORT void append_to_display(GtkLabel *_label, const char *text_to_append);

#endif