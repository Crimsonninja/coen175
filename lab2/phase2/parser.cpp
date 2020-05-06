# include <cerrno>
# include <cstdlib>
# include <iostream>
# include "lexer.h"
# include "tokens.h"
# include "string.h"

using namespace std;

int peek();
void match(int t);
bool isSpecifier();
void pointers();
void expression();
void expression_prime();
void logicalAndExpression();
void logicalAndExpression_prime();
void equalityExpression();
void equalityExpression_prime();
void relationalExpression();
void relationalExpression_prime();
void additiveExpression();
void additiveExpression_prime();
void multiplicativeExpression();
void multiplicativeExpression_prime();
void prefixExpression();
void postfixExpression();
void postfixExpression_prime();
void primaryExpression();
void expressionList();
void assignment();
void declarator();
void declarator_prime();
void declarator_list();
void declaration();
void declarations();
void statement();
void statements();
void parameter();
void parameter_list();
void parameters();
void global_declarator();
void remainingDecls();
void functionOrGlobal();

int lookahead, nexttoken;
string lexbuf, nextbuf;

int main() {
    lookahead = yylex();
    while(lookahead!=DONE) {
        functionOrGlobal();
    }
}

int peek() {
  if (!nexttoken) {
    nexttoken = yylex();
    nextbuf = yytext;
  }
  return nexttoken;
}

void match(int t) {
  if (lookahead!=t) {
    report("Does not match. Error at %s",yytext);
    exit(1);
  }
  if (nexttoken) {
    lookahead = nexttoken;
    lexbuf = nextbuf;
    nexttoken = 0;
  } else {
    lookahead = yylex();
    lexbuf = yytext;
  }
}

bool isSpecifier(int token) {
    return token==CHAR||token==INT||token==DOUBLE;
}

void pointers() {
    while(lookahead==STAR) {
        match(STAR);
    }
}

void specifier() {
    if (isSpecifier(lookahead)) {
        match(lookahead);
    } else {
        report("Is not a legal specifier. Error at %s",yytext);
    }

}

void expression() {
    logicalAndExpression();
    expression_prime();
}

void expression_prime() {
    while (lookahead == OR) {
        match(OR);
        logicalAndExpression();
        cout << "or" << endl;
    }
}

void logicalAndExpression() {
    equalityExpression();
    logicalAndExpression_prime();
}

void logicalAndExpression_prime() {
    while (lookahead == AND) {
        match(AND);
        equalityExpression();
        cout << "and" << endl;
    }
}

void equalityExpression() {
    relationalExpression();
    equalityExpression_prime();
}

void equalityExpression_prime() {
    while(1) {
        if (lookahead == EQL) {
            match(EQL);
            relationalExpression();
            cout << "eql" << endl;
        } else if (lookahead==NEQ) {
            match(NEQ);
            relationalExpression();
            cout << "neq" << endl;
        }
        else {
            break;
        }
    }
}

void relationalExpression() {
    additiveExpression();
    relationalExpression_prime();
}

void relationalExpression_prime() {
    while(1) {
        if (lookahead == LTN) {
            match(LTN);
            additiveExpression();
            cout << "ltn" << endl;
        } else if (lookahead == GTN) {
            match(GTN);
            additiveExpression();
            cout << "gtn" << endl;
        } else if (lookahead == LEQ) {
            match(LEQ);
            additiveExpression();
            cout << "leq" << endl;
        } else if (lookahead == GEQ) {
            match(GEQ);
            additiveExpression();
            cout << "geq" << endl;
        } else {
            break;
        }
    }
}

void additiveExpression() {
    multiplicativeExpression();
    additiveExpression_prime();
}

void additiveExpression_prime() {
    while(1) {
        if (lookahead == PLUS) {
            match(PLUS);
            multiplicativeExpression();
            cout << "add" << endl;
        } else if (lookahead == MINUS) {
            match(MINUS);
            multiplicativeExpression();
            cout << "sub" << endl;
        } else {
            break;
        }
    }
}

void multiplicativeExpression() {
    prefixExpression();
    multiplicativeExpression_prime();
}

void multiplicativeExpression_prime() {
    while(1) {
        if (lookahead == STAR) {
            match(STAR);
            prefixExpression();
            cout << "mul" << endl;
        } else if (lookahead == DIV) {
            match(DIV);
            prefixExpression();
            cout << "div" << endl;
        } else if (lookahead == REM) {
            match(REM);
            prefixExpression();
            cout << "rem" << endl;
        } else {
            break;
        }
    }
}

