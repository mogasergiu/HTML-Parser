/*  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef LIB_H
#define LIB_H

#define MAX_TAG_LEN 100
#define MAX_LINE_LEN 1000
#define MAX_PROPERTY_LEN 50
#define MAX_CHILD_ID_LEN 5

typedef struct TNodAttr {
	char *name;
	char *value;
	struct TNodAttr *next;
} TNodAttr, *TAttr;

typedef struct TNodInfo {
	char *type;
	char *id;
	TAttr style;
	TAttr otherAttributes;
	int isSelfClosing;
	char *contents;
} TNodInfo, *TInfo;

typedef struct TNodArb {
	TInfo info;
	struct TNodArb *nextSibling;
	struct TNodArb *firstChild;
} TNodArb, *TArb;

typedef enum
{
    PARSE_ERROR             = 0,
    PARSE_CONTENTS          = 1,
    PARSE_OPENING_BRACKET   = 2,
    PARSE_TAG_TYPE          = 3,
    PARSE_CLOSING_TAG       = 4,
    PARSE_REST_OF_TAG       = 5,
    PARSE_ATTRIBUTE_NAME    = 6,
    PARSE_ATTRIBUTE_EQ      = 7,
    PARSE_ATTRIBUTE_VALUE   = 8,
    PARSE_SELF_CLOSING      = 9,
} TParseState;

// structure used to efficiently store nodes in the queue during BFS search
typedef struct arbP {
    TArb node, parent;
	int valid;
} TArbNodP, *TArbP;

// gets the child's index based on ID
int getChildNo(char *id);

// deletes the "wantedChild" indexed child of "parent"
int deleteChild(TArb parent, int wantedChild);

TParseState Interpret(TParseState currentState, char c);

// removes any unnecessary characters in string (such as whitespaces)
void removeSurplusChar(char *string);

// obtains the new ID based on parent's ID and child's "index"
char *getNewID(char *id, int index);

// initializes memory required for a node
TArb arbInit(char *id, char *tag);

// add a "child" node to the "parent" node
void addChild(TArb parent, TArb child);

/*
 * returns the node that has the requested "id" and the number of its children
 * through the "children" parameter
 */
TArb getNode(TArb arb, char *id, int *children);

// adds the input "content" to a node's "info"
void addContent(TInfo info, char *content);

// adds a CSS property to a node's "attr"
void setProperty(TAttr atr, char *name, char *value);

// adds a new TAttr node
TAttr* addAttr(TAttr *atr);

/*
 * used for "style" CSS property
 * parses through the string containing the property's values and adds them
 */
void setAttr(TAttr *atr, char *name, char *valString);

/*
 * obtains a new tree based on the string given to parse
 */
TArb parseString(char *id, int childNo, char *string);

// used by "parseArb" to properly parse and add a new attribute
void parseAtrEq(TArb parent, char *auxString1, char *auxString2, FILE *f);

// used by "parseArb" to properly parse and add a new tag type
void parseTagType(TArb parent, char *auxString1, char *auxString2, FILE *f,
                    TParseState *state, char c, int *children);

// used by "parseArb" to properly oarse abd add contents to the node
void parseContents(TArb parent, char *auxString1, char *auxString2, FILE *f,
                    TParseState *state, char c);

// used to obtain the tree coresponding to the main HTML file
TArb parseArb(char *id, int childNo, TParseState *state, FILE *f, char *tag,
				char *auxString1, char *auxString2);

void freeAttr(TAttr atr);

void freeInfo(TInfo info);

void freeArb(TArb arb);

// gets the depth of a node based on its ID
int getLevel(char *id);

void printTabs(int tabsNo, FILE *f);

// function used to properly execute the "format" command
void formatTree(TArb arb, FILE *f);

// used for the "add" command
void addTag(TArb tree, char *string, FILE *f);

// checks whether a node's class corresponds to the given class
int checkCLass(TArb node, char *value);

// checks whether a node's tag corresponds to the given tag
int checkTag(TArb node, char *tag);

// function used for the "overrideStyle" command
int overrideStyle(TArb node, char *value);

// returns the node with the corresponding "style"
TAttr findStyle(TArb node, char *name);

// function used to properly execute the "appendStyle" command
int appendStyle(TArb node, char *value);

/*
 * all these functions with the "BFS" prefix are breadth first search
 * algorithms used by the commands and functions for "deleteRecursively",
 * respectively "appendStyle" and "overrideStyle"
 * They do a BFS search in the tree to find the node corresponding to the
 * given selector
 * Due to the nature of each selector, a simple function pointer or additional
 * parameters wouldn't suffice in order to compress all these functions into
 * one (it would, but that requires an unnecessary amount of conditionals
 * and additional difficulty in understanding). Thus, the easiest to read
 * (and write) would be to have a "BFS" function for each type of selector
 */
int BFSdelWithTag(TArb arb, char *tag);

int BFSdelWithClass(TArb arb, char *classy);

int BFSdelWithTagandClass(TArb arb, char *tag, char *classy);

int BFSdelWithParent(TArb arb, char *pCrit, char *cCrit);

int BFSdelWithAncestor(TArb arb, char *pCrit, char *cCrit);

int BFSstyleWithTag(TArb arb, char *tag, char *value,
                    int (*styleFunc)(TArb, char*));

int BFSstyleWithClass(TArb arb, char *classy, char *value,
					    int (*styleFunc)(TArb, char*));

int BFSstyleWithTagandClass(TArb arb, char *tag, char *classy, char *value,
						        int (*styleFunc)(TArb, char*));

int BFSstyleWithParent(TArb arb, char *pCrit, char *cCrit, char *value,
							int (*styleFunc)(TArb, char*));

int BFSstyleWithAncestor(TArb arb, char *pCrit, char *cCrit, char *value,
							int (*styleFunc)(TArb, char*));

// function used to properly execute the command "deleteRecursively"
void deleteRecursively(TArb tree, char *string, FILE *f, char *auxString);

// function used to properly execute the commands regarding the "style"
void HTMLCSSstylist(char *cmd, TArb tree, char *string, FILE *f,
                    char *auxString);

#endif
