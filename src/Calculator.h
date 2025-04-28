#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

typedef enum {CSTATE_RESET, CSTATE_INPUT_OP1, CSTATE_INPUT_OPERAND, CSTATE_INPUT_OP2}CSTATE;

typedef struct _Calculator Calculator;

struct _Calculator
{
    //int a,b;
    CSTATE cState;
};


#endif