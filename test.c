#include "lib.h"

int main() {
    /*TParseState t = 1;
    FILE *fin = fopen("_test/input/simple.html", "r");
    char c = 'h';
    int a = 1;
    char *p, *temp = (char*)calloc(100, sizeof(char));
    strcpy(temp, "l");
    printf("%s %ld\n", temp, strlen(temp));
    sprintf(temp, "%c %s", c, temp);
    strcpy(temp, "\n\rc\r\n");
    p = strtok(temp, "\n\r");
    printf("%d\n", p == NULL);
    printf("%s\n", temp);
    scanf("%[^> \"]s", temp);
    printf("%s\n", temp);
   /*while (fscanf(fin, "%c", &c) == 1) {
        t = Interpret(t, c);
        printf("%d\n", t);

	if (t == 1) {
		fscanf(fin, "%[^<]s", temp);
		if (temp[1] != '\n' && temp[0] != '\0')
		{printf("temp: %send", temp); temp[0] = '\0';}
		else {temp[0] = '\0';}

	} else if (t == 6) {
		strncat(temp, &c, 1);

	} else if (t == 7) {
		printf("style: %send", temp);
		temp[0] = '\0';
	}

   }*/
   //printf("%d\n", 1 ??!??! 0);
   //strcpy(temp, "<");
   //char *tag = strtok(temp, "<");
   //tag = strtok(NULL, ">");
   //printf("%s\n", tag);
   /*printf("%s %ld\n %s\n", tag + 4, strlen(strrchr(tag+4, '/')), temp);
   p = strtok(temp, " ");
   printf("%s", p);*/
   //c = '1';
   //printf("%d\n", atoi(&c));
   char *p, *temp = (char*)calloc(100, sizeof(char));
    strcpy(temp, "class");
   if (strstr(temp, "") != NULL) {
	   printf("DA\n");
	}
   free(temp);
   //fclose(fin);
   return 0;
}
