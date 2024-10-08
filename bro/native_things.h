#define NCOLLECTIONS 5
struct NativeFunctionCollection* collections[NCOLLECTIONS];

struct NativeFunctionCollection* native_getCollection(char* name) {
	for(size_t i = 0; i < NCOLLECTIONS; i++) {
		if(equal(collections[i]->name, name)) {
			return collections[i];
		}
	}
	
	return NULL;
} 
 
// *MATH 
// good enough constants ok
void math_getPI(struct NativeFunction* thisFunc) { 
	thisFunc->result->data.fval = 3.14159265;
}

void math_getE(struct NativeFunction* thisFunc) { 
	thisFunc->result->data.fval = 2.71828182;
} 

void math_abs(struct NativeFunction* thisFunc) { 
	float v = thisFunc->params[0]->data.fval;
	thisFunc->result->data.fval = (v > 0) * v - (v < 0) * v;
} 

void math_ln(struct NativeFunction* func) {
	func->result->data.fval = log(func->params[0]->data.fval);
} 

void math_log(struct NativeFunction* func) {
	func->result->data.fval = log10(func->params[0]->data.fval);
} 

void math_isNan(struct NativeFunction* func) {
	func->result->data.ival = isnan(func->params[0]->data.fval);
}

void math_cos(struct NativeFunction* func) {
	func->result->data.fval = cos(func->params[0]->data.fval);
}

void math_sin(struct NativeFunction* func) {
	func->result->data.fval = sin(func->params[0]->data.fval);
}

void math_tan(struct NativeFunction* func) {
	func->result->data.fval = tan(func->params[0]->data.fval);
}

void math_min(struct NativeFunction* func) {
	func->result->data.fval = fmin(func->params[0]->data.fval, func->params[1]->data.fval);
}

void math_max(struct NativeFunction* func) {
	func->result->data.fval = fmax(func->params[0]->data.fval, func->params[1]->data.fval);
}

void math_round(struct NativeFunction* func) {
	func->result->data.ival = round(func->params[0]->data.fval);
}

void math_ceil(struct NativeFunction* func) {
	func->result->data.ival = ceil(func->params[0]->data.fval);
}

void math_floor(struct NativeFunction* func) {
	func->result->data.ival = floor(func->params[0]->data.fval);
}

// *STRING 
void string_equals(struct NativeFunction* thisFunc) {
	thisFunc->result->data.ival = equal(thisFunc->params[0]->data.sval->str, thisFunc->params[1]->data.sval->str);
}  

void string_len(struct NativeFunction* thisFunc) {
	thisFunc->result->data.ival = thisFunc->params[0]->data.sval->len;
}

void string_splice(struct NativeFunction* thisFunc) {
	struct Str* str = thisFunc->params[0]->data.sval;
	int index = thisFunc->params[1]->data.ival;
	int len = thisFunc->params[2]->data.ival;
	
	if(len < 0) {
		printf("\n[ String::splice ] so... how are you gonna remove %d chars?\n", len);
		return;
	}
	
	if(index < 0 || index > str->len - 1) {
		printf("\n[ String::splice ] character index out of bounds!\n");
		return;
	}
	
	str_copy(thisFunc->result->data.sval, str);
	str_spliceChars(thisFunc->result->data.sval, index, len);
}

void string_charAt(struct NativeFunction* thisFunc) {
	struct Str* str = thisFunc->params[0]->data.sval;
	int index = thisFunc->params[1]->data.ival; 
	
	if(index < 0 || index > str->len - 1) {
		printf("\n[ String::charAt ] character index out of bounds!\n");
		return;
	}
	
	str_pushChar(thisFunc->result->data.sval, str->str[index]);
}

void string_charCodeAt(struct NativeFunction* thisFunc) {
	struct Str* str = thisFunc->params[0]->data.sval;
	int index = thisFunc->params[1]->data.ival; 
	
	if(index < 0 || index > str->len - 1) {
		printf("\n[ String::charCodeAt ] character index out of bounds!\n");
		return;
	}
	
	thisFunc->result->data.ival = (int) str->str[index];
}

