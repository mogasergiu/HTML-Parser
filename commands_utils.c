/* MOGA SERGIU */
#include "queue.h"

int getChildNo(char *id) {
    if (strcmp(id, "") == 0) {
        return 0;
    }

    char *idAux = (char*)calloc(strlen(id) + 1, sizeof(char));
    if (idAux == NULL) {
        return 0;
    }
    strcpy(idAux, id);

    char *it = strtok(idAux, "."), *temp;
    while(it != NULL) {
        temp = it;
        it = strtok(NULL, ".");
    }

    int childNo = atoi(temp);
    free(idAux);
    return childNo;
}

int deleteChild(TArb parent, int wantedChild) {
    TArb aux, child;
    if (wantedChild == 1) {
        aux = parent->firstChild->nextSibling;
        freeArb(parent->firstChild);
        parent->firstChild = aux;
        child = parent->firstChild;

    } else {
        int index = 1;
        child = parent->firstChild;
        while (index < wantedChild - 1) {
            child = child->nextSibling;
            index++;
        }

        aux = child->nextSibling->nextSibling;
        freeArb(child->nextSibling);
        child->nextSibling = aux;
        child = aux;
    }

    while (child != NULL) {
        child->info->id[strlen(child->info->id) - 1] = wantedChild + '\0' + 48;
        wantedChild++;
        child = child->nextSibling;
    }

    return 1;
}

int getLevel(char *id) {
    if (strcmp(id, "") == 0) {
        return 0;
    } else if (strlen(id) == 1) {
        return 1;
    }

    int level = 0;
    while (id != NULL) {
        id = strchr(id + 1, '.');
        level++;
    }

    return level;
}

void printTabs(int tabsNo, FILE *f) {
    if (f == NULL) {
        return;
    }

    int i;
    for (i = 0; i < tabsNo; i++) {
        fprintf(f, "\t");
    }
}

void formatTree(TArb arb, FILE *f) {
    if (arb == NULL || f == NULL) {
        return;
    }

    int tabsNo = getLevel(arb->info->id);
    printTabs(tabsNo, f);

    fprintf(f, "<%s", arb->info->type);

    if (arb->info->style != NULL) {
        fprintf(f, " style=\"");

        TAttr atr = arb->info->style;
        while (atr->next != NULL) {
            fprintf(f, "%s: %s; ", atr->name, atr->value);
            atr = atr->next;
        }

        fprintf(f, "%s: %s;\"", atr->name, atr->value);
    }

    if (arb->info->otherAttributes != NULL) {
        TAttr atr = arb->info->otherAttributes;
        while (atr != NULL) {
            fprintf(f, " %s=\"%s\"", atr->name, atr->value);
            atr = atr->next;
        }
    }

    if (arb->info->isSelfClosing == 1) {
        fprintf(f, "/>\n");
    } else {
        fprintf(f, ">\n");

        if (arb->info->contents != NULL) {
            printTabs(tabsNo + 1, f);
            fprintf(f, "%s\n", arb->info->contents);
        }
    }

    TArb child = arb->firstChild;
    while (child != NULL) {
        formatTree(child, f);
        child = child->nextSibling;
    }

    if (arb->info->isSelfClosing == 0) {
        printTabs(tabsNo, f);
        fprintf(f, "</%s>\n", arb->info->type);
    }
}

void addTag(TArb tree, char *string, FILE *f) {
    char *id = strtok(string, " ");
    strtok(NULL, "\"");
    char *tag = strtok(NULL, "\"");
    int childNo;

    TArb parent = getNode(tree, id + 3, &childNo);

    if (parent != NULL) {
        TArb child = parseString(id + 3, childNo + 1, tag + 1);
        addChild(parent, child);

    } else {
        fprintf(f, "Add tag failed: node with id %s not"
                " found!\n", id + 3);
    }
}

int checkCLass(TArb node, char *value) {
    if (node == NULL) {
        return 0;
    }
    TAttr atr = node->info->otherAttributes;
    while (atr != NULL && strcmp(atr->name, "class") != 0) {
        atr = atr->next;
    }

    if (atr == NULL) {
        return 0;
    }

    return (strcmp(atr->value, value) == 0);
}

