#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include "gtk/gtk.h"
#include <glib/gstdio.h>

typedef enum {CSTATE_RESET, CSTATE_INPUT_OP1, CSTATE_INPUT_OPERAND, CSTATE_INPUT_OP2}CSTATE;

typedef struct _Calculator Calculator;

struct _Calculator
{
    double op1,op2;
    char operand; 
    CSTATE cState;
};

G_MODULE_EXPORT double perform_calculation(Calculator *calc);
G_MODULE_EXPORT void calc_on_button_click(GtkWidget *button, gpointer user_data);
G_MODULE_EXPORT void update_display(GtkEntry *entry, const char *text);
G_MODULE_EXPORT void append_to_display(GtkEntry *entry, const char *text);

#endif