// *IO
void io_print(struct NativeFunction* thisFunc) {
	printf("%s", thisFunc->params[0]->data.sval->str);
}

void io_println(struct NativeFunction* thisFunc) {
	printf("%s\n", thisFunc->params[0]->data.sval->str);
}
  
char hm[300];
void io_readLine(struct NativeFunction* thisFunc) {   
	// I know this is very junky ok??
	
	memset(hm, 0, 300);
	
	scanf("%299[^\n]s", hm);  
	getchar();
	
	str_concat(thisFunc->result->data.sval, hm);
}

void io_readChar(struct NativeFunction* thisFunc) {    
	#ifdef _WIN32
		str_pushChar(thisFunc->result->data.sval, _getch());
		return;
	#endif
	
	str_pushChar(thisFunc->result->data.sval, getchar());
	getchar();
}

// *Sys
void sys_toInt(struct NativeFunction* thisFunc) {
	struct Str* input = thisFunc->params[0]->data.sval;
	thisFunc->result->data.ival = atoi(input->str);
}

void sys_toFloat(struct NativeFunction* thisFunc) {
	struct Str* input = thisFunc->params[0]->data.sval;
	thisFunc->result->data.fval = atof(input->str);
} 
// *Array   

struct NativeArrays intArrays;
struct NativeArrays floatArrays;
struct NativeArrays stringArrays;

size_t array_decodeKey(struct Str* key) {
	size_t index = 0;
	for(size_t i = 1; i < key->len; i++) {
		index += (int) key->str[i];
	}
	return index - 1;
} 

struct NativeArray* array_getFromCode(struct Str* key) {
	struct NativeArrays* arrays = NULL; 
	switch(key->str[0]) {
		case 'i':
			arrays = &intArrays;
		break;
		case 'f':
			arrays = &floatArrays;
		break;
		case 's':
			arrays = &stringArrays;
		break;
		default:  
			return NULL;
		break;
	}
	
	size_t index = array_decodeKey(key);
	if(arrays->arrayCount == 0 || index > arrays->arrayCount - 1) { 
		return NULL;
	}
	
	return &arrays->arrays[index];
}

void array_type(struct NativeFunction* func) {
	struct Str* key = func->params[0]->data.sval;
	if(key->len < 2) {
		printf("\n[ Array::array_type ] Invalid array key!\n");
		return;
	}
	
	switch(key->str[0]) {
		case 'i':
			str_concat(func->result->data.sval, "int");
		break;
		case 'f':
			str_concat(func->result->data.sval, "float");
		break;
		case 's':
			str_concat(func->result->data.sval, "string");
		break;
		default:
			printf("\n[ Array::array_type ] Invalid array key!\n");
			return; 
		break;
	}
}

void array_len(struct NativeFunction* func) {   
	struct NativeArray* array = array_getFromCode(func->params[0]->data.sval);
	if(array == NULL) {
		printf("\n[ Array::len ] That array doesn't exist!\n");
		return;
	}
	func->result->data.ival = array->count;
} 

void array_alloc(struct NativeFunction* func) {   
	struct NativeArray* array = array_getFromCode(func->params[0]->data.sval);
	if(array == NULL) {
		printf("\n[ Array::len ] That array doesn't exist!\n");
		return;
	}
	nativeArray_allocate(array, func->params[1]->data.ival);
} 

void array_remove(struct NativeFunction* func) {   
	struct Str* key = func->params[0]->data.sval;
	int i = func->params[1]->data.ival;
	
	struct NativeArray* array = array_getFromCode(key);
	if(array == NULL) {
		printf("\n[ Array::remove ] That array doesn't exist!\n");
		return;
	} 
	
	if(i < 0 || i > array->count - 1) {
		printf("\n[ Array::remove ] Array index out of bounds!\n");
		return;
	}
	
	switch (key->str[0]) {
		case 'i':
			nativeArray_removeElement(array, i, TYPE_Int); 
		break;
		case 'f':
			nativeArray_removeElement(array, i, TYPE_Float); 
		break;
		case 's':
			nativeArray_removeElement(array, i, TYPE_String); 
		break;
	}
} 

