#include "Calculator.h"
#include "gtk/gtk.h"
#include "Application.h"
#include <stdlib.h> // For atof, strtod
#include <stdio.h>  // For snprintf
#include <ctype.h>  // For isdigit

G_MODULE_EXPORT double perform_calculation(Calculator *calc)
{
    switch (calc->operand)
    {
    case '+':
        return calc->op1 + calc->op2;
    case '-':
        return calc->op1 - calc->op2;
    case '*':
        return calc->op1 * calc->op2;
    case '/':
        if (calc->op2 == 0)
        {
            g_warning("Division by zero attempted.");
            // Handle error appropriately - maybe return NaN or display "Error"
            // For now, just return 0 and reset state
            calc->cState = CSTATE_RESET;
            return 0.0; // Or NAN maybe?
        }
        return calc->op1 / calc->op2;
    default:
        g_warning("Invalid operand '%c'", calc->operand);
        calc->cState = CSTATE_RESET;
        return 0.0; // Should not happen
    }
}

G_MODULE_EXPORT void update_display(GtkEntry *entry, const char *text)
{
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(entry);
    gtk_entry_buffer_set_text(buffer, text, -1);
}

G_MODULE_EXPORT void append_to_display(GtkEntry *entry, const char *text)
{
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(entry);
    gtk_entry_buffer_insert_text(buffer, gtk_entry_buffer_get_length(buffer), text, -1);
}

