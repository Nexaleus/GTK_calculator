#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "Calculator.h"
#include "gtk/gtk.h"
#include "Application.h"
#include <stdlib.h> // For atof, strtod
#include <stdio.h>  // For snprintf
#include <ctype.h>  // For isdigit
#include <math.h>

G_MODULE_EXPORT double perform_calculation(Calculator *calc)
{
    double result = 0.0;

    // Check for potential errors before performing calculation
    if (calc->operator == '/' && calc->op2 == 0)
    {
        g_warning("Division by zero attempted.\n");
        // Indicate error state, maybe display "Error"
        calc->cState = CSTATE_RESET;   // Or a specific CSTATE_ERROR
        calc->just_calculated = FALSE; // Not a valid calculation result
        return NAN;                    // Return Not-a-Number to signify error
    }

    if (calc->cState != CSTATE_INPUT_OP2)
    {
        g_warning("perform_calculation called in invalid state (%d).\n", calc->cState);
        // Don't change state drastically, just return current op1 maybe?
        // Or reset? Let's return op1 for now.
        return calc->op1;
    }

    switch (calc->operator)
    {
    case '+':
        result = calc->op1 + calc->op2;
        break;
    case '-':
        result = calc->op1 - calc->op2;
        break;
    case '*':
        result = calc->op1 * calc->op2;
        break;
    case '/':
        result = calc->op1 / calc->op2;
        break;
    default:
        g_warning("Invalid operator '%c'", calc->operator);
        calc->cState = CSTATE_RESET;
        calc->just_calculated = FALSE;
        return NAN; // Return error
        break;
    }

    // Store result back into op1 for potential chaining
    calc->op1 = result;
    // Set flag indicating a calculation was just performed
    calc->just_calculated = TRUE;
    // Reset state, ready for new input or operator chaining
    calc->cState = CSTATE_RESET; // Or CSTATE_INPUT_OP1 if result is shown? Let's stick with RESET for now.

    return result;
}

