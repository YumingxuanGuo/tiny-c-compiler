// Miscellaneous helper functions

#include "defs.h"
#include "data.h"
#include "decl.h"

// Ensure that the current token is t, and fetch the next token. 
// Otherwise throw an error.
void match(int t, char *str) {
    if (Token.token == t) {
        scan(&Token);
    } else {
        printf("%s expected on line %d\n", str, Line);
        exit(1);
    }
}

// Match a semicon and fetch the next token.
void semi(void) {
    match(T_SEMI, ";");
}