G_MODULE_EXPORT void calc_on_button_click(GtkWidget *button, gpointer user_data)
{
    mainApp *mApp = NULL;
    mApp = (mainApp *)user_data;
    if (!mApp || !mApp->calc || !mApp->entry_handle)
    {
        g_error("MAINAPP POINTER IS NULL\n");
    }

    Calculator *calc = mApp->calc;
    GtkEntry *entry = GTK_ENTRY(mApp->entry_handle);

    if (GTK_IS_BUTTON(button) && gtk_button_get_label(GTK_BUTTON(button)) != NULL)
    {
        const char *label = gtk_button_get_label(GTK_BUTTON(button));
        const char *current_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
        char display_buffer[100]; // For formatting results
        double current_value;

        // Attempt to parse the current display value
        // Use strtod for better error handling than atof
        char *endptr;
        current_value = strtod(current_text, &endptr);
        // Check if conversion was successful (basic check)
        if (endptr == current_text && current_value == 0.0)
        {
            // Handle cases like empty string or invalid input if necessary
            // For simplicity, we often assume the display holds a valid number or "0"
        }

        // --- Clear Button ---
        if (strcmp(label, "C") == 0 || strcmp(label, "CE") == 0)
        { 
            calc->op1 = 0.0;
            calc->op2 = 0.0;
            calc->operand = '\0';
            calc->cState = CSTATE_RESET;
            update_display(entry, "0");
        }
        // --- Digit or Decimal Point ---
        else if (isdigit(label[0]) || (strcmp(label, ".") == 0))
        {
            // Prevent multiple decimals in the current number being entered
            if (strcmp(label, ".") == 0)
            {
                // Only allow decimal if not already present AND we are in a state
                // where we are actively inputting a number
                if (strchr(current_text, '.') != NULL &&
                    (calc->cState == CSTATE_INPUT_OP1 || calc->cState == CSTATE_INPUT_OP2))
                {
                    return; // Already has a decimal point in the current input
                }
                // Special case: if state is RESET or OPERAND, start with "0."
                if (calc->cState == CSTATE_RESET || calc->cState == CSTATE_INPUT_OPERAND)
                {
                    update_display(entry, "0.");
                    calc->cState = (calc->cState == CSTATE_RESET) ? CSTATE_INPUT_OP1 : CSTATE_INPUT_OP2;
                    return;
                }
            }

            switch (calc->cState)
            {
            case CSTATE_RESET:                // Starting fresh or after '=' or 'C'
                update_display(entry, label); // Start new number
                calc->cState = CSTATE_INPUT_OP1;
                break;
            case CSTATE_INPUT_OP1:
                // Handle initial "0" - replace if not entering decimal
                if (strcmp(current_text, "0") == 0 && strcmp(label, ".") != 0)
                {
                    update_display(entry, label);
                }
                else
                {
                    append_to_display(entry, label);
                }
                break;
            case CSTATE_INPUT_OPERAND: // Just received an operator, start op2
                update_display(entry, label);
                calc->cState = CSTATE_INPUT_OP2;
                break;
            case CSTATE_INPUT_OP2:
                // Handle initial "0" for op2 - replace if not entering decimal
                if (strcmp(current_text, "0") == 0 && strcmp(label, ".") != 0)
                {
                    update_display(entry, label);
                }
                else
                {
                    append_to_display(entry, label);
                }
                break;
            }
        }
        // --- Operator Buttons ---
        else if (strchr("+-*/", label[0]) && strlen(label) == 1)
        {
            char new_operand = label[0];
            switch (calc->cState)
            {
            case CSTATE_RESET:   // Treat value 0 as op1 if operator pressed first
                calc->op1 = 0.0; // Or maybe use value 0 from display? Let's use 0.
                calc->operand = new_operand;
                calc->cState = CSTATE_INPUT_OPERAND;
                // Display remains "0"
                break;
            case CSTATE_INPUT_OP1: // First operand entered, store it and the operator
                calc->op1 = current_value;
                calc->operand = new_operand;
                calc->cState = CSTATE_INPUT_OPERAND;
                // Display doesn't change yet, waiting for op2
                break;
            case CSTATE_INPUT_OPERAND: // Operator pressed again, just update the operator
                calc->operand = new_operand;
                // State remains CSTATE_INPUT_OPERAND
                break;
            case CSTATE_INPUT_OP2: // Second operand entered, calculate intermediate result
                calc->op2 = current_value;
                calc->op1 = perform_calculation(calc); // Result becomes new op1
                calc->operand = new_operand;           // Store the NEW operator
                calc->cState = CSTATE_INPUT_OPERAND;   // Ready for next op2
                // Display the intermediate result
                snprintf(display_buffer, sizeof(display_buffer), "%g", calc->op1); // %g removes trailing zeros
                update_display(entry, display_buffer);
                break;
            }
        }
        // --- Equals Button ---
        else if (strcmp(label, "=") == 0)
        {
            switch (calc->cState)
            {
            case CSTATE_RESET:
            case CSTATE_INPUT_OP1:
                // Do nothing, no calculation pending involving a second operand
                break;
            case CSTATE_INPUT_OPERAND:
                // Interpret as op1 operand op1 (e.g., 5 + = -> 10)
                calc->op2 = calc->op1; // Use op1 as op2
                calc->op1 = perform_calculation(calc);
                // Display result
                snprintf(display_buffer, sizeof(display_buffer), "%g", calc->op1);
                update_display(entry, display_buffer);
                // Keep operand, allow repeated equals. State becomes RESET,
                // next number will overwrite display and become new op1.
                calc->cState = CSTATE_RESET;
                break;
            case CSTATE_INPUT_OP2: // Standard calculation
                calc->op2 = current_value;
                calc->op1 = perform_calculation(calc); // Result stored in op1
                // Display result
                snprintf(display_buffer, sizeof(display_buffer), "%g", calc->op1);
                update_display(entry, display_buffer);
                // Reset state, ready for new calculation. Op1 holds result.
                // Operand is cleared or kept depending on desired behavior for repeated equals.
                // Let's clear it to match simple calculators.
                // Keep op1 so result can be used in next calculation.
                // Keep op2 for repeated equals press.
                calc->cState = CSTATE_RESET; // Ready for new op1 entry
                // To allow repeated equals (e.g., 2 * 3 = = = -> 6, 18, 54)
                // we need to keep the last operand and op2, but the state becomes
                // something like CSTATE_RESULT. For simplicity, CSTATE_RESET is used here,
                // which means pressing '=' again won't repeat the operation with the last op2.
                // A more complex state machine could handle that.
                break;
            }
        }
        // --- Other Buttons (e.g., +/- , %, etc. - Not implemented) ---
        else
        {
            g_message("Button '%s' not implemented yet.", label);
        }
    }
    else
    {
        g_warning("Clicked widget is not a button or has no label.");
    }
}