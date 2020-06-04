#include "lib.h"

char* getNewID(char *id, int index) {
    char *newID = (char*)calloc(strlen(id) + 4, sizeof(char));
    if (newID == NULL) {
        return NULL;
    }

    if (id == NULL) {
        strcpy(newID, "");

    } else if (strcmp(id, "") == 0) {
        sprintf(newID, "%d", index);

    } else {
        sprintf(newID, "%s.%d", id, index);
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
    if (info == NULL || content == NULL) {
        return;
    }

    if (info->contents == NULL) {
        info->contents = (char*)calloc(strlen(content) + 1, sizeof(char));
        if (info->contents == NULL) {
            return;
        }
    }

    strcpy(info->contents, content);
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

void addAttr(TAttr *atr, char *name, FILE *f) {
    char *valString = (char*)calloc(MAX_LINE_LEN, sizeof(char));
    if (valString == NULL) {
        return;
    }

    fscanf(f, "%[^\"]s", valString);

    if (*atr == NULL) {
        *atr = (TAttr)calloc(1, sizeof(TNodAttr));
        if (*atr == NULL) {
            free(valString);
            return;
        }

    } else {
        while ((*atr)->next) {
            atr = &(*atr)->next;
        }

        (*atr)->next = (TAttr)calloc(1, sizeof(TNodAttr));
        if ((*atr)->next == NULL) {
            free(valString);
            return;
        }

        atr = &(*atr)->next;
    }

    if (strcmp(name, "style") == 0) {
        char *property, *propertyVal, *propertySave, *propertyValSave;
        for (property = strtok_r(valString, ";", &propertySave);
                property != NULL;
                property = strtok_r(NULL, ";", &propertySave)) {

            if (*atr == NULL) {
                *atr = (TAttr)calloc(1, sizeof(TNodAttr));
                if (*atr == NULL) {
                    break;
                }

                atr = &(*atr)->next;
            }

            for (propertyVal = strtok_r(valString, ":", &propertyValSave);
                    propertyVal != NULL;
                    propertyVal = strtok_r(NULL, ":", &propertyValSave)) {

                setProperty(*atr, property, propertyVal);

                atr = &(*atr)->next;
            }
        }

    } else {
        strcpy((*atr)->name, name);
        strcpy((*atr)->value, valString);
    }

    free(valString);
}

TArb parseArb(char *id, int childNo, TParseState *state, FILE *f, char *tag) {
    char *newID = getNewID(id, childNo);
    if (newID == NULL) {
        return NULL;
    }

    TArb parent = arbInit(newID, tag), child;

    if (parent == NULL) {
        free(newID);
        return NULL;
    }

    char *auxString = (char*)calloc(MAX_LINE_LEN, sizeof(char));
    if (auxString == NULL) {
        free(newID);
        freeArb(parent);
        return NULL;
    }

    char *childTag = (char*)calloc(MAX_TAG_LEN, sizeof(char));
    if (childTag == NULL) {
        free(auxString);
        free(newID);
        freeArb(parent);
        return NULL;
    }

    int children = 0;
    char c;
    do {
        fscanf(f, "%c", &c);
        *state = Interpret(*state, c);
        switch (*state) {
            case PARSE_ATTRIBUTE_NAME:
                strncat(auxString, &c, 1);
                //fscanf(f, "%[^=]s", auxString);
                break;

            case PARSE_ATTRIBUTE_EQ:
                fscanf(f, "%c", &c);  // read the following '"' character

                if (strcmp(auxString, "style") == 0) {
                    addAttr(&parent->info->style, auxString, f);

                } else {
                    addAttr(&parent->info->otherAttributes, auxString, f);
                }

                auxString[0] = '\0';
                break;

            case PARSE_CONTENTS:
                fscanf(f, "%[^<]s", auxString);

                // first check if it is not just a newline or empty string
                if (auxString[1] != '\n' && auxString[0] != '\0') {
                    addContent(parent->info, auxString);
                }

                auxString[0] = '\0';
                break;

            case PARSE_CLOSING_TAG:
                fscanf(f, "%[^>]s", auxString);
                auxString[0] =  '\0';
                *state = PARSE_ERROR;
                break;

            case PARSE_TAG_TYPE:
                fscanf(f, "%[^> ]s", auxString);
                strncat(childTag, &c, 1);
                strcat(childTag, auxString);
                children++;
                child = parseArb(newID, children, state, f, auxString);
                addChild(parent, child);
                childTag[0] = '\0';
                break;

            case PARSE_SELF_CLOSING:
                parent->info->isSelfClosing = 1;
                break;

            default:
                break;
        }

    } while (*state != PARSE_ERROR);

    free(newID);
    free(auxString);
    free(childTag);

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
            info->style = info->style->next;
            free(aux);
        }

        while (info->otherAttributes != NULL) {
            aux = info->otherAttributes;
            info->otherAttributes = info->otherAttributes->next;
            free(aux);
        }
    }
}

void freeArb(TArb arb) {
    if (arb != NULL) {
        TArb aux;
        while (arb->firstChild) {
            aux = arb->firstChild;
            arb->firstChild = arb->firstChild->nextSibling;
            freeArb(aux);
        }

        freeInfo(arb->info);
        free(arb);
    }
}
