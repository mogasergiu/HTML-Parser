/* MOGA SERGIU 312CB */
#include "lib.h"

void removeSurplusChar(char *string) {
    char *aux = (char*)calloc(strlen(string) + 1, sizeof(char));
    if (aux == NULL) {
        return;
    }

    strcpy(aux, string);
    string[0] = '\0';

    char *it = strtok(aux, " ");
    while (it != NULL) {
        strcat(string, it);
        it = strtok(NULL, " ");
    }

    free(aux);
}

char* getNewID(char *id, int index) {
    char *newID;

    if (id == NULL) {
        newID = (char*)calloc(MAX_CHILD_ID_LEN, sizeof(char));
        if (newID == NULL) {
            return NULL;
        }
        strcpy(newID, "");

    } else {
        newID = (char*)calloc(strlen(id) + MAX_CHILD_ID_LEN, sizeof(char));
        if (newID == NULL) {
            return NULL;
        }

        if (strcmp(id, "") == 0) {
            sprintf(newID, "%d", index);

        } else {
            sprintf(newID, "%s.%d", id, index);
        }
    }

    return newID;
}

TArb arbInit(char *id, char *tag) {
    TArb newArb = (TArb)calloc(1, sizeof(TNodArb));
    if (newArb == NULL) {
        return NULL;
    }

    newArb->info = (TInfo)calloc(1, sizeof(TNodInfo));
    if (newArb->info == NULL) {
        free(newArb);
        return NULL;
    }

    newArb->info->id = (char*)calloc(strlen(id) + 1, sizeof(char));
    if (newArb->info->id == NULL) {
        free(newArb->info);
        free(newArb);
        return NULL;
    }

    strcpy(newArb->info->id, id);

    newArb->info->type = (char*)calloc(MAX_TAG_LEN, sizeof(char));
    if (newArb->info->type == NULL) {
        free(newArb->info->id);
        free(newArb->info);
        free(newArb);
        return NULL;
    }

    strcpy(newArb->info->type, tag);

    return newArb;
}

void addChild(TArb parent, TArb child) {
    if (parent == NULL || child == NULL) {
        return;
    }

    if (parent->firstChild == NULL) {
        parent->firstChild = child;

    } else {
        TArb it = parent->firstChild;
        while (it->nextSibling) {
            it = it->nextSibling;
        }

        it->nextSibling = child;
    }
}

void addContent(TInfo info, char *content) {
    if (info == NULL || content == NULL || strchr(content, '/') != NULL) {
        return;
    }

    if (info->contents == NULL) {
        info->contents = (char*)calloc(strlen(content) + 1, sizeof(char));
        if (info->contents == NULL) {
            return;
        }
    }

    // get rid of unnecessary whitespaces
    char *it = strtok(content, " ");
    while (it != NULL) {
        strcat(info->contents, it);
        it = strtok(NULL, " ");
        if (it != NULL) {
            strcat(info->contents, " ");
        }
    }
}

void setProperty(TAttr atr, char *name, char *value) {
    atr->name = (char*)calloc(strlen(name) + 1, sizeof(char));
    if (atr->name == NULL) {
        return;
    }

    strcpy(atr->name, name);

    atr->value = (char*)calloc(strlen(value) + 1, sizeof(char));
    if (atr->value == NULL) {
        free(atr->name);
        return;
    }

    strcpy(atr->value, value);
}

TAttr* addAttr(TAttr *atr) {
    if (*atr == NULL) {
        *atr = (TAttr)calloc(1, sizeof(TNodAttr));
        if (*atr == NULL) {
            return NULL;
        }

    } else {
        while ((*atr)->next) {
            atr = &(*atr)->next;
        }

        (*atr)->next = (TAttr)calloc(1, sizeof(TNodAttr));
        if ((*atr)->next == NULL) {
            return NULL;
        }

        atr = &(*atr)->next;
    }

    return atr;
}

