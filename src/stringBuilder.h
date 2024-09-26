// New string!!!
struct Str* str_new() {
	struct Str* str = malloc(sizeof(struct Str));
	str->str = malloc(11);
	str->str[0] = 0;
	
	str->len = 0;
	str->alen = 10;
	
	return str;
}

// New string but with custom starting size!!!
struct Str* str_newSpaced(size_t space) {
	struct Str* str = malloc(sizeof(struct Str));
	str->str = malloc(space + 11);
	str->str[0] = 0;
	
	str->len = space;
	str->alen = space + 10;
	
	return str;
}

// Push a character to the string
void str_pushChar(struct Str* str, char c) { 
	if(str->len + 1 > str->alen) {
		str->str = realloc(str->str, str->alen + 11);  
		str->alen += 10;
	}
	
	str->str[str->len] = c;
	str->str[str->len + 1] = 0;
	str->len++;
} 

// Cut characters from string
void str_spliceChars(struct Str* str, size_t i, size_t len) {
	if(i + len > str->len) {
		len = str->len - i;
	} 
	
	memcpy(str->str + i, str->str + i + len, str->len);
	str->len -= len;
}

// Pop a char from the string 
void str_popChar(struct Str* str) {
	if(str->len > 0) {
		str->str[str->len] = 0;
		str->len--;
	}
}

// Reset the string's content and keep it if needed
void str_newContent(struct Str* str, char keepContentRef) {
	if(!keepContentRef) {
		free(str->str);
	}
	
	str->str = malloc(11);
	str->str[0] = 0;
	
	str->len = 0;
	str->alen = 10;
}  

// Copy a string's contents to another's
void str_copy(struct Str* str, struct Str* target) {
	str->str = realloc(str->str, target->alen + 1);
	str->alen = target->alen;
	str->len = target->len;
	
	memcpy(str->str, target->str, target->len + 1); 
}

// New string based on another string!
struct Str* str_newCopy(struct Str* str) {
	struct Str* r = malloc(sizeof(struct Str));
	
	r->alen = str->alen;
	r->len = str->len;
	r->str = malloc(r->alen + 1);
	memcpy(r->str, str->str, str->len + 1); 
	
	return r;
}

// Concatenate a string to a char[] 
void str_concat(struct Str* strA, char* strB) {
	size_t blen = strlen(strB);
	size_t offset = strA->len;

	strA->alen = strA->len + blen + 10; 
	strA->len += blen; 
	
	strA->str = realloc(strA->str, strA->alen + 1);
	memcpy(strA->str + offset, strB, blen);
	
	strA->str[strA->len] = 0;  
}

void str_free(struct Str* str, char keepContentRef) {
	if(!keepContentRef) {
		free(str->str);
	}
	free(str);
}