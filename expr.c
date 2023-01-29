// Parsing of expressions

#include "defs.h"
#include "data.h"
#include "decl.h"

// Operator precedence for each token.
static int OpPrec[] = {0, 10, 10, 20, 20, 0};

// Convert a token into an AST operation.
int arithop(int token) {
    switch (token) {
    case T_PLUS:
        return (A_ADD);
    case T_MINUS:
        return (A_SUBTRACT);
    case T_STAR:
        return (A_MULTIPLY);
    case T_SLASH:
        return (A_DIVIDE);
    default:
        fprintf(stderr, "unknown token in arithop() on line %d: token type = %d\n", Line, token);
        exit(1);
    }
}

// Parse a primary factor and return an AST node representing it.
static struct ASTnode *primary(void) {
    struct ASTnode *n;

    // For an INTLIT token, make a leaf AST node for it and scan in the next token.
    // Otherwise, a syntax error for any other token type.
    switch (Token.token) {
    case T_INTLIT:
        n = makeastleaf(A_INTLIT, Token.intvalue);
        scan(&Token);
        return (n);
    default:
        fprintf(stderr, "syntax error on line %d\n", Line);
        exit(1);
    }
}


// Return an AST tree whose root is a '*' or '/' binary operator
struct ASTnode *multiplicative_expr(void) {
    struct ASTnode *left, *right;
    int tokentype;

    // Get the integer literal on the left.
    left = primary();

    // If no tokens left, return just the left node.
    tokentype = Token.token;
    if (tokentype == T_EOF) {
        return left;
    }

    // While the token is a '*' or '/'.
    while ((tokentype == T_STAR) || (tokentype == T_SLASH))  {
        // Fetch in the next integer literal
        scan(&Token);
        right = primary();

        // Join that with the left integer literal
        left = makeastnode(arithop(tokentype), left, right, 0);

        // Update the details of the current token.
        // If no tokens left, return just the left node
        tokentype = Token.token;
        if (tokentype == T_EOF) {
            break;
        }
    }

    // Return whatever tree we have created
    return left;
}

// Return an AST tree whose root is a '+' or '-' binary operator
struct ASTnode *additive_expr(void) {
    struct ASTnode *left, *right;
    int tokentype;

    // Get the multiplicative expression on the left.
    left = multiplicative_expr();

    // If no tokens left, return just the left node.
    tokentype = Token.token;
    if (tokentype == T_EOF) {
        return left;
    }

    // Loop the additive expressions.
    while (1) {
        // Fetch in the next multiplicative expression.
        scan(&Token);
        right = multiplicative_expr();

        // Join that with the left integer literal
        left = makeastnode(arithop(tokentype), left, right, 0);

        // Update the details of the current token.
        // If no tokens left, return just the left node
        tokentype = Token.token;
        if (tokentype == T_EOF) {
            break;
        }
    }

    return left;
}

// Check that we have a binary operator and return its precedence.
static int op_precedence(int tokentype) {
    int prec = OpPrec[tokentype];
    if (prec == 0) {
        fprintf(stderr, "syntax error on line %d, token %d\n", Line, tokentype);
        exit(1);
    }
    return prec;
}

// Return an AST tree whose root is a binary operator
struct ASTnode *binexpr(int ptp) {
    struct ASTnode *left, *right;
    int tokentype;

    // Get the integer literal on the left.
    // Fetch the next token at the same time.
    left = primary();

    // If no tokens left, return just the left node
    tokentype = Token.token;
    if (tokentype == T_SEMI)
        return left;

    // While the precedence of this token is larger than the previous one's:
    while (op_precedence(tokentype) > ptp) {
        // Fetch in the next integer literal.
        scan(&Token);

        // Recursively build up the right subtree.
        right = binexpr(op_precedence(tokentype));

        // Join the subtrees.
        left = makeastnode(arithop(tokentype), left, right, 0);

        // Update the details of the current token.
        tokentype = Token.token;
        if (tokentype == T_SEMI) {
            return left;
        }
    }

    return left;
}