void prefixExpression() {
    if (lookahead == ADDR) {
        match(ADDR);
        prefixExpression();
        cout << "addr" << endl;
    }
    else if (lookahead == STAR) {
        match(STAR);
        prefixExpression();
        cout << "deref" << endl;
    }
    else if (lookahead == NOT) {
        match(NOT);
        prefixExpression();
        cout << "not" << endl;
    }
    else if (lookahead==LPAREN && (peek()==CHAR||peek()==INT||peek()==DOUBLE)) {
        match(LPAREN);
        specifier();
        pointers();
        match(RPAREN);
        prefixExpression();
        cout << "cast" << endl;
    }
    else if (lookahead==SIZEOF) {
        match(SIZEOF);
        if (lookahead!=LPAREN) {
            prefixExpression();
        } else {
            if (isSpecifier(peek())) {
                match(LPAREN);
                specifier();
                pointers();
                match(RPAREN);
            } else {
                match(LPAREN);
                expression();
                match(RPAREN);
            }
        }
        cout << "sizeof" << endl;
    }
    else if (lookahead == MINUS) {
        match(MINUS);
        prefixExpression();
        cout << "neg" << endl;
    } else {
        postfixExpression();
    }
    //postfixExpression();
}

void postfixExpression() {
    primaryExpression();
    postfixExpression_prime();
}

void postfixExpression_prime() {
    while(1) {
        if (lookahead == LBRACK) {
            match(LBRACK);
            expression();
            match(RBRACK);
            cout << "index" << endl;
        } else if (lookahead == INC) {
            match(INC);
            cout << "inc" << endl;
        } else if (lookahead == DEC) {
            match(DEC);
            cout << "dec" << endl;
        } else {
            break;
        }
    }
}

void primaryExpression() {
    if (lookahead == LPAREN) {
        match(LPAREN);
        expression();
        match(RPAREN);
    } else if (lookahead==STRING){
        match(STRING);
    } else if (lookahead==ID) {
        match(ID);
        if (lookahead==LPAREN) {
            match(LPAREN);
            if(lookahead!=RPAREN) {
                expressionList();
            }
            match(RPAREN);
        }
    } else if (lookahead==INTEGER) {
        match(INTEGER);
    } else if (lookahead==REAL) {
        match(REAL);
    } else if (lookahead==CHARACTER) {
        match(CHARACTER);
    } else {
        report("Not one of the primary expressions. Error at %s",yytext);
        exit(1);
    }
}

void expressionList() {
    expression();
    while(lookahead==',') {
        match(',');
        expression();
    }
}

void assignment() {
    expression();
    if (lookahead==ASSIGN) {
        match(ASSIGN);
        expression();
    }
}

void declarator() {
    pointers();
    match(ID);
    declarator_prime();
}

void declarator_prime() {
    if (lookahead==LBRACK) {
        match(LBRACK);
        match(INTEGER);
        match(RBRACK);
    }
}

void declarator_list() {
    declarator();
    while(lookahead==',') {
        match(',');
        declarator();
    }
}

void declaration() {
    specifier();
    declarator_list();
    match(';');
}

void declarations() {
    while(isSpecifier(lookahead)) {
        declaration();
    }
}

void statement() {
    if (lookahead==LBRACE) {
        match(LBRACE);
        declarations();
        statements();
        match(RBRACE);
    } else if (lookahead==BREAK) {
        match(BREAK);
        match(';');
    } else if (lookahead==RETURN){
        match(RETURN);
        expression();
        match(';');
    } else if (lookahead==WHILE) {
        match(WHILE);
        match(LPAREN);
        expression();
        match(RPAREN);
        statement();
    } else if (lookahead==FOR) {
        match(FOR);
        match(LPAREN);
        assignment();
        match(';');
        expression();
        match(';');
        assignment();
        match(RPAREN);
        statement();
    } else if (lookahead==IF) {
        match(IF);
        match(LPAREN);
        expression();
        match(RPAREN);
        statement();
        if (lookahead==ELSE) {
            match(ELSE);
            statement();
        }
    } else {
        assignment();
        match(';');
    }
}

void statements() {
    while (lookahead!=RBRACE) {
        statement();
    }
}

void parameter() {
    specifier();
    pointers();
    match(ID);
}

void parameter_list() {
    parameter();
    while(lookahead==',' && peek()!=ELLIPSIS) {
        match(',');
        parameter();
    }
}

void parameters() {
    if (lookahead==VOID) {
        match(VOID);
    } else {
        parameter_list();
        if (lookahead==',') {
            match(',');
            match(ELLIPSIS);
        }
    }
}

void global_declarator() {
    pointers();
    match(ID);
    if (lookahead==LPAREN) {
        match(LPAREN);
        parameters();
        match(RPAREN);
    } else if (lookahead==LBRACK) {
        match(LBRACK);
        match(INTEGER);
        match(RBRACK);
    }
}

void remainingDecls() {
    if (lookahead==';') {
        match(';');
    } else if (lookahead==',') {
        match(',');
        global_declarator();
        remainingDecls();
    } else {
        report("remaingDecls. Error at %s",yytext);
    }
}

void functionOrGlobal() {
    specifier();
    pointers();
    match(ID);
    //functionOrGlobal_prime()
    if(lookahead==LPAREN) {
        match(LPAREN);
        parameters();
        match(RPAREN);
        if (lookahead==LBRACE) {
            match(LBRACE);
            declarations();
            statements();
            match(RBRACE);
        } else {
            remainingDecls();
        }
    } else if (lookahead==LBRACK) {
        match(LBRACK);
        match(INTEGER);
        match(RBRACK);
        remainingDecls();
    } else {
        remainingDecls();
    }
}