int checkTag(TArb node, char *tag) {
    if (node == NULL) {
        return 0;
    }
    return (strcmp(node->info->type, tag) == 0);
}

int overrideStyle(TArb node, char *value) {
    TAttr aux;
    TInfo info = node->info;
    while (info->style != NULL) {
        aux = info->style;
        info->style = aux->next;
        freeAttr(aux);
    }

    freeAttr(info->style);
    node->info->style = NULL;

    char *auxVal = (char*)calloc(strlen(value) + 1, sizeof(char));
    if (auxVal == NULL) {
        return 0;
    }

    strcpy(auxVal, value);
    setAttr(&node->info->style, "style", auxVal);
    free(auxVal);

    return 1;
}

TAttr findStyle(TArb node, char *name) {
    TAttr atr = node->info->style;
    while (atr != NULL && strcmp(atr->name, name) != 0) {
        atr = atr->next;
    }

    return atr;
}

int appendStyle(TArb node, char *value) {
    char *auxVal = (char*)calloc(strlen(value) + 1, sizeof(char));
    if (auxVal == NULL) {
        return 0;
    }

    int mods = 0;
    strcpy(auxVal, value);

    // this usees the same technique as the setAttr function
    char *property, *propertySave, *propertyValSave;
    char *propertyVal1, *propertyVal2;
    TAttr *atr = &node->info->style, foundIt;
    for (property = strtok_r(auxVal, ";", &propertySave);
            property != NULL;
            property = strtok_r(NULL, ";", &propertySave)) {

        for (propertyVal1 = strtok_r(property, ":", &propertyValSave),
                propertyVal2 = strtok_r(NULL, ":", &propertyValSave);
                propertyVal1 != NULL && propertyVal2 != NULL;
                propertyVal1 = strtok_r(NULL, ":", &propertyValSave),
                propertyVal2 = strtok_r(NULL, ":", &propertyValSave)) {

            removeSurplusChar(propertyVal1);
            removeSurplusChar(propertyVal2);
            foundIt = findStyle(node, propertyVal1);
            if (foundIt != NULL) {
                free(foundIt->value);
                foundIt->value = (char*)calloc(strlen(propertyVal2) + 1,
                                    sizeof(char));
                if (foundIt->value == NULL) {
                    free(auxVal);
                    return 0;
                }
                strcpy(foundIt->value, propertyVal2);
                mods++;
            } else {
                atr = addAttr(atr);
                setProperty(*atr, propertyVal1, propertyVal2);
                mods++;
            }
        }
    }

    free(auxVal);
    return mods;
}

int BFSdelWithTag(TArb arb, char *tag) {
    void *que = InitQ(sizeof(TArbNodP));
    if (que == NULL) {
        return 0;
    }

    TArb children;
    PushQ(que, arb, NULL, 0);
    int deletions = 0;
    TArbP node = (TArbP)HEAD(que)->info;
    while (node != NULL) {
        if (checkTag(node->node, tag)) {
            int childNo = getChildNo(node->node->info->id);
            deletions += deleteChild(node->parent, childNo);
        } else {
            children = node->node->firstChild;
            int childNo = 1;
            while (children != NULL) {
                PushQ(que, children, node->node, 0);
                children = children->nextSibling;
                childNo++;
            }
        }

        PopQ(que);
        if (HEAD(que) == NULL) {
            break;
        }
        node = (TArbP)HEAD(que)->info;
    }

    DelQ(&que);
    return deletions;
}

int BFSdelWithClass(TArb arb, char *classy) {
    void *que = InitQ(sizeof(TArbNodP));
    if (que == NULL) {
        return 0;
    }

    TArb children;
    PushQ(que, arb, NULL, 0);
    int deletions = 0;
    TArbP node = (TArbP)HEAD(que)->info;
    while (node != NULL) {
        if (checkCLass(node->node, classy)) {
            int childNo = getChildNo(node->node->info->id);
            deletions += deleteChild(node->parent, childNo);

        } else {
            children = node->node->firstChild;
            int childNo = 1;
            while (children != NULL) {
                PushQ(que, children, node->node, 0);
                children = children->nextSibling;
                childNo++;
            }
        }

        PopQ(que);
        if (HEAD(que) == NULL) {
            break;
        }
        node = (TArbP)HEAD(que)->info;
    }

    DelQ(&que);
    return deletions;
}

