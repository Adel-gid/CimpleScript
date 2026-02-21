#include <stdio.h>
#include <ctype.h>

enum token {
	tok_unknown,
	tok_id,
	tok_num,
	tok_lpar,
	tok_rpar
};

char tempstring[4096];

int gettok(char* str, char** endp) {
	while (*str && isspace(*str)) ++str;
	if (isdigit(*str)) {
		int i = 0;
		for (; i < 4096 && isdigit(*str) && *str; ++i, ++str) {
			tempstring[i] = *str;
		}
		tempstring[i] = '\0';
		*endp = str;
		return tok_num;
	}
	if (isalpha(*str) || *str == '_') {
		int i = 0;
		for (; i < 4096 && ( isalnum(*str) || *str == '_') && *str; ++i, ++str) {
			tempstring[i] = *str;
		}
		tempstring[i] = '\0';
		*endp = str;
		return tok_id;
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
			printf(" %s\n", tempstring);
		}
		else printf("\n");
		tok = gettok(input, &input);
	}
	return 0;
}
