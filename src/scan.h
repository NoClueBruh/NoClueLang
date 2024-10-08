char isEmpty(char c) {
	return c == 32;
}

// Keywords!
char isKeyword(char* str) {
	return toType(str) > 0 || 
		equal(str, "while") || 
		equal(str, "break") || 
		equal(str, "return") || 
		equal(str, "end") || 
		equal(str, "function") ||
		equal(str, "do") ||
		equal(str, "else") ||
		equal(str, "elseif") ||
		equal(str, "if");
}

// Comparators!
char isComp(char* c){
	return equal(c, "==") || equal(c, "!=") || equal(c, ">=") || equal(c, ">") || equal(c, "<=") || equal(c, "<") || equal(c, "&&") || equal(c, "||");
}

// Symbols!
char isSymbol(char c) {
	return (c >= 33 && c <= 47) || (c >= 58 && c <= 64) || (c >= 91 && c <= 96) || (c >= 123 && c <= 126);
}

// Numbers!
char isNumber(char c) {
	return c >= 48 && c <= 57;
}

// Check if a and b should be saved as one single token
char joinChars(char a, char b) {
	return (b == '=' && (a == '>' || a == '<' || a == '!')) || (a == b && (b == '=' || b == '|' || b == '&' || b == ':'));
}

// Push a token to the scan results
void scan_tokenListPushToken(struct Scan* scan, struct Str* str, enum TokenType type, char ignoreStringReref) {
	if(scan->len + 1 > scan->alen) {
		scan->alen += 25; 
		scan->tokens = realloc(scan->tokens, sizeof(struct Token*) * scan->alen);
	}
	
	struct Token* token = malloc(sizeof(struct Token));
	token->type = type;
	token->content = str->str;
	
	if(token->type == LITERAL && isKeyword(str->str)) { 
		token->type = KEYWORD;
	}
	
	scan->tokens[scan->len] = token;
	scan->len++;
	
	if(!ignoreStringReref) {
		str_newContent(str, 1);
	}
}

// New scan
struct Scan* scan_new(char* str) {
	size_t inputSize = strlen(str);
	
	struct Str* currentMatch = str_new();
	struct Scan* scan = malloc(sizeof(struct Scan)); 
	
	scan->tokens = malloc(sizeof(struct Token*) * 25); 
	scan->len = 0;
	scan->alen = 25; 
	
	char inString = 0;  
	enum TokenType type = INT;
	size_t i = 0;
	
	while(i < inputSize) {
		char c = str[i]; 
		if(inString) {
			// If we hit the end of the string
			if(c == 39) {
				inString = 0;
				scan_tokenListPushToken(scan, currentMatch, STRING, 0);
				type = INT;
			}
			else {	
				str_pushChar(currentMatch, c);
			}			
		}
		else if(c >= 32){
			if(isSymbol(c)) {  
				// Comments
				if(c == '~') { 
					i++;
					while(i < inputSize && str[i] != '~') {
						i++;
					}
					i++;
					continue;
				}
				
				// If we hit a symbol and the current token is of type LITERAL 
				// save them separately, UNLESS that symbol is the underscore symbol which 
				// is used for variable naming and whatnot
				if(type == LITERAL && c == '_') {
					str_pushChar(currentMatch, c);
					i++;
					continue;
				}
				else if(c == '.' && type == INT) {
					// If we are currently processing a number and we hit a point,
					// the number is now a float, not an int!
					
					type = FLOAT;
					str_pushChar(currentMatch, c);	
					i++;
					
					continue;
				}
				
				// If there are any characters before this symbol, save them separately
				if(currentMatch->len > 0) {
					scan_tokenListPushToken(scan, currentMatch, type, 0);
					type = INT;
				}
			
				// If this symbol is the start of a string, go into stringMode B)
				if(c == 39){
					i++;
					inString = 1; 
					continue;
				}
				
				// Pushing the character to the token
				str_pushChar(currentMatch, c);			
				if(i + 1 < inputSize) {
					char nextChar = str[i+1];
					// Check if we should join 2 tokens together
					if(joinChars(c, nextChar)) { 
						str_pushChar(currentMatch, nextChar);	
						i++;
					}
				}
				scan_tokenListPushToken(scan, currentMatch, SYMBOL, 0);
				type = INT;
			}
			else if(isEmpty(c)) {
				// Save token if we hit a space
				if(currentMatch->len > 0) {
					scan_tokenListPushToken(scan, currentMatch, type, 0);
					type = INT;
				}
			}
			else {
				// Push the character to the token as normal
				str_pushChar(currentMatch, c);
				if(type != LITERAL && !isNumber(c)) {
					type = LITERAL;
				}
			}
		}
		else {
			// if we hit a weird token 
			// add the previous valid ones 
			// and ignore this one
			if(currentMatch->len > 0) {
				scan_tokenListPushToken(scan, currentMatch, type, 0);
				type = INT;
			}
		}
		i++;
	}		
	
	// If there are any characters left, save them as one token  
	if(currentMatch->len > 0) {
		scan_tokenListPushToken(scan, currentMatch, type, 1); 
	} 
	
	str_free(currentMatch, currentMatch->len > 0); 
	return scan;
}

struct Token* scan_getTokenAt(struct Scan* scan, size_t index) {
	return scan->tokens[index];
}

char* tokenType_toString(enum TokenType type) {
	if(type == INT) {
		return "integer";
	}
	else if(type == FLOAT) {
		return "float";
	} 
	else if(type == STRING) {
		return "string";
	}
	else if(type == SYMBOL){
		return "symbol";
	}
	else if(type == KEYWORD) {
		return "keyword";
	}
	return "literal";
}

void scan_free(struct Scan* scan) {
	for(size_t i = 0; i < scan->len; i++) {
		struct Token* token = scan->tokens[i];
		free(token->content);
		free(token);
	}
	free(scan->tokens);
	free(scan);
}