void array_splice(struct NativeFunction* func) {   
	struct Str* key = func->params[0]->data.sval;
	int i = func->params[1]->data.ival;
	int len = func->params[2]->data.ival;
	
	if(len < 1) {
		printf("\n[ Array::splice ] so uhm, how are you gonna remove %d elements?\n", len);
		return;
	} 
	
	struct NativeArray* array = array_getFromCode(key);
	if(array == NULL) {
		printf("\n[ Array::splice ] That array doesn't exist!\n");
		return;
	} 
	
	if(i < 0 || i > array->count - 1) {
		printf("\n[ Array::splice ] Array index out of bounds!\n");
		return;
	}
	
	switch (key->str[0]) {
		case 'i':
			nativeArray_removeElements(array, i, len, TYPE_Int); 
		break;
		case 'f':
			nativeArray_removeElements(array, i, len, TYPE_Float); 
		break;
		case 's':
			nativeArray_removeElements(array, i, len, TYPE_String); 
		break;
	}
} 

void array_inew(struct NativeFunction* func) {   
	nativeArrays_newArray(&intArrays);
	
	struct Str* output = func->result->data.sval;
	str_pushChar(output, 'i');
	
	size_t c = intArrays.arrayCount; 
	while(c > 0) {
		if(c > 127) {
			str_pushChar(output, (char) 127);
			c -= 127;
		}
		else {
			str_pushChar(output, (char) c);
			break;
		}
	}  
}

void array_ipush(struct NativeFunction* func) {  
	struct Str* key = func->params[0]->data.sval;
	struct VarValue* element = func->params[1]; 
	
	if(key->len < 2) {
		printf("\n[ Array::ipush ] Invalid array key!\n");
		return;
	} 
	
	if(key->str[0] != 'i') {
		printf("\n[ Array::ipush ] That is not a int array!\n");
		return;
	}
	
	size_t index = array_decodeKey(key);
	if(intArrays.arrayCount == 0 || index > intArrays.arrayCount - 1) {
		printf("\n[ Array::ipush ] That array doesn't exist!\n");
		return;
	}
	nativeArray_push(&intArrays.arrays[index], element, intArrays.arrays[index].count);
}

void array_iinsert(struct NativeFunction* func) {  
	struct Str* key = func->params[0]->data.sval;
	int i = func->params[1]->data.ival;
	struct VarValue* element = func->params[2];
	
	if(key->len < 2) {
		printf("\n[ Array::iinsert ] Invalid array key!\n");
		return;
	} 
	
	if(key->str[0] != 'i') {
		printf("\n[ Array::iinsert ] That is not a int array!\n");
		return;
	}
	
	size_t index = array_decodeKey(key);

	if(intArrays.arrayCount == 0 || index > intArrays.arrayCount - 1) {
		printf("\n[ Array::iinsert ] That array doesn't exist!\n");
		return;
	}
	
	if(i < 0 || i > intArrays.arrays[index].count) {
		printf("\n[ Array::iinsert ] Array index out of bounds!\n");
		return;
	}
	nativeArray_push(&intArrays.arrays[index], element, i);
}

void array_iget(struct NativeFunction* func) { 
	struct Str* key = func->params[0]->data.sval;
	int i = func->params[1]->data.ival;
	
	if(i < 0) { 
		printf("\n[ Array::iget ] Array index must be a positive integer!\n"); 
		return;
	}
	
	if(key->len < 2) {
		printf("\n[ Array::iget ] Invalid array key!\n"); 
		return;
	}  
		
	if(key->str[0] != 'i') {
		printf("\n[ Array::iget ] That is not a int array!\n");
		return;
	}
	
	size_t index = array_decodeKey(key); 
 
	if(intArrays.arrayCount == 0 || index > intArrays.arrayCount - 1) {
		printf("\n [ Array::iget ] That array doesn't exist!\n");  
		return;
	} 
	
	struct NativeArray arr = intArrays.arrays[index];
	if(i > arr.count - 1) {
		printf("\n[ Array::iget ] Array index out of bounds!\n"); 
		return;
	}
	
	func->result->data.ival = arr.elements[i].ival;
}

