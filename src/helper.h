void token_trace(struct Scan* scan, size_t from, size_t to, size_t offset) {
	if(offset > 49) {
		offset = 49;
	}
	
	char o[51];
	for(size_t i = 0; i < offset; i++) {
		o[i] = ' ';
	}
	o[offset+1] = 0;
	
	for(size_t i = from; i < to; i++) {
		struct Token* token = scan_getTokenAt(scan, i);
		printf("%s[ TOKEN ] { content: '%s', type: %s }\n", o, token->content, tokenType_toString(token->type));
	}
}

// Basically, everything that has an 'end' to it is here
// Not else and elseif because they share the if's end!
char scan_hasBody(char* str) {
	return equal(str, "function") || equal(str, "if") || equal(str, "while");
} 

char varValue_assign(struct VarValue* a, struct VarValue* b) {
	if(a->type == b->type) { 
		switch(a->type) {
			case TYPE_Float:
				a->data.fval = b->data.fval;
			break;
			case TYPE_Int:
				a->data.ival = b->data.ival;
			break;
			case TYPE_String:  
				str_copy(a->data.sval, b->data.sval);
			break;
		}
		
		return 0;
	}
	
	return 1;
}

void varValue_set(struct VarValue* val, void* value) {
	switch(val->type) {
		case TYPE_Int: 
			// lies!!!
			val->data.ival = (int) *(float*)value;
		break;
		case TYPE_Float: 
			val->data.fval = *(float*)value;
		break;
		case TYPE_String: 
			str_copy(val->data.sval, value);
		break;
	}
}

void varValue_default(struct VarValue* val) {
	switch(val->type) {
		case TYPE_Int:
			val->data.ival = 0;
		break;
		case TYPE_Float:
			val->data.fval = 0;
		break;
		case TYPE_String:  
			val->data.sval = str_new();  
		break;
	}
}

void varValue_free(struct VarValue* val) {
	if(val->type == TYPE_String) {
		str_free(val->data.sval, 0);
	}	
	free(val);
}

// Returns 2 if we go up a layer, 1 if we don't move a layer and 0 if we go down a layer 
char scan_shiftLayer(struct Scan* scan, size_t i) {
	struct Token* token = scan_getTokenAt(scan, i);
	if(token->type == SYMBOL) { 
		char* str = token->content;
		
		if(equal(str, "(") || equal(str, "{") || equal(str, "[")) return 2;
		else if(equal(str, ")") || equal(str, "}") || equal(str, "]")) return 0; 
	}
	else if (token->type == KEYWORD) {
		if(scan_hasBody(token->content)) {
			return 2;
		}
		else if(equal(token->content, "end")) { 
			return 0;
		}
	}
	
	return 1;
}

// Get the position of the nearest available 'end' token
size_t scan_getEnd(struct Scan* scan, size_t from, size_t to) {
	size_t i = from + 1;
	size_t layer = 0;
	
	while(i < to) {
		struct Token* token = scan_getTokenAt(scan, i);
		if (token->type == KEYWORD) {
			if(scan_hasBody(token->content)) {
				layer++; 
			}
			else if(equal(token->content, "end")) {
				if(layer == 0) { 
					return i;
				}
				layer--;  
			}
		}
		i++;
	} 
	return to + 1;
} 

// Simple out of bounds check that throws an error as well
void scan_checkOutOfBounds(size_t i, size_t lim, char* error) {
	if(i > lim) {
		printf("\n[ SYNTAX ERROR ] %s\n", error);
		exit(1);
	}
}

size_t scan_getNextToken(struct Scan* scan, size_t from, size_t to, char* content, enum TokenType type) {
	size_t i = from + 1;
	size_t layer = 0;
	 
	while(i < to) { 
		struct Token* token = scan_getTokenAt(scan, i);
		if(layer == 0 && token->type == type && equal(token->content, content)) {
			return i;
		}
	
		char shift = scan_shiftLayer(scan, i);  
		if(!shift && !layer) {
			printf("\n[ SYNTAX ERROR ] Unexpected token '%s'!\n", token->content);
			exit(1);
		} 
		layer += shift - 1; 
		
		i++;
	} 
	return i + 1;
}

size_t scan_getNextSemicolon(struct Scan* scan, size_t from, size_t to) {
	return scan_getNextToken(scan, from, to, ";", SYMBOL);
}