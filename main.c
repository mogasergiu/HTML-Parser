/*  */
#include "queue.h"

FILE** openFiles(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    // opening the HTML file we are supposed to parse
    FILE *inputHTML = fopen(argv[1], "r");
    if (inputHTML == NULL) {
        return NULL;
    }

    // opening the file from which we read the commands
    FILE *cmdIN = fopen(argv[2], "r");
    if (cmdIN == NULL) {
        fclose(inputHTML);
        return NULL;
    }

    // opening the file containing the output of commands
    FILE *cmdOUT = fopen(argv[3], "w");
    if (cmdOUT == NULL) {
        fclose(inputHTML);
        fclose(cmdIN);
        return NULL;
    }

    FILE **files = (FILE**)calloc(3, sizeof(FILE*));
    if (files == NULL) {
        fclose(inputHTML);
        fclose(cmdIN);
        fclose(cmdOUT);
        return NULL;
    }

    files[0] = inputHTML;
    files[1] = cmdIN;
    files[2] = cmdOUT;

    return files;
}

// frees the memory allocated for the file pointers
void closeFiles(FILE **files) {
    int i;
    for (i = 0; i < 3; i++) {
        fclose(files[i]);
    }

    free(files);
}

int main(int argc, char **argv) {
    // obtaining file pointers
    FILE **files = openFiles(argc, argv);
    if (files == NULL) {
        return 0;
    }

    // allocating the auxiliary strings we will use for diverse tasks
    char *string1 = (char*)calloc(MAX_LINE_LEN, sizeof(char));
    if (string1 == NULL) {
        closeFiles(files);
        return 0;
    }

    char *string2 = (char*)calloc(MAX_LINE_LEN, sizeof(char));
    if (string2 == NULL) {
        free(string1);
        closeFiles(files);
        return 0;
    }

    FILE *fin = files[0];  // assigning the HTML file first
    TParseState state = PARSE_REST_OF_TAG;  // initial state
    fscanf(fin, "%[^ ]s", string1);
    fgetc(fin);
    strcpy(string1, "html");

    TArb tree = parseArb(NULL, -1, &state, fin, string1, string1, string2);
    fin = files[1];  // assigning the commands.in
    FILE *fout = files[2];  // assigning the commands.out
    size_t maxLen = MAX_LINE_LEN;
    int testsNo;
    fscanf(fin, "%d", &testsNo);  // reading the number of tests
    fgetc(fin);
    while (testsNo > 0) {
        getline(&string1, &maxLen, fin);
        if (strcmp(string1, "format\n") == 0) {
            formatTree(tree, fout);

        } else {
            char *cmd = strtok(string1, " ");
            if (strcmp(cmd, "add") == 0) {
                char *restOfString = strtok(NULL, "\n");
                addTag(tree, restOfString, fout);

            } else if (strcmp(cmd, "deleteRecursively") == 0) {
                char *restOfString = strtok(NULL, "\n");
                deleteRecursively(tree, restOfString, fout, string2);

            } else {
                char *restOfString = strtok(NULL, "\n");
                HTMLCSSstylist(cmd, tree, restOfString, fout, string2);
            }
        }
        testsNo--;
    }

    // freeing everything and exiting program
    freeArb(tree);
    closeFiles(files);
    free(string1);
    free(string2);

    return 0;

}