void array_iset(struct NativeFunction* func) { 
	struct Str* key = func->params[0]->data.sval;
	int i = func->params[1]->data.ival;
	int v = func->params[2]->data.ival;
	
	if(i < 0) { 
		printf("\n[ Array::iset ] Array index must be a positive integer!\n"); 
		return;
	}
	
	if(key->len < 2) {
		printf("\n[ Array::iset ] Invalid array key!\n"); 
		return;
	}  
		
	if(key->str[0] != 'i') {
		printf("\n[ Array::iset ] That is not a int array!\n");
		return;
	}
	
	size_t index = array_decodeKey(key); 
	if(intArrays.arrayCount == 0 || index > intArrays.arrayCount - 1) {
		printf("\n[ Array::iset ] That array doesn't exist!\n"); 
		return;
	}
	 
	struct NativeArray arr = intArrays.arrays[index];
	if(i > arr.count - 1) {
		printf("\n[ Array::iset ] Array index out of bounds!\n"); 
		return;
	} 
	arr.elements[i].ival = v; 
}

void array_fnew(struct NativeFunction* func) {   
	nativeArrays_newArray(&floatArrays);
	
	struct Str* output = func->result->data.sval;
	str_pushChar(output, 'f');
	
	size_t c = floatArrays.arrayCount; 
	while(c > 0) {
		if(c > 127) {
			str_pushChar(output, (char) 127);
			c -= 127;
		}
		else {
			str_pushChar(output, (char) c);
			break;
		}
	}  
}

void array_fpush(struct NativeFunction* func) {  
	struct Str* key = func->params[0]->data.sval;
	struct VarValue* element = func->params[1]; 
	
	if(key->len < 2) {
		printf("\n[ Array::fpush ] Invalid array key!\n");
		return;
	} 
	
	if(key->str[0] != 'f') {
		printf("\n[ Array::fpush ] That is not a float array!\n");
		return;
	}
	
	size_t index = array_decodeKey(key);
	if(floatArrays.arrayCount == 0 || index > floatArrays.arrayCount - 1) {
		printf("\n[ Array::fpush ] That array doesn't exist!\n");
		return;
	}  
	nativeArray_push(&floatArrays.arrays[index], element, floatArrays.arrays[index].count);
}

void array_finsert(struct NativeFunction* func) {  
	struct Str* key = func->params[0]->data.sval;
	struct VarValue* element = func->params[2];
	int i = func->params[1]->data.ival;
	
	if(key->len < 2) {
		printf("\n[ Array::finsert ] Invalid array key!\n");
		return;
	} 
	
	if(key->str[0] != 'f') {
		printf("\n[ Array::finsert ] That is not a float array!\n");
		return;
	}
	
	size_t index = array_decodeKey(key);

	if(floatArrays.arrayCount == 0 || index > floatArrays.arrayCount - 1) {
		printf("\n[ Array::finsert ] That array doesn't exist!\n");
		return;
	} 
	
	if(i < 0 || i > floatArrays.arrays[index].count) {
		printf("\n[ Array::finsert ] Array index out of bounds!\n");
		return;
	}
	nativeArray_push(&floatArrays.arrays[index], element, i);
}

void array_fget(struct NativeFunction* func) { 
	struct Str* key = func->params[0]->data.sval;
	int i = func->params[1]->data.ival;
	
	if(i < 0) { 
		printf("\n[ Array::fget ] Array index must be a positive integer!\n"); 
		return;
	}
	
	if(key->len < 2) {
		printf("\n[ Array::fget ] Invalid array key!\n"); 
		return;
	} 
	
	if(key->str[0] != 'f') {
		printf("\n[ Array::fget ] That is not a float array!\n");
		return;
	} 
	
	size_t index = array_decodeKey(key); 
	if( floatArrays.arrayCount  == 0 || index > floatArrays.arrayCount - 1) {
		printf("\n[ Array::fget ] That array doesn't exist!\n"); 
		return;
	}
	 
	struct NativeArray arr = floatArrays.arrays[index];
	if(i > arr.count - 1) {
		printf("\n[ Array::fget ] Array index out of bounds!\n"); 
		return;
	}
	
	func->result->data.fval = arr.elements[i].fval;
}