void setAttr(TAttr *atr, char *name, char *valString) {
    if (strcmp(name, "style") == 0) {

        /*
         * in order to properly parse through the CSS values, we need to
         * save the previous string somewhere, using strtok_r
         */
        char *property, *propertySave, *propertyValSave;
        char *propertyVal1, *propertyVal2;
        // this loop obtains the fields separated by ";"
        for (property = strtok_r(valString, ";", &propertySave);
                property != NULL;
                property = strtok_r(NULL, ";", &propertySave)) {

            // this loop obtains the fields separated by ":"
            for (propertyVal1 = strtok_r(property, ":", &propertyValSave),
                    propertyVal2 = strtok_r(NULL, ":", &propertyValSave);
                    propertyVal1 != NULL && propertyVal2 != NULL;
                    propertyVal1 = strtok_r(NULL, ":", &propertyValSave),
                    propertyVal2 = strtok_r(NULL, ":", &propertyValSave)) {

                atr = addAttr(atr);

                // get rid of unnecessary whitespaces
                removeSurplusChar(propertyVal1);
                removeSurplusChar(propertyVal2);

                setProperty(*atr, propertyVal1, propertyVal2);
            }
        }

    } else {
        // no need to separate fields if it is just a normal attribute
        atr = addAttr(atr);
        setProperty(*atr, name, valString);
    }
}

TArb getNode(TArb arb, char *id, int *children) {
    if (arb == NULL) {
        return NULL;
    }

    *children = 1;
    TArb child = arb->firstChild;
    int it = 1, index;

    if (id == NULL) {
        index = 1 << 30;
    } else {
        index = 0;
        // obtain the index of the child
        while (*id != '\0' && *id != '.') {
            index = index * 10 +  (id[0] - '\0' - 48);
            id++;
        }
    }

    while (it < index && child != NULL) {
        child = child->nextSibling;
        it++;
        (*children)++;
    }

    if (index > it && id != NULL) {
        return NULL;
    }

    if (child != NULL && it == index) {
        char *nextIndex = strchr(id, '.');
        if (nextIndex != NULL) {
            return getNode(child, nextIndex + 1, children);
        } else {
            return getNode(child, NULL, children);
        }
    }

    *children = *children - 1;
    return arb;
}

TArb parseString(char *id, int childNo, char *string) {
    char *newID = getNewID(id, childNo);
    if (newID == NULL) {
        return NULL;
    }

    char *tag = strtok(string, "> ");
    TArb node = arbInit(newID, tag);
    string = tag + strlen(tag) + 1;

    char *attributes;
    if (strlen(strrchr(string, '/')) != 2) {  // if it is not self-enclosing
        node->info->isSelfClosing = 0;
        string = strtok(NULL, "<");
        attributes = strtok(string, ">");
        if (strcmp(attributes, string) == 0) {
            /*
             * if we don't have attributes, then the only thing that is left
             * is the contents (which we have just read inside "attributes",
             * because since the contents are missing, the attributes take
             * their place in the parsing)
             */
            addContent(node->info, attributes);
            free(newID);
            return node;

        } else {
            char *contents = strtok(NULL, ">");
            addContent(node->info, contents);
        }

    } else {
        node->info->isSelfClosing = 1;
        attributes = strtok(NULL, "/");
    }

    if (strchr(attributes, ' ') == NULL) {
        free(newID);
        return node;
    }

    char *saveAtr, *saveProperties, *name, *value;
    char *atr = strtok_r(attributes, " ", &saveAtr);
    while (atr != NULL) {
        name = strtok_r(atr, "=\"", &saveProperties);
        value = strtok_r(atr, "=\"", &saveProperties);

        if (strcmp(name, "style") == 0) {
            setAttr(&node->info->style, name, value);

        } else {
            setAttr(&node->info->otherAttributes, name, value);
        }
    }

    free(newID);
    return node;
}

void parseAtrEq(TArb parent, char *auxString1, char *auxString2, FILE *f) {
    fgetc(f);  // read the following '"' character
    if (fscanf(f, "%[^\"]s", auxString1) != 1) {
        return;
    }
    fgetc(f);

    if (strcmp(auxString2, "style") == 0) {
        setAttr(&parent->info->style, auxString2, auxString1);

    } else {
        setAttr(&parent->info->otherAttributes, auxString2,
                auxString1);
    }

    auxString2[0] = '\0';
}

void parseTagType(TArb parent, char *auxString1, char *auxString2, FILE *f,
                    TParseState *state, char c, int *children) {
    if (fscanf(f, "%[^> ]s", auxString1) != 1) {
        if (isalpha(c)) {
            strcpy(auxString1, "");  // making sure it's not empty

        } else {
            return;
        }
    }

    sprintf(auxString2, "%c%s", c, auxString1);
    *children = *children + 1;
    c = fgetc(f);
    if (c == '>') {
        *state = PARSE_CONTENTS;
    } else {
        *state = PARSE_REST_OF_TAG;
    }
    TArb child = parseArb(parent->info->id, *children, state, f, auxString2,
                    auxString1, auxString2);
    addChild(parent, child);
    auxString2[0] = '\0';
    *state = PARSE_CONTENTS;
}

