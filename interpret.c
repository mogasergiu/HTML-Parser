/*  */
#include "lib.h"

TParseState Interpret(TParseState currentState, char c)
{
    TParseState nextState = PARSE_ERROR;

    switch (currentState)
    {
    case PARSE_CONTENTS:
        if (c == '<')           {   nextState = PARSE_OPENING_BRACKET;  }
        else                    {   nextState = PARSE_CONTENTS; }
        break;
    case PARSE_OPENING_BRACKET:
        if (isspace(c))         {   nextState = PARSE_OPENING_BRACKET;  }
        else if (c == '>')      {   nextState = PARSE_ERROR;            }
        else if (c == '/')      {   nextState = PARSE_CLOSING_TAG;      }
        else                    {   nextState = PARSE_TAG_TYPE; }
        break;
    case PARSE_TAG_TYPE:
        if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
        else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
        else                    {   nextState = PARSE_TAG_TYPE; }
        break;
    case PARSE_CLOSING_TAG:
        if (c == '>')           {   nextState = PARSE_CONTENTS; }
        else                    {   nextState = PARSE_CLOSING_TAG;      }
        break;
    case PARSE_REST_OF_TAG:
        if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
        else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
        else if (c == '/')      {   nextState = PARSE_SELF_CLOSING;     }
        else                    {   nextState = PARSE_ATTRIBUTE_NAME;   }
        break;
    case PARSE_ATTRIBUTE_NAME:
        if (c == '=')           {   nextState = PARSE_ATTRIBUTE_EQ;   }
        else                    {   nextState = PARSE_ATTRIBUTE_NAME; }
        break;
    case PARSE_ATTRIBUTE_EQ:
        if (c == '\"')          {   nextState = PARSE_ATTRIBUTE_VALUE;  }
        break;
    case PARSE_ATTRIBUTE_VALUE:
        if (c == '\"')          {   nextState = PARSE_REST_OF_TAG; }
        else                    {   nextState = PARSE_ATTRIBUTE_VALUE; }
        break;
    case PARSE_SELF_CLOSING:
        if (c == '>')           {   nextState = PARSE_CONTENTS; }
        else                    {   nextState = PARSE_ERROR;            }
        break;
    default:
        break;
    }

    /* For debugging */
#if 0
    printf("%d", nextState);
    if (c == '\n') printf("\n");
#endif

    return nextState;
}