void array_fset(struct NativeFunction* func) { 
	struct Str* key = func->params[0]->data.sval;
	int i = func->params[1]->data.ival;
	float v = func->params[2]->data.fval;
	
	if(i < 0) { 
		printf("\n[ Array::fset ] Array index must be a positive integer!\n"); 
		return;
	}
	
	if(key->len < 2) {
		printf("\n[ Array::fset ] Invalid array key!\n"); 
		return;
	} 
	
	if(key->str[0] != 'f') {
		printf("\n[ Array::fset ] That is not a float array!\n");
		return;
	} 
	
	size_t index = array_decodeKey(key); 
	if( floatArrays.arrayCount == 0 || index > floatArrays.arrayCount - 1) {
		printf("\n[ Array::fset ] That array doesn't exist!\n"); 
		return;
	}
	 
	struct NativeArray arr = floatArrays.arrays[index];
	if(i > arr.count - 1) {
		printf("\n[ Array::fset ] Array index out of bounds!\n"); 
		return;
	}
	
	arr.elements[i].fval = v;
}

void array_snew(struct NativeFunction* func) {   
	nativeArrays_newArray(&stringArrays);
	
	struct Str* output = func->result->data.sval;
	str_pushChar(output, 's');
	
	size_t c = stringArrays.arrayCount; 
	while(c > 0) {
		if(c > 127) {
			str_pushChar(output, (char) 127);
			c -= 127;
		}
		else {
			str_pushChar(output, (char) c);
			break;
		}
	}  
}

void array_spush(struct NativeFunction* func) {  
	struct Str* key = func->params[0]->data.sval;
	struct VarValue* element = func->params[1]; 
	
	if(key->len < 2) {
		printf("\n[ Array::spush ] Invalid array key!\n");
		return;
	} 
	
	if(key->str[0] != 's') {
		printf("\n[ Array::spush ] That is not a string array!\n");
		return;
	}
	
	size_t index = array_decodeKey(key);

	if(stringArrays.arrayCount == 0 || index > stringArrays.arrayCount - 1) {
		printf("\n[ Array::spush ] That array doesn't exist!\n");
		return;
	} 
	nativeArray_push(&stringArrays.arrays[index], element, stringArrays.arrays[index].count);
}

void array_sinsert(struct NativeFunction* func) {  
	struct Str* key = func->params[0]->data.sval;
	struct VarValue* element = func->params[2];
	int i = func->params[1]->data.ival;
	
	if(key->len < 2) {
		printf("\n[ Array::spush ] Invalid array key!\n");
		return;
	} 
	
	if(key->str[0] != 's') {
		printf("\n[ Array::spush ] That is not a string array!\n");
		return;
	}
	
	size_t index = array_decodeKey(key);

	if(stringArrays.arrayCount == 0 || index > stringArrays.arrayCount - 1) {
		printf("\n[ Array::spush ] That array doesn't exist!\n");
		return;
	}
	  
	if(i < 0 || i > stringArrays.arrays[index].count) {
		printf("\n[ Array::spush ] Array index out of bounds!\n");
		return;
	} 
	nativeArray_push(&stringArrays.arrays[index], element, i);
}

