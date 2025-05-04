#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "Calculator.h"
#include "gtk/gtk.h"
#include "Application.h"
#include <stdlib.h> // For atof, strtod
#include <stdio.h>  // For snprintf
#include <ctype.h>  // For isdigit

G_MODULE_EXPORT double perform_calculation(Calculator *calc)
{
    switch (calc->operator)
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
            g_warning("Division by zero attempted.\n");
            // Handle error appropriately - maybe return NaN or display "Error"
            // For now, just return 0 and reset state
            calc->cState = CSTATE_RESET;
            return 0.0; // Or NAN maybe?
        }
        return calc->op1 / calc->op2;
    default:
        g_warning("Invalid operator '%c'", calc->operator);
        calc->cState = CSTATE_RESET;
        return 0.0; // Should not happen
    }
}

G_MODULE_EXPORT void update_display(GtkLabel *_label, const char *text)
{
    gtk_label_set_text(GTK_LABEL(_label),text);
}

G_MODULE_EXPORT void append_to_display(GtkLabel *_label, const char *text_to_append)
{
    const char *current_text = gtk_label_get_text(GTK_LABEL(_label));
    gsize current_len = strlen(current_text);
    gsize append_len = strlen(text_to_append);
    // Calculate total size needed: current length + append length + 1 (for null terminator)
    gsize total_size = current_len + append_len + 1;

    // Allocate memory for the new combined string
    char *new_text_buffer = g_malloc(total_size);
    if (!new_text_buffer)
    {
        g_error("Failed to allocate memory in append_to_display");
        // Allocation failed, cannot proceed
    }

    // Copy the current text into the new buffer first.
    // Using strcpy here is safe because we allocated exactly enough space initially.
    strcpy(new_text_buffer, current_text);

    // Append the new text using g_strlcat for safety.
    // g_strlcat ensures null termination and prevents buffer overflows.
    // The third argument is the total size of the destination buffer.
    g_strlcat(new_text_buffer, text_to_append, total_size);

    // Update the label with the newly created string
    gtk_label_set_text(GTK_LABEL(_label), new_text_buffer);

    // Free the memory allocated by g_malloc
    g_free(new_text_buffer);
}

