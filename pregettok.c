#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

enum token {
	tok_unknown,
	tok_id,
	tok_num,
	tok_lpar,
	tok_rpar
};

char* tempString = NULL;

int gettok(char* str, char** endp) {
	while (*str && isspace(*str)) ++str;
	if (isdigit(*str)) {
		int n = 0;
		for (; isdigit(*str) && *str; ++n, ++str);
		tempString = malloc(n+1);
		memcpy(tempString, str, n);
		tempString[n] = '\0';
		*endp = str;
		return tok_num;
	}
	if (isalpha(*str) || *str == '_') {
		int n = 0;
		for (; (isalnum(*str) || *str == '_') && *str; ++n, ++str);
		tempString = malloc(n+1);
		memcpy(tempString, str, n);
		tempString[n] = '\0';
		*endp = str;
		return tok_num;
	}
	if (*str == '(') {
		++str;
		*endp = str;
		return tok_lpar;
	}
	if (*str == ')') {
		++str;
		*endp = str;
		return tok_rpar;
	}
	return tok_unknown;
}


int main(int argc, char *argv[])
{
	char* input = "22 id ()";
	int tok = gettok(input, &input);
	while (tok) {
		printf("tok %d", tok);
		if (tok == tok_num || tok == tok_id) {
			printf(" %s\n", tempString);
			free(tempString);
		}
		else printf("\n");
		tok = gettok(input, &input);
	}
	return 0;
}
