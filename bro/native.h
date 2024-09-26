// not sure if the word native is accurate in this scenario but oh well
// bite me

// i know its a mess ok
struct NativeFunction* nativeFunction_new
	(
		void (*ref)(struct NativeFunction*),
		char* name,
		size_t params, 
		enum VarType* paramTypes,
		enum VarType returnType
	)
{
	struct NativeFunction* func = malloc(sizeof(struct NativeFunction));
	
	func->name = name;
	func->paramCount = params;
	func->ref = ref;  
	
	func->params = malloc(sizeof(struct VarValue*) * params);
	for(size_t i = 0; i < params; i++) {
		func->params[i] = malloc(sizeof(struct VarValue));
		func->params[i]->type = paramTypes[i];
		
		varValue_default(func->params[i]);
	}
	
	func->result = malloc(sizeof(struct VarValue));
	func->result->type = returnType;
	varValue_default(func->result);
	
	return func;
}

struct VarValue* nativeFunction_exec(struct NativeFunction* func, struct Expr** values, size_t argsPassed) {
	for(size_t i = 0; i < func->paramCount; i++) {
		struct VarValue* arg = func->params[i];
		
		if(i < argsPassed) {  
			varValue_assign(arg, expr_exec(values[i]));
		}
		else if(arg->type == TYPE_String) { 
			str_newContent(arg->data.sval, 0);
		}
		else {
			varValue_default(arg);
		} 
	} 
	 
	if(func->result->type == TYPE_String) { 
		if(func->result->data.sval->len > 0) {
			str_newContent(func->result->data.sval, 0);
		}
	}
	else {
		varValue_default(func->result);
	} 
	func->ref(func);
	
	return func->result;
}

void nativeFunction_free(struct NativeFunction* func) { 
	for(size_t i = 0; i < func->paramCount; i++) {
		varValue_free(func->params[i]);
	}
	varValue_free(func->result);
	free(func->params);
	free(func);
}

struct NativeFunctionCollection* nfc_new(char* name, struct NativeFunction** funcs, size_t funcCount){
	struct NativeFunctionCollection* ok = malloc(sizeof(struct NativeFunctionCollection));
	
	ok->name = name;
	ok->native_function_count = funcCount; 
	
	ok->native_functions = malloc(sizeof(struct NativeFunction*) * funcCount);
	for(size_t i = 0; i < funcCount; i++) { 
		ok->native_functions[i] = funcs[i];  
	}
	
	return ok;
}

struct NativeFunction* nfc_getFunction(struct NativeFunctionCollection* nfc, char* name) {
	for(size_t i = 0; i < nfc->native_function_count; i++) {
		struct NativeFunction* f = nfc->native_functions[i];
		if(equal(f->name, name)) return f;
	}
	return NULL;
}

void nfc_free(struct NativeFunctionCollection* nfc) {
	for(size_t i = 0; i < nfc->native_function_count; i++) { 
		nativeFunction_free(nfc->native_functions[i]);
	}
	free(nfc->native_functions);
	free(nfc);
}