G_MODULE_EXPORT void calc_on_button_click(GtkWidget *button, gpointer user_data)
{
    mainApp *mApp = NULL;
    mApp = (mainApp *)user_data;
    if (!mApp || !mApp->calc || !mApp->label_handle || !mApp->label_preview)
    {
        g_error("MAINAPP POINTER IS NULL\n");
    }

    Calculator *calc = mApp->calc;
    GtkLabel *display_label = GTK_LABEL(mApp->label_handle);
    GtkLabel *preview_label = GTK_LABEL(mApp->label_preview);

    if (GTK_IS_BUTTON(button) && gtk_button_get_label(GTK_BUTTON(button)) != NULL)
    {
        const char *label = gtk_button_get_label(GTK_BUTTON(button));
        const char *current_text = gtk_label_get_text(display_label);
        const char *preview_text = gtk_label_get_text(preview_label);
        char display_buffer[100]; // For formatting results
        char preview_buffer[100];
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
            g_warning("INVALID INPUT!\n");
        }

        // --- Clear Button ---
        if (strcmp(label, "C") == 0 || strcmp(label, "CE") == 0)
        { 
            calc->op1 = 0.0;
            calc->op2 = 0.0;
            calc->operator = '\0';
            calc->cState = CSTATE_RESET;
            update_display(display_label, "0");
            update_display(preview_label, "0");
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
                if (calc->cState == CSTATE_RESET || calc->cState == CSTATE_INPUT_OPERATOR)
                {
                    update_display(display_label, "0.");
                    calc->cState = (calc->cState == CSTATE_RESET) ? CSTATE_INPUT_OP1 : CSTATE_INPUT_OP2;
                    return;
                }
            }

            switch (calc->cState)
            {
            case CSTATE_RESET:                // Starting fresh or after '=' or 'C'
                update_display(display_label, label); // Start new number
                update_display(preview_label, "0");
                calc->cState = CSTATE_INPUT_OP1;
                break;
            case CSTATE_INPUT_OP1:
                // Handle initial "0" - replace if not entering decimal
                if (strcmp(current_text, "0") == 0 && strcmp(label, ".") != 0)
                {
                    update_display(display_label, label);
                }
                else
                {
                    append_to_display(display_label, label);
                }
                break;
            case CSTATE_INPUT_OPERATOR: // Just received an operator, start op2
                update_display(display_label, label);
                calc->cState = CSTATE_INPUT_OP2;
                break;
            case CSTATE_INPUT_OP2:
                // Handle initial "0" for op2 - replace if not entering decimal
                if (strcmp(current_text, "0") == 0 && strcmp(label, ".") != 0)
                {
                    update_display(display_label, label);
                }
                else
                {
                    append_to_display(display_label, label);
                }
                break;
            }
        }
        // --- Operator Buttons ---
        else if (strchr("+-*/", label[0]) && strlen(label) == 1)
        {
            char new_operator = label[0];
            switch (calc->cState)
            {
            case CSTATE_RESET:   // Treat value 0 as op1 if operator pressed first
                calc->op1 = 0.0; // Or maybe use value 0 from display? Let's use 0.
                calc->operator = new_operator;
                calc->cState = CSTATE_INPUT_OPERATOR;
                // Display remains "0"
                break;
            case CSTATE_INPUT_OP1: // First operand entered, store it and the operator
                calc->op1 = current_value;
                calc->operator = new_operator;
                calc->cState = CSTATE_INPUT_OPERATOR;

                //PREVIEW DISPLAY UPDATE
                snprintf(preview_buffer, sizeof(preview_buffer), "%g", calc->op1); // %g removes trailing zeros
                update_display(preview_label, preview_buffer);
                //char* oper_string = label[0];
                //append_to_display(preview_label, oper_string);

                char op_str[2];
                op_str[0] = new_operator; op_str[1] = '\0';
                append_to_display(preview_label, op_str);
                // Display doesn't change yet, waiting for op2
                break;
            case CSTATE_INPUT_OPERATOR: // Operator pressed again, just update the operator
                if (calc->operator != new_operator)
                {
                    calc->operator = new_operator;
                    // PREVIEW DISPLAY UPDATE
                    gsize current_len = strlen(preview_text);
                    char *buffer = g_strdup(preview_text);
                    if (buffer)
                    {                                   // Check if g_strdup succeeded
                        buffer[current_len - 1] = '\0'; // Truncate the string
                        update_display(preview_label, buffer);
                        char op_str[2];
                        op_str[0] = new_operator; op_str[1] = '\0';
                        append_to_display(preview_label, op_str);
                        g_free(buffer);
                    }
                    else
                    {
                        g_warning("Failed to allocate memory for backspace operation.\n");
                    }
                }
                // State remains CSTATE_INPUT_OPERAND
                break;
            case CSTATE_INPUT_OP2: // Second operand entered, calculate intermediate result
                calc->op2 = current_value;
                calc->op1 = perform_calculation(calc); // Result becomes new op1
                calc->operator = new_operator;           // Store the NEW operator
                calc->cState = CSTATE_INPUT_OPERATOR;   // Ready for next op2
                // Display the intermediate result
                snprintf(display_buffer, sizeof(display_buffer), "%g", calc->op1); // %g removes trailing zeros
                update_display(display_label, display_buffer);
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
            case CSTATE_INPUT_OPERATOR:
                // Interpret as op1 operand op1 (e.g., 5 + = -> 10)
                calc->op2 = calc->op1; // Use op1 as op2
                calc->op1 = perform_calculation(calc);
                // Display result
                snprintf(display_buffer, sizeof(display_buffer), "%g", calc->op1);
                update_display(display_label, display_buffer);


                //PREVIEW DISPLAY UPDATE
                //snprintf(preview_buffer, sizeof(preview_buffer), "%g", calc->op2, calc->operator, calc->op2,"="); // %g removes trailing zeros
                snprintf(preview_buffer, sizeof(preview_buffer), "%g", calc->op2);
                update_display(preview_label, preview_buffer);
                char op_str[2];
                op_str[0] = calc->operator; op_str[1] = '\0';
                append_to_display(preview_label,op_str);        
                append_to_display(preview_label,preview_buffer);
                append_to_display(preview_label,"="); 

                // Keep operand, allow repeated equals. State becomes RESET,
                // next number will overwrite display and become new op1.
                calc->cState = CSTATE_RESET;
                break;
            case CSTATE_INPUT_OP2: // Standard calculation
                calc->op2 = current_value;
                //double temp_op1 = calc->op1;
                calc->op1 = perform_calculation(calc); // Result stored in op1
                // Display result
                snprintf(display_buffer, sizeof(display_buffer), "%g", calc->op1);
                update_display(display_label, display_buffer);

                //PREVIEW DISPLAY UPDATE
                snprintf(preview_buffer, sizeof(preview_buffer), "%g", calc->op2);
                append_to_display(preview_label, preview_buffer);
                append_to_display(preview_label,"=");

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
        // --- Backspace Button ---
        else if (strcmp(label, "←") == 0)
        {
            if (calc->cState == CSTATE_INPUT_OP1 || calc->cState == CSTATE_INPUT_OP2)
            {
                gsize current_len = strlen(current_text);

                if (current_len > 1)
                {
                    // Create a mutable copy, remove the last char, update display
                    char *buffer = g_strdup(current_text);
                    if (buffer)
                    {                                   // Check if g_strdup succeeded
                        buffer[current_len - 1] = '\0'; // Truncate the string
                        update_display(display_label, buffer);
                        g_free(buffer);
                    }
                    else
                    {
                        g_warning("Failed to allocate memory for backspace operation.\n");
                    }
                }
                else if (current_len == 1 && current_text[0] != '0')
                {
                    // If only one digit is left (and it's not '0'), change display to "0"
                    update_display(display_label, "0");
                }
                // If current_len is 0 or (len is 1 and text is "0"), do nothing.
            }
            // g_message("Button: '←' was clicked!\n");
        }
        // --- Other Buttons (e.g., +/- , %, etc. - Not implemented) ---
        else
        {
            g_message("Button '%s' not implemented yet.\n", label);
        }
    }
    else
    {
        g_warning("Clicked widget is not a button or has no label.\n");
    }
}