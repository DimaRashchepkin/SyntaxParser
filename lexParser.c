/*
Типы токенов:
- FUN = fun
- INT = int
- REAL = real
- PLUS = +
- MINUS = -
- COLON = :
- EQUAL = =
- LBRACKET = (
- RBRACKET = )
- ID = [a-zA-Z]+
*/

/*
Состояния автомата:
[\w\t\n] -> START
[a-zA-Z]+ -> ID
[:=+-()] -> DLM
& -> FIN
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MX_LEN 100

enum condition
{
    START,
    ID,
    DLM,
    FIN,
    KEY,
} cond = START;

typedef enum token
{
    FUN,
    INT,
    REAL,
    PLUS,
    MINUS,
    COLON,
    EQUAL,
    LBRACKET,
    RBRACKET,
    I,
    FN,
} token;

token lexArray[MX_LEN];
int lexArrayLen = 0;

char lexem[MX_LEN] = {0};
int lexemLen = 0;

void add(char sym)
{
    lexem[lexemLen] = sym;
    lexemLen++;
    return;
}

void clear()
{
    for (;lexemLen != 0; lexemLen--)
        lexem[lexemLen - 1] = 0;
    return;
}

int is_keyword()
{
    if (!strncmp(lexem, "fun", lexemLen) || !strncmp(lexem, "int", lexemLen) || !strncmp(lexem, "real", lexemLen))
        return 1;
    else
        return 0;
}

void putToken(enum condition cond)
{
    switch(cond)
        {
            case KEY:
                if (!strncmp(lexem, "fun", lexemLen))
                    lexArray[lexArrayLen] = FUN;
                else if (!strncmp(lexem, "int", lexemLen))
                    lexArray[lexArrayLen] = INT;
                else if (!strncmp(lexem, "real", lexemLen))
                    lexArray[lexArrayLen] = REAL;
                lexArrayLen++;
                break;
            case ID:
                lexArray[lexArrayLen] = I;
                lexArrayLen++;
                break;
            case DLM:
                if (!strncmp(lexem, ":", lexemLen))
                    lexArray[lexArrayLen] = COLON;
                else if (!strncmp(lexem, "=", lexemLen))
                    lexArray[lexArrayLen] = EQUAL;
                else if (!strncmp(lexem, "+", lexemLen))
                    lexArray[lexArrayLen] = PLUS;
                else if (!strncmp(lexem, "-", lexemLen))
                    lexArray[lexArrayLen] = MINUS;
                else if (!strncmp(lexem, "(", lexemLen))
                    lexArray[lexArrayLen] = LBRACKET;
                else if (!strncmp(lexem, ")", lexemLen))
                    lexArray[lexArrayLen] = RBRACKET;
                lexArrayLen++;
                break;
            case FIN:
                lexArray[lexArrayLen] = FN;
                lexArrayLen++;
                break;
        }
    return;
}

int lexParsing()
{
    FILE * input = fopen("input.txt", "r");

    for (char sym = getc(input); sym != EOF;)
    {
        switch(cond)
        {
            case START:
                if (sym == '\t' || sym == '\n' || sym == ' ')
                {
                    sym = getc(input);
                }
                else if ((sym >= 'a' && sym <= 'z') || (sym >= 'A' && sym <= 'Z'))
                {
                    add(sym);
                    sym = getc(input);
                    cond = ID;
                }
                else if (sym == ':' || sym == '=' || sym == '+' || sym == '-' || sym == '(' || sym == ')')
                {
                    add(sym);
                    sym = getc(input);
                    cond = DLM;
                }
                else if (sym == '&')
                {
                    cond = FIN;
                }
                else
                {
                    printf("Лексическая ошибка в символе: %c\n", sym);
                    return 1;
                }
                break;
            case ID:
                if ((sym >= 'a' && sym <= 'z') || (sym >= 'A' && sym <= 'Z'))
                {
                    add(sym);
                    sym = getc(input);
                    cond = ID;
                }
                else
                {
                    if (is_keyword())
                    {
                        putToken(KEY);
                    }
                    else
                    {
                        putToken(ID);
                    }
                    cond = START;
                    clear();
                }
                break;
            case DLM:
                putToken(DLM);
                cond = START;
                clear();
                break;
            case FIN:
                putToken(FIN);
                clear();
                sym = getc(input);
                break;
        }
    }

    fclose(input);
}

token tk;
int tmp = 0;

void nextToken()
{
    tk = lexArray[tmp];
    tmp++;
}

void parseI()
{
    nextToken();
    if (tk != I)
    {
        printf("Ожидался идентификатор, %d\n", tmp);
        exit(1);
    }
}

void parseFIN()
{
    if (tk != FN)
    {
        printf("Ожидался конец выражения, %d\n", tmp);
        exit(1);
    }
    printf("Грамматика корректна");
}

void parseT()
{
    nextToken();
    if (tk != INT && tk != REAL)
    {
        printf("Ожидался тип данных int или real, %d\n", tmp);
        exit(1);
    }
}

void parseD()
{
    if (tk != LBRACKET)
    {
        printf("Ожидался символ '(', %d\n", tmp);
        exit(1);
    }
    parseI();
    nextToken();
    if (tk != COLON)
    {
        printf("Ожидался символ ':', %d\n", tmp);
        exit(1);
    }
    parseT();
    nextToken();
    if (tk != RBRACKET)
    {
        printf("Ожидался символ ')', %d\n", tmp);
        exit(1);
    }
    nextToken();
    if (tk == LBRACKET)
    {
        parseD();
    }
}

void parseE()
{
    parseI();
    nextToken();
    while(tk == PLUS || tk == MINUS)
    {
        parseE();
        nextToken();
    }
}

void parseS()
{
    nextToken();
    if (tk != FUN)
    {
        printf("Ожидался токен FUN, %d\n", tmp);
        exit(1);
    }
    parseI();
    nextToken();
    parseD();
    if (tk != COLON)
    {
        printf("Ожидался символ ':', %d\n", tmp);
        exit(1);
    }
    parseT();
    nextToken();
    if (tk != EQUAL)
    {
        printf("Ожидался символ '=', %d\n", tmp);
        exit(1);
    }
    parseE();
    parseFIN();
}

int main(void)
{
    if (lexParsing())
        return 1;
    parseS();
    
    return 0;
}