int BFSdelWithTagandClass(TArb arb, char *tag, char *classy) {
    void *que = InitQ(sizeof(TArbNodP));
    if (que == NULL) {
        return 0;
    }

    TArb children;
    PushQ(que, arb, NULL, 0);
    int deletions = 0;
    TArbP node = (TArbP)HEAD(que)->info;
    while (node != NULL) {
        if (checkTag(node->node, tag) && checkCLass(node->node, classy)) {
            int childNo = getChildNo(node->node->info->id);
            deletions += deleteChild(node->parent, childNo);

        } else {
            children = node->node->firstChild;
            int childNo = 1;
            while (children != NULL) {
                PushQ(que, children, node->node, 0);
                children = children->nextSibling;
                childNo++;
            }
        }

        PopQ(que);
        if (HEAD(que) == NULL) {
            break;
        }
        node = (TArbP)HEAD(que)->info;
    }

    DelQ(&que);
    return deletions;
}

int BFSdelWithParent(TArb arb, char *pCrit, char *cCrit) {
    void *que = InitQ(sizeof(TArbNodP));
    if (que == NULL) {
        return 0;
    }

    TArb children;
    PushQ(que, arb, NULL, 0);
    int deletions = 0;
    TArbP node = (TArbP)HEAD(que)->info;
    while (node != NULL) {
        if (checkTag(node->parent, pCrit) && checkTag(node->node, cCrit)) {
            int childNo = getChildNo(node->node->info->id);
            deletions += deleteChild(node->parent, childNo);

        } else {
            children = node->node->firstChild;
            int childNo = 1;
            while (children != NULL) {
                PushQ(que, children, node->node, 0);
                children = children->nextSibling;
                childNo++;
            }
        }

        PopQ(que);
        if (HEAD(que) == NULL) {
            break;
        }
        node = (TArbP)HEAD(que)->info;
    }

    DelQ(&que);
    return deletions;
}

int BFSdelWithAncestor(TArb arb, char *pCrit, char *cCrit) {
    void *que = InitQ(sizeof(TArbNodP));
    if (que == NULL) {
        return 0;
    }

    TArb children;
    PushQ(que, arb, NULL, 0);
    int deletions = 0;
    TArbP node = (TArbP)HEAD(que)->info;
    while (node != NULL) {
        if (checkTag(node->node, cCrit) && node->valid == 1) {
            int childNo = getChildNo(node->node->info->id);
            deletions += deleteChild(node->parent, childNo);

        } else {
            int valid = 0;
            if (checkTag(node->node, pCrit) == 1 || node->valid == 1) {
                valid = 1;
            }
            children = node->node->firstChild;
            int childNo = 1;
            while (children != NULL) {
                PushQ(que, children, node->node, valid);
                children = children->nextSibling;
                childNo++;
            }
        }

        PopQ(que);
        if (HEAD(que) == NULL) {
            break;
        }
        node = (TArbP)HEAD(que)->info;
    }

    DelQ(&que);
    return deletions;
}

int BFSstyleWithTag(TArb arb, char *tag, char *value,
                    int (*styleFunc)(TArb, char*)) {
    void *que = InitQ(sizeof(TArbNodP));
    if (que == NULL) {
        return 0;
    }

    TArb children;
    PushQ(que, arb, NULL, 0);
    int mods = 0;
    TArbP node = (TArbP)HEAD(que)->info;
    while (node != NULL) {
        if (checkTag(node->node, tag)) {
            mods += styleFunc(node->node, value);
        } else {
            children = node->node->firstChild;
            int childNo = 1;
            while (children != NULL) {
                PushQ(que, children, node->node, 0);
                children = children->nextSibling;
                childNo++;
            }
        }

        PopQ(que);
        if (HEAD(que) == NULL) {
            break;
        }
        node = (TArbP)HEAD(que)->info;
    }

    DelQ(&que);
    return mods;
}