G_MODULE_EXPORT void update_display(GtkLabel *_label, const char *text)
{
    gtk_label_set_text(GTK_LABEL(_label), text);
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
        // const char *preview_text = gtk_label_get_text(preview_label); // Keep for reference if needed
        char display_buffer[100];   // For formatting results
        char preview_buffer[100];   // For formatting preview
        double current_value = 0.0; // Initialize

        // Attempt to parse the current display value
        char *endptr;
        current_value = strtod(current_text, &endptr);

        // More robust check for invalid input on display
        if (endptr == current_text && strcmp(current_text, "0") != 0 && strcmp(current_text, "") != 0 && strcmp(current_text, ".") != 0 && strcmp(current_text, "0.") != 0)
        {
            if (strcmp(current_text, "Error") != 0)
            { // Don't warn if it's a known "Error" string
                g_warning("Display contains non-numeric value: '%s', parsed as %f\n", current_text, current_value);
            }
            // If display is "Error", current_value will be 0.0. Subsequent operations might use this.
        }

        // --- Clear Button ---
        if (strcmp(label, "C") == 0 || strcmp(label, "CE") == 0)
        {
            calc->op1 = 0.0;
            calc->op2 = 0.0;
            calc->operator = '\0';
            calc->cState = CSTATE_RESET;
            calc->just_calculated = FALSE; // Explicitly reset this flag
            update_display(display_label, "0");
            update_display(preview_label, "0"); // Clear preview
        }
        // --- Digit or Decimal Point ---
        else if (isdigit(label[0]) || (strcmp(label, ".") == 0))
        {
            if (calc->just_calculated)
            {
                // A result was just shown (or C pressed). Start new number.
                update_display(preview_label, "0"); // Clear preview
                if (strcmp(label, ".") == 0)
                {
                    update_display(display_label, "0.");
                }
                else
                {
                    update_display(display_label, label);
                }
                calc->cState = CSTATE_INPUT_OP1;
                calc->just_calculated = FALSE; // Now inputting a new number
            }
            else // Not immediately after a calculation, proceed with normal input logic
            {
                if (strcmp(label, ".") == 0)
                {
                    // Prevent multiple decimals if already inputting op1 or op2
                    if (strchr(current_text, '.') != NULL &&
                        (calc->cState == CSTATE_INPUT_OP1 || calc->cState == CSTATE_INPUT_OP2))
                    {
                        return; // Already has a decimal
                    }
                    // If starting op1 or op2 with a decimal, or display is "0"
                    if (calc->cState == CSTATE_RESET || calc->cState == CSTATE_INPUT_OPERATOR || strcmp(current_text, "0") == 0)
                    {
                        update_display(display_label, "0.");
                        // Determine if this starts op1 or op2
                        if (calc->cState == CSTATE_INPUT_OPERATOR || (calc->cState == CSTATE_INPUT_OP2 && strcmp(current_text, "0") == 0))
                        {
                            calc->cState = CSTATE_INPUT_OP2;
                        }
                        else
                        {
                            calc->cState = CSTATE_INPUT_OP1;
                        }
                        return;
                    }
                }

                // Standard digit input based on current state
                switch (calc->cState)
                {
                case CSTATE_RESET: // e.g. after C, or initial state (but not after '=')
                    if (strcmp(label, ".") == 0)
                        update_display(display_label, "0.");
                    else
                        update_display(display_label, label);
                    update_display(preview_label, "0"); // Reset preview
                    calc->cState = CSTATE_INPUT_OP1;
                    break;
                case CSTATE_INPUT_OP1:
                    if (strcmp(current_text, "0") == 0 && strcmp(label, ".") != 0)
                    {
                        update_display(display_label, label); // Replace "0"
                    }
                    else
                    {
                        append_to_display(display_label, label);
                    }
                    break;
                case CSTATE_INPUT_OPERATOR: // Operator was just pressed, start op2
                    if (strcmp(label, ".") == 0)
                        update_display(display_label, "0.");
                    else
                        update_display(display_label, label);
                    calc->cState = CSTATE_INPUT_OP2;
                    break;
                case CSTATE_INPUT_OP2:
                    if (strcmp(current_text, "0") == 0 && strcmp(label, ".") != 0)
                    {
                        update_display(display_label, label); // Replace "0" for op2
                    }
                    else
                    {
                        append_to_display(display_label, label);
                    }
                    break;
                }
            }
        }
        // --- Operator Buttons ---
        else if (strchr("+-*/", label[0]) && strlen(label) == 1)
        {
            char new_operator = label[0];
            char op_char_str[2] = {new_operator, '\0'};

            if (calc->just_calculated)
            {
                // op1 holds the result of the previous calculation. Use it.
                // Display already shows op1 (the result).
                calc->operator = new_operator;
                calc->cState = CSTATE_INPUT_OPERATOR;
                calc->just_calculated = FALSE; // No longer just calculated, now chaining
                // Update preview: "result op"
                snprintf(preview_buffer, sizeof(preview_buffer), "%g %s", calc->op1, op_char_str);
                update_display(preview_label, preview_buffer);
            }
            else // Not immediately after a calculation
            {
                switch (calc->cState)
                {
                case CSTATE_RESET:             // Not after '=', e.g. after 'C' or initial state. Display shows "0".
                    calc->op1 = current_value; // current_value is 0 if display is "0"
                    calc->operator = new_operator;
                    calc->cState = CSTATE_INPUT_OPERATOR;
                    snprintf(preview_buffer, sizeof(preview_buffer), "%g %s", calc->op1, op_char_str);
                    update_display(preview_label, preview_buffer);
                    // Display (main) remains "0" or current_value, op2 input will overwrite it
                    break;
                case CSTATE_INPUT_OP1: // First operand entered
                    calc->op1 = current_value;
                    calc->operator = new_operator;
                    calc->cState = CSTATE_INPUT_OPERATOR;
                    snprintf(preview_buffer, sizeof(preview_buffer), "%g %s", calc->op1, op_char_str);
                    update_display(preview_label, preview_buffer);
                    // Main display doesn't change yet, op2 input will go there
                    break;
                case CSTATE_INPUT_OPERATOR: // Operator pressed again, change current operator
                    calc->operator = new_operator;
                    // Preview should be "op1 new_operator"
                    snprintf(preview_buffer, sizeof(preview_buffer), "%g %s", calc->op1, op_char_str);
                    update_display(preview_label, preview_buffer);
                    break;
                case CSTATE_INPUT_OP2: // Second operand entered, calculate intermediate, then apply new op
                    calc->op2 = current_value;

                    double temp_op1_for_preview = calc->op1; // For preview before op1 is overwritten
                    char temp_current_op_for_preview = calc->operator;

                    calc->op1 = perform_calculation(calc); // Result becomes new op1. Sets just_calculated=TRUE, cState=CSTATE_RESET

                    if (isnan(calc->op1))
                    { // Error from perform_calculation
                        update_display(display_label, "Error");
                        snprintf(preview_buffer, sizeof(preview_buffer), "%g %c %g = Error", temp_op1_for_preview, temp_current_op_for_preview, calc->op2);
                        update_display(preview_label, preview_buffer);
                        // calc->cState is CSTATE_RESET, calc->just_calculated is FALSE (from perform_calculation error path)
                    }
                    else
                    {
                        // Chain the new operator
                        calc->operator = new_operator;        // Apply the NEW operator
                        calc->cState = CSTATE_INPUT_OPERATOR; // Ready for next op2
                        calc->just_calculated = FALSE;        // Not a final result display, it's chaining

                        snprintf(display_buffer, sizeof(display_buffer), "%g", calc->op1);
                        update_display(display_label, display_buffer); // Show intermediate result

                        // Preview for the new operation: "intermediate_result new_op"
                        snprintf(preview_buffer, sizeof(preview_buffer), "%g %s", calc->op1, op_char_str);
                        update_display(preview_label, preview_buffer);
                    }
                    break;
                }
            }
        }
        // --- Equals Button ---
        else if (strcmp(label, "=") == 0)
        {
            // Temporary strings for constructing preview before state changes
            char temp_op1_str[50], temp_op2_str[50];
            char temp_operator_char = calc->operator; // Capture before it might be cleared or changed

            switch (calc->cState)
            {
            case CSTATE_RESET: // e.g. "5 =" (after 5 was a result) or "0 =" (initial) or after "C"
                // If just_calculated is TRUE, op1 holds the result. "result =" means result.
                // If just_calculated is FALSE (e.g. after C), op1 is 0 (or what's on display if parsed). "0 =" means 0.
                // No actual calculation, just finalize the display of op1.
                // current_value would be from display, op1 might be from previous calc.
                // If just_calculated, op1 is the one to show. Otherwise, current_value from display.
                double val_to_show = calc->just_calculated ? calc->op1 : current_value;

                snprintf(display_buffer, sizeof(display_buffer), "%g", val_to_show);
                update_display(display_label, display_buffer);
                snprintf(preview_buffer, sizeof(preview_buffer), "%g =", val_to_show);
                update_display(preview_label, preview_buffer);

                calc->op1 = val_to_show;      // Ensure op1 has this value for next step if operator is pressed
                calc->just_calculated = TRUE; // It's now a displayed result
                calc->cState = CSTATE_RESET;  // Remains/becomes reset
                break;
            case CSTATE_INPUT_OP1:         // e.g., user types "5" then "="
                calc->op1 = current_value; // op1 is what's on display
                // No actual calculation, op1 is the result.
                snprintf(display_buffer, sizeof(display_buffer), "%g", calc->op1);
                update_display(display_label, display_buffer);
                snprintf(preview_buffer, sizeof(preview_buffer), "%g =", calc->op1);
                update_display(preview_label, preview_buffer);
                calc->just_calculated = TRUE; // Set flag
                calc->cState = CSTATE_RESET;  // Reset state
                break;
            case CSTATE_INPUT_OPERATOR: // e.g., "5 + =" (op1 is 5, operator is +)
                calc->op2 = calc->op1;  // Use op1 as op2 (standard behavior for "op =")

                snprintf(temp_op1_str, sizeof(temp_op1_str), "%g", calc->op1); // op1 before calculation
                snprintf(temp_op2_str, sizeof(temp_op2_str), "%g", calc->op2); // op2 (which is same as op1 before calc)

                calc->op1 = perform_calculation(calc); // perform_calculation sets just_calculated=TRUE, cState=CSTATE_RESET on success

                if (isnan(calc->op1))
                { // Error from perform_calculation
                    update_display(display_label, "Error");
                    snprintf(preview_buffer, sizeof(preview_buffer), "%s %c %s = Error", temp_op1_str, temp_operator_char, temp_op2_str);
                    update_display(preview_label, preview_buffer);
                    // calc->cState is CSTATE_RESET, calc->just_calculated is FALSE (from perform_calculation error path)
                }
                else
                {
                    snprintf(display_buffer, sizeof(display_buffer), "%g", calc->op1);
                    update_display(display_label, display_buffer);
                    snprintf(preview_buffer, sizeof(preview_buffer), "%s %c %s =", temp_op1_str, temp_operator_char, temp_op2_str);
                    update_display(preview_label, preview_buffer);
                    // calc->just_calculated is TRUE, calc->cState is CSTATE_RESET (from perform_calculation)
                }
                break;
            case CSTATE_INPUT_OP2:         // Standard calculation: "5 + 3 ="
                calc->op2 = current_value; // op2 is from current display

                snprintf(temp_op1_str, sizeof(temp_op1_str), "%g", calc->op1); // op1 before calculation
                snprintf(temp_op2_str, sizeof(temp_op2_str), "%g", calc->op2); // op2 from display

                calc->op1 = perform_calculation(calc); // perform_calculation sets just_calculated=TRUE, cState=CSTATE_RESET on success

                if (isnan(calc->op1))
                { // Error from perform_calculation
                    update_display(display_label, "Error");
                    snprintf(preview_buffer, sizeof(preview_buffer), "%s %c %s = Error", temp_op1_str, temp_operator_char, temp_op2_str);
                    update_display(preview_label, preview_buffer);
                    // calc->cState is CSTATE_RESET, calc->just_calculated is FALSE (from perform_calculation error path)
                }
                else
                {
                    snprintf(display_buffer, sizeof(display_buffer), "%g", calc->op1);
                    update_display(display_label, display_buffer);
                    snprintf(preview_buffer, sizeof(preview_buffer), "%s %c %s =", temp_op1_str, temp_operator_char, temp_op2_str);
                    update_display(preview_label, preview_buffer);
                    // calc->just_calculated is TRUE, calc->cState is CSTATE_RESET (from perform_calculation)
                }
                break;
            }
        }
        // --- Backspace Button ---
        else if (strcmp(label, "←") == 0)
        {
            // Backspace should not operate if a result was just calculated and not yet cleared by new input
            if (calc->just_calculated)
            {
                // Optionally, could clear the 'just_calculated' state and allow backspace on the result
                // For now, let's make it do nothing if a result is freshly displayed.
                return;
            }

            if (calc->cState == CSTATE_INPUT_OP1 || calc->cState == CSTATE_INPUT_OP2)
            {
                gsize current_len = strlen(current_text);

                if (current_len > 1)
                {
                    char *buffer = g_strdup(current_text);
                    if (buffer)
                    {
                        buffer[current_len - 1] = '\0';
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
                    update_display(display_label, "0");
                }
                // If current_len is 0 or (len is 1 and text is "0"), do nothing.
            }
            // No action for backspace if in CSTATE_RESET or CSTATE_INPUT_OPERATOR,
            // or if just_calculated is true.
        }
        // --- Other Buttons (e.g., +/- , %, etc. - Not implemented) ---
        else
        {
            g_warning("Button '%s' not implemented yet.\n", label);
        }
    }
    else
    {
        g_warning("Clicked widget is not a button or has no label.\n");
    }
}