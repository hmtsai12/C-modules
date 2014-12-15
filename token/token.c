#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int enStrTok(char *string, char ***array, char *word)
{
	int strLen = 0;
	int argc = 0;
	int count = 0;
	char **tmp = NULL;
	
	strLen = strlen(string);

	for(count = 0; count < strLen; count++)
	{
		if(string[count] == *word)
			argc++;
	}
	
	tmp = malloc(++argc * sizeof(char *));
	/*string token*/
	{
		char *pch;
		count = 0;
		pch = strtok(string, word);
		while(pch != NULL)
		{
			tmp[count] = pch;
			count++;
			pch = strtok(NULL, word);
		}
	}
	*array = tmp;
	return count;
}

int main(int argc, char* argv[])
{
	int num = 0;
	int count = 0;
	char *delim=",";
	char **strArray = NULL;
	char test[50] = "play,1,1,";
	num = enStrTok(test, &strArray, delim);
	printf("number = %d\n", num);
	for(count = 0; count < num; count++)
		printf("array string[%d] = %s\n", count, strArray[count]);

	free(strArray);
	return 0;
}