int BFSstyleWithClass(TArb arb, char *classy, char *value,
                        int (*styleFunc)(TArb, char*)) {
    void *que = InitQ(sizeof(TArbNodP));
    if (que == NULL) {
        return 0;
    }

    TArb children;
    PushQ(que, arb, NULL, 0);
    int mods = 0;
    TArbP node = (TArbP)HEAD(que)->info;
    while (node != NULL) {
        if (checkCLass(node->node, classy)) {
            mods += styleFunc(node->node, value);
        } else {
            children = node->node->firstChild;
            int childNo = 1;
            while (children != NULL) {
                PushQ(que, children, node->node, 0);
                children = children->nextSibling;
                childNo++;
            }
        }

        PopQ(que);
        if (HEAD(que) == NULL) {
            break;
        }
        node = (TArbP)HEAD(que)->info;
    }

    DelQ(&que);
    return mods;
}

int BFSstyleWithTagandClass(TArb arb, char *tag, char *classy, char *value,
                            int (*styleFunc)(TArb, char*)) {
    void *que = InitQ(sizeof(TArbNodP));
    if (que == NULL) {
        return 0;
    }

    TArb children;
    PushQ(que, arb, NULL, 0);
    int mods = 0;
    TArbP node = (TArbP)HEAD(que)->info;
    while (node != NULL) {
        if (checkTag(node->node, tag) && checkCLass(node->node, classy)) {
            mods += styleFunc(node->node, value);

        } else {
            children = node->node->firstChild;
            int childNo = 1;
            while (children != NULL) {
                PushQ(que, children, node->node, 0);
                children = children->nextSibling;
                childNo++;
            }
        }

        PopQ(que);
        if (HEAD(que) == NULL) {
            break;
        }
        node = (TArbP)HEAD(que)->info;
    }

    DelQ(&que);
    return mods;
}

int BFSstyleWithParent(TArb arb, char *pCrit, char *cCrit, char *value,
                        int (*styleFunc)(TArb, char*)) {
    void *que = InitQ(sizeof(TArbNodP));
    if (que == NULL) {
        return 0;
    }

    TArb children;
    PushQ(que, arb, NULL, 0);
    int mods = 0;
    TArbP node = (TArbP)HEAD(que)->info;
    while (node != NULL) {
        if (checkTag(node->parent, pCrit) && checkTag(node->node, cCrit)) {
            mods += styleFunc(node->node, value);

        } else {
            children = node->node->firstChild;
            int childNo = 1;
            while (children != NULL) {
                PushQ(que, children, node->node, 0);
                children = children->nextSibling;
                childNo++;
            }
        }

        PopQ(que);
        if (HEAD(que) == NULL) {
            break;
        }
        node = (TArbP)HEAD(que)->info;
    }

    DelQ(&que);
    return mods;
}

int BFSstyleWithAncestor(TArb arb, char *pCrit, char *cCrit, char *value,
                        int (*styleFunc)(TArb, char*)) {
    void *que = InitQ(sizeof(TArbNodP));
    if (que == NULL) {
        return 0;
    }

    TArb children;
    PushQ(que, arb, NULL, 0);
    int mods = 0;
    TArbP node = (TArbP)HEAD(que)->info;
    while (node != NULL) {
        if (checkTag(node->node, cCrit) && node->valid == 1) {
            mods += styleFunc(node->node, value);

        } else {
            int valid = 0;
            if (checkTag(node->node, pCrit) == 1 || node->valid == 1) {
                valid = 1;
            }
            children = node->node->firstChild;
            int childNo = 1;
            while (children != NULL) {
                PushQ(que, children, node->node, valid);
                children = children->nextSibling;
                childNo++;
            }
        }

        PopQ(que);
        if (HEAD(que) == NULL) {
            break;
        }
        node = (TArbP)HEAD(que)->info;
    }

    DelQ(&que);
    return mods;
}

