#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stack>


#define PRINT 1

#define STRING_LIT 15
#define STRING_VAR 16
#define NUMBER_LIT 17
#define NUMBER_VAR 18
#define POINTER_VAR 19

#define VAR_NAME 30

#define ASSIGN_OP 100
#define ADD_OP 101
#define SUB_OP 102
#define MULTI_OP 103
#define DIVIDE_OP 104
#define LESSTHAN_OP 105
#define GREATERTHAN_OP 106
#define EQUALS_OP 107

#define START_EXP 150
#define END_EXP 151
#define START_LOOP 152
#define END_LOOP 153
#define LEAVE_LOOP 154
#define CONDITIONAL 155

#define START_FUNC 160
#define END_FUNC 161

#define BLANK 10000