void array_sget(struct NativeFunction* func) { 
	struct Str* key = func->params[0]->data.sval;
	int i = func->params[1]->data.ival;
	
	if(i < 0) { 
		printf("\n[ Array::sget ] Array index must be a positive integer!\n"); 
		return;
	}
	
	if(key->len < 2) {
		printf("\n[ Array::sget ] Invalid array key!\n"); 
		return;
	} 
	
	if(key->str[0] != 's') {
		printf("\n[ Array::sget ] That is not a string array!\n");
		return;
	}
	
	size_t index = array_decodeKey(key); 
	if(stringArrays.arrayCount == 0 || index > stringArrays.arrayCount - 1) {
		printf("\n[ Array::sget ] That array doesn't exist!\n"); 
		return;
	}
	 
	struct NativeArray arr = stringArrays.arrays[index];
	if(i > arr.count - 1) {
		printf("\n[ Array::sget ] Array index out of bounds!\n"); 
		return;
	}
	
	str_copy(func->result->data.sval, arr.elements[i].sval);
}

void array_sset(struct NativeFunction* func) { 
	struct Str* key = func->params[0]->data.sval;
	int i = func->params[1]->data.ival;
	struct Str* v = func->params[2]->data.sval;
	
	if(i < 0) { 
		printf("\n[ Array::sset ] Array index must be a positive integer!\n"); 
		return;
	}
	
	if(key->len < 2) {
		printf("\n[ Array::sset ] Invalid array key!\n"); 
		return;
	} 
	
	if(key->str[0] != 's') {
		printf("\n[ Array::sset ] That is not a string array!\n");
		return;
	}
	
	size_t index = array_decodeKey(key); 
	if(stringArrays.arrayCount == 0 || index > stringArrays.arrayCount - 1) {
		printf("\n[ Array::sset ] That array doesn't exist!\n"); 
		return;
	}
	 
	struct NativeArray arr = stringArrays.arrays[index];
	if(i > arr.count - 1) {
		printf("\n[ Array::sset ] Array index out of bounds!\n"); 
		return;
	}
	
	str_copy(arr.elements[i].sval, v);
}

////////////////////////////////////////////////////////////

void native_define() {     
	nativeArrays_make(&intArrays);
	nativeArrays_make(&floatArrays);
	nativeArrays_make(&stringArrays);

	struct NativeFunction* math_getPI_f = nativeFunction_new(&math_getPI, "getPI", 0, NULL, TYPE_Float);
	struct NativeFunction* math_getE_f = nativeFunction_new(&math_getE, "getE", 0, NULL, TYPE_Float);
	struct NativeFunction* math_abs_f = nativeFunction_new(&math_abs, "abs", 1, (enum VarType[1]) {TYPE_Float}, TYPE_Float);
	struct NativeFunction* math_cos_f = nativeFunction_new(&math_cos, "cos", 1, (enum VarType[1]) {TYPE_Float}, TYPE_Float);
	struct NativeFunction* math_sin_f = nativeFunction_new(&math_sin, "sin", 1, (enum VarType[1]) {TYPE_Float}, TYPE_Float);
	struct NativeFunction* math_tan_f = nativeFunction_new(&math_tan, "tan", 1, (enum VarType[1]) {TYPE_Float}, TYPE_Float);
	struct NativeFunction* math_min_f = nativeFunction_new(&math_min, "min", 2, (enum VarType[2]) {TYPE_Float, TYPE_Float}, TYPE_Float);
	struct NativeFunction* math_max_f = nativeFunction_new(&math_max, "max", 2, (enum VarType[2]) {TYPE_Float, TYPE_Float}, TYPE_Float);
	struct NativeFunction* math_floor_f = nativeFunction_new(&math_floor, "floor", 1, (enum VarType[1]) {TYPE_Float}, TYPE_Int);
	struct NativeFunction* math_ceil_f = nativeFunction_new(&math_ceil, "ceil", 1, (enum VarType[1]) {TYPE_Float}, TYPE_Int);
	struct NativeFunction* math_round_f = nativeFunction_new(&math_round, "round", 1, (enum VarType[1]) {TYPE_Float}, TYPE_Int);
	struct NativeFunction* math_log_f = nativeFunction_new(&math_log, "log", 1, (enum VarType[1]) {TYPE_Float}, TYPE_Float);
	struct NativeFunction* math_ln_f = nativeFunction_new(&math_ln, "ln", 1, (enum VarType[1]) {TYPE_Float}, TYPE_Float);
	struct NativeFunction* math_isNan_f = nativeFunction_new(&math_isNan, "isNan", 1, (enum VarType[1]) {TYPE_Float}, TYPE_Int);
	
	struct NativeFunction* string_equals_f = nativeFunction_new(&string_equals, "equals", 2, (enum VarType[2]) {TYPE_String, TYPE_String}, TYPE_Int);
	struct NativeFunction* string_len_f = nativeFunction_new(&string_len, "len", 1, (enum VarType[1]) {TYPE_String}, TYPE_Int);
	struct NativeFunction* string_splice_f = nativeFunction_new(&string_splice, "splice", 3, (enum VarType[3]) {TYPE_String, TYPE_Int, TYPE_Int}, TYPE_String);
	struct NativeFunction* string_charAt_f = nativeFunction_new(&string_charAt, "charAt", 2, (enum VarType[2]) {TYPE_String, TYPE_Int}, TYPE_String);
	struct NativeFunction* string_charCodeAt_f = nativeFunction_new(&string_charCodeAt, "charCodeAt", 2, (enum VarType[2]) {TYPE_String, TYPE_Int}, TYPE_Int);
	
	struct NativeFunction* array_inew_f = nativeFunction_new(&array_inew, "inew", 0, NULL, TYPE_String);
	struct NativeFunction* array_ipush_f = nativeFunction_new(&array_ipush, "ipush", 2, (enum VarType[2]) {TYPE_String, TYPE_Int}, TYPE_Void);
	struct NativeFunction* array_iinsert_f = nativeFunction_new(&array_iinsert, "iinsert", 3, (enum VarType[3]) {TYPE_String, TYPE_Int, TYPE_Int}, TYPE_Void);
	struct NativeFunction* array_iget_f = nativeFunction_new(&array_iget, "iget", 2, (enum VarType[2]) {TYPE_String, TYPE_Int}, TYPE_Int);
	struct NativeFunction* array_iset_f = nativeFunction_new(&array_iset, "iset", 3, (enum VarType[3]) {TYPE_String, TYPE_Int, TYPE_Int}, TYPE_Void);
	
	struct NativeFunction* array_fnew_f = nativeFunction_new(&array_fnew, "fnew", 0, NULL, TYPE_String);
	struct NativeFunction* array_fpush_f = nativeFunction_new(&array_fpush, "fpush", 2, (enum VarType[2]) {TYPE_String, TYPE_Float}, TYPE_Void);
	struct NativeFunction* array_finsert_f = nativeFunction_new(&array_finsert, "finsert", 3, (enum VarType[3]) {TYPE_String, TYPE_Int, TYPE_Float}, TYPE_Void);
	struct NativeFunction* array_fget_f = nativeFunction_new(&array_fget, "fget", 2, (enum VarType[2]) {TYPE_String, TYPE_Int}, TYPE_Float);
	struct NativeFunction* array_fset_f = nativeFunction_new(&array_fset, "fset", 3, (enum VarType[3]) {TYPE_String, TYPE_Int, TYPE_Float}, TYPE_Void);
	
	struct NativeFunction* array_snew_f = nativeFunction_new(&array_snew, "snew", 0, NULL, TYPE_String);
	struct NativeFunction* array_spush_f = nativeFunction_new(&array_spush, "spush", 2, (enum VarType[2]) {TYPE_String, TYPE_String}, TYPE_Void);
	struct NativeFunction* array_sinsert_f = nativeFunction_new(&array_sinsert, "sinsert", 3, (enum VarType[3]) {TYPE_String, TYPE_Int, TYPE_String}, TYPE_Void);
	struct NativeFunction* array_sget_f = nativeFunction_new(&array_sget, "sget", 2, (enum VarType[2]) {TYPE_String, TYPE_Int}, TYPE_String);
	struct NativeFunction* array_sset_f = nativeFunction_new(&array_sset, "sset", 3, (enum VarType[3]) {TYPE_String, TYPE_Int, TYPE_String}, TYPE_Void);
	
	struct NativeFunction* array_len_f = nativeFunction_new(&array_len, "len", 1, (enum VarType[1]) {TYPE_String}, TYPE_Int);
	struct NativeFunction* array_type_f = nativeFunction_new(&array_type, "type", 1, (enum VarType[1]) {TYPE_String}, TYPE_String);
	struct NativeFunction* array_remove_f = nativeFunction_new(&array_remove, "remove", 2, (enum VarType[2]) {TYPE_String, TYPE_Int}, TYPE_Void);
	struct NativeFunction* array_splice_f = nativeFunction_new(&array_splice, "splice", 3, (enum VarType[3]) {TYPE_String, TYPE_Int, TYPE_Int}, TYPE_Void);
	struct NativeFunction* array_alloc_f = nativeFunction_new(&array_alloc, "alloc", 2, (enum VarType[2]) {TYPE_String, TYPE_Int}, TYPE_Void);
	
	struct NativeFunction* io_print_f = nativeFunction_new(&io_print, "print", 1, (enum VarType[1]) {TYPE_String}, TYPE_Void);
	struct NativeFunction* io_println_f = nativeFunction_new(&io_println, "println", 1, (enum VarType[1]) {TYPE_String}, TYPE_Void);
	struct NativeFunction* io_readLine_f = nativeFunction_new(&io_readLine, "readLine", 0, NULL, TYPE_String);
	struct NativeFunction* io_readChar_f = nativeFunction_new(&io_readChar, "readChar", 0, NULL, TYPE_String);
	
	struct NativeFunction* sys_toInt_f = nativeFunction_new(&sys_toInt, "toInt", 1, (enum VarType[1]) {TYPE_String}, TYPE_Int);
	struct NativeFunction* sys_toFloat_f = nativeFunction_new(&sys_toFloat, "toFloat", 1, (enum VarType[1]) {TYPE_String}, TYPE_Float);
	 
	collections[0] = nfc_new("Math", (struct NativeFunction*[14]) {
		math_getE_f, 
		math_getPI_f,
		math_abs_f,
		math_sin_f,
		math_cos_f,
		math_tan_f,
		math_max_f,
		math_min_f,
		math_round_f,
		math_ceil_f,
		math_floor_f,
		math_log_f,
		math_ln_f,
		math_isNan_f
	}, 14);

	collections[1] = nfc_new("String", (struct NativeFunction*[5]) {
		string_equals_f,
		string_len_f,
		string_splice_f,
		string_charAt_f,
		string_charCodeAt_f
	}, 5); 
	
	collections[2] = nfc_new("Array", (struct NativeFunction*[20]) {
		array_inew_f,
		array_iget_f,
		array_ipush_f,
		array_iinsert_f,
		array_iset_f,
		array_fnew_f,
		array_fget_f,
		array_fpush_f,
		array_finsert_f,
		array_fset_f,
		array_snew_f,
		array_sget_f,
		array_spush_f,
		array_sinsert_f,
		array_sset_f,
		
		array_len_f,
		array_type_f,
		array_remove_f,
		array_splice_f,
		array_alloc_f
	}, 20);
	
	collections[3] = nfc_new("IO", (struct NativeFunction*[4]) {
		io_print_f,
		io_println_f,
		io_readLine_f,
		io_readChar_f
	}, 4);
	
	collections[4] = nfc_new("Sys", (struct NativeFunction*[2]) {
		sys_toInt_f,
		sys_toFloat_f
	}, 2);
} 

void native_empty() {  
	nativeArrays_free(&intArrays, 0);
	nativeArrays_free(&floatArrays, 0);
	nativeArrays_free(&stringArrays, 1);    

	nativeArrays_make(&intArrays);
	nativeArrays_make(&floatArrays);
	nativeArrays_make(&stringArrays); 
}

void native_free() {  
	for(size_t i = 0; i < NCOLLECTIONS; i++) {
		nfc_free(collections[i]);
	}   
	
	nativeArrays_free(&intArrays, 0);
	nativeArrays_free(&floatArrays, 0);
	nativeArrays_free(&stringArrays, 1);  
}