void parseContents(TArb parent, char *auxString1, char *auxString2, FILE *f,
                    TParseState *state, char c) {
    switch (fscanf(f, "%[^<]s", auxString1)) {
        case EOF: {
            *state = PARSE_ERROR;
            return;
        }
        case 0: {
            if (!isalpha(c)) {
                return;
            }
            sprintf(auxString2, "%c", c);
            addContent(parent->info, auxString2);
            return;
        }
        default: {
            break;
        }
    }

    // first check if it is not just a newline or empty string
    char *p = strtok(auxString1, "\r\n");
    if (p != NULL) {

        if (c == '>') {
            addContent(parent->info, p);
        } else {
            sprintf(auxString2, "%c%s", c, p);
            addContent(parent->info, auxString2);
        }

        auxString2[0] = '\0';
    }

    c = fgetc(f);
    if (c == '<') {
        *state = PARSE_OPENING_BRACKET;
    } else {
        *state = PARSE_CONTENTS;
    }
}

TArb parseArb(char *id, int childNo, TParseState *state, FILE *f, char *tag,
                char *auxString1, char *auxString2) {
    char *newID = getNewID(id, childNo);
    if (newID == NULL) {
        return NULL;
    }

    TArb parent = arbInit(newID, tag);

    if (parent == NULL) {
        free(newID);
        return NULL;
    }

    int children = 0;
    char c;
    do {
        fscanf(f, "%c", &c);
        *state = Interpret(*state, c);
        switch (*state) {
            case PARSE_ATTRIBUTE_NAME: {
                    if (fscanf(f, "%[^=]s", auxString1) != 1) {
                        break;
                    }

                    sprintf(auxString2, "%c%s", c, auxString1);
                    break;

                }
            case PARSE_ATTRIBUTE_EQ: {
                parseAtrEq(parent, auxString1, auxString2, f);
                *state = PARSE_REST_OF_TAG;
                break;
            }
            case PARSE_CONTENTS: {
                parseContents(parent, auxString1, auxString2, f, state, c);
                break;
            }
            case PARSE_CLOSING_TAG: {
                if (fscanf(f, "%[^>]s", auxString1) != 1) {
                    if (childNo != -1) {
                        break;
                    } else {
                        *state = PARSE_ERROR;
                        break;
                    }
                }
                fgetc(f);
                *state = PARSE_ERROR;
                break;
            }
            case PARSE_TAG_TYPE: {
                parseTagType(parent, auxString1, auxString2, f, state, c,
                                &children);
                break;
            }
            case PARSE_SELF_CLOSING: {
                parent->info->isSelfClosing = 1;
                fgetc(f);

                if (childNo != -1) {
                    *state = PARSE_CONTENTS;
                } else {
                    *state = PARSE_ERROR;
                }
                break;
            }
            default: {
                break;
            }
        }

    } while (*state != PARSE_ERROR);

    free(newID);

    return parent;
}

void freeAttr(TAttr atr) {
    if (atr != NULL) {
        if (atr->name != NULL) {
            free(atr->name);
        }

        if (atr->value != NULL) {
            free(atr->value);
        }

        free(atr);
    }
}

void freeInfo(TInfo info) {
    if (info != NULL) {
        if (info->type != NULL) {
            free(info->type);
        }

        if(info->id != NULL) {
            free(info->id);
        }

        if (info->contents != NULL) {
            free(info->contents);
        }

        TAttr aux;
        while (info->style != NULL) {
            aux = info->style;
            info->style = aux->next;
            freeAttr(aux);
        }

        while (info->otherAttributes != NULL) {
            aux = info->otherAttributes;
            info->otherAttributes = aux->next;
            freeAttr(aux);
        }

        free(info);
    }
}

void freeArb(TArb arb) {
    if (arb != NULL) {
        TArb aux, child = arb->firstChild;
        while (child != NULL) {
            aux = child;
            child = aux->nextSibling;
            freeArb(aux);

        }

        freeInfo(arb->info);
        free(arb);
    }
}