void deleteRecursively(TArb tree, char *string, FILE *f, char *auxString) {
    int exists = 0;
    strtok(string, "\"");
    char *selector = strtok(NULL, "\"");
    if (selector != NULL) {
        strcpy(auxString, selector);
        if (selector[0] == '#') {
            int childNo = getChildNo(selector + 1);
            selector[strlen(selector) - 2] = '\0';
            TArb parent = getNode(tree, selector + 1, &exists);
            if (parent != NULL && exists + 1 < childNo) {
                fprintf(f, "Delete recursively failed: no node"
                        " found for selector %s!\n", auxString);
                return;
            } else {
                deleteChild(parent, childNo);
                exists = 1;
            }
        } else if (selector[0] == '.') {
            exists = BFSdelWithClass(tree, selector + 1);

        } else {
            if (strchr(selector, '.') != NULL) {
                char *tag = strtok(selector, ".");
                char *classy = strtok(NULL, ".");
                exists = BFSdelWithTagandClass(tree, tag, classy);

            } else if (strchr(selector, '>') != NULL) {
                char *parentTag = strtok(selector, ">");
                char *childTag = strtok(NULL, ">");
                exists = BFSdelWithParent(tree, parentTag, childTag);

            } else if (strchr(selector, ' ') != NULL) {
                char *ancestorTag = strtok(selector, " ");
                char *inTag = strtok(NULL, " ");
                exists = BFSdelWithAncestor(tree, ancestorTag, inTag);

            } else {
                exists = BFSdelWithTag(tree, selector);
            }
        }
        if (exists == 0) {
            fprintf(f, "Delete recursively failed: no node"
                    " found for selector %s!\n", auxString);
        }
    }
}

void HTMLCSSstylist(char *cmd, TArb tree, char *string, FILE *f,
                    char *auxString) {
    int (*styleFunc)(TArb, char*);
    // assigning the proper function depending on the command
    if (strcmp(cmd, "overrideStyle") == 0) {
        styleFunc = overrideStyle;

    } else {
        styleFunc = appendStyle;
    }

    strtok(string, "\"");
    char *selector = strtok(NULL, "\"");
    if (selector != NULL) {
        strcpy(auxString, selector);
        int mods = 0;  // retains if there were any modifications at all
        strtok(NULL, "\"");
        char *style = strtok(NULL, "\"");
        if (selector[0] == '#') {
            TArb child = getNode(tree, selector + 1, &mods);

            if (child != NULL) {
                if (strcmp(child->info->id, selector + 1) == 0) {
                    styleFunc(child, style);
                    mods = 1;
                } else {
                    mods = 0;
                }
            }

        } else if (selector[0] == '.') {
            mods = BFSstyleWithClass(tree, selector + 1, style,
                                    styleFunc);

        } else {
            if (strchr(selector, '.') != NULL) {
                char *tag = strtok(selector, ".");
                char *classy = strtok(NULL, ".");
                mods = BFSstyleWithTagandClass(tree, tag, classy,
                                    style, styleFunc);

            } else if (strchr(selector, '>') != NULL) {
                char *parentTag = strtok(selector, ">");
                char *childTag = strtok(NULL, ">");
                mods = BFSstyleWithParent(tree, parentTag,
                                    childTag, style, styleFunc);

            } else if (strchr(selector, ' ') != NULL) {
                char *ancestorTag = strtok(selector, " ");
                char *inTag = strtok(NULL, " ");
                mods = BFSstyleWithAncestor(tree, ancestorTag,
                                        inTag, style, styleFunc);

            } else {
                mods = BFSstyleWithTag(tree, selector, style,
                                        styleFunc);
            }
        }
        if (mods == 0) {
            if (styleFunc == overrideStyle) {
                fprintf(f, "Override style failed: no node"
                        " found for selector %s!\n", auxString);
            } else {
                fprintf(f, "Append to style failed: no node"
                        " found for selector %s!\n", auxString);
            }
        }
    }
}
