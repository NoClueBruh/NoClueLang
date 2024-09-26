struct Function* function_new(struct Scan* scan, struct Context* parentContext, size_t from, size_t to) { 
	if(from + 5 > to) {
		printf("\n[ SYNTAX ERROR ] is this supposed to be a function?\n");
		exit(1);
	}
	
	struct Function* func = malloc(sizeof(struct Function)); 
	// Function return type
	struct Token* typeToken = scan_getTokenAt(scan, from + 1);
	enum VarType fType = toType(typeToken->content);
	
	if(typeToken->type != KEYWORD || fType < 1) {
		printf("\n[ SYNTAX ERROR ] expected function type\n");
		exit(1);
	}
	
	// Function name
	struct Token* nameToken = scan_getTokenAt(scan, from + 2);
	if(nameToken->type != LITERAL) {
		printf("\n[ SYNTAX ERROR ] function name cannot be a keyword, symbol, number or string\n");
		exit(1);
	}
	
	func->name = nameToken->content;
	func->context = context_new(parentContext); 
	
	size_t i = from + 4;
	size_t paramSpace = scan_getNextToken(scan, from + 3, to, ")", SYMBOL);
	if(paramSpace > to) {
		printf("\n[ SYNTAX ERROR ] Expected parameters!\n");
		exit(1);
	}  
	
	// Processing parameters and their types
	func->paramCount = 0;  
	while(i < paramSpace) {
		struct Token* currentToken = scan_getTokenAt(scan, i);
		if(currentToken->type == KEYWORD) { 
			enum VarType typeNum = toType(currentToken->content);
			if(typeNum == TYPE_Void) {
				printf("\n[ COMPILE ERROR ] so, what exactly are you gonna do with a void parameter\n");
				exit(1);
			}
			if(typeNum > 0 && i + 1 < paramSpace) {
				struct Token* nameToken = scan_getTokenAt(scan, i + 1);
				if(nameToken->type != LITERAL) { 
					printf("\n[ SYNTAX ERROR ] function's parameter name cannot be a keyword, symbol, number or string\n");
					exit(1);
				}
				 
				struct Variable* var = variable_new(nameToken->content, typeNum);
				context_pushVariable(func->context, var); 
				func->paramCount++;
				
				i++;
			}
			else {
				printf("\n[ SYNTAX ERROR ] expected function parameter!\n");
				exit(1);
			}
		}
		else if(currentToken->type == SYMBOL && equal(currentToken->content, ",")) {
			// lies
		}
		else {
			printf("\n[ SYNTAX ERROR ] expected function parameter!\n");
			exit(1); 
		}
		i++;
	}    
	context_pushFunction(parentContext, func);
	context_bind(func->context, scan, fType, paramSpace + 1, to); 
	return func;
}  

struct VarValue* function_exec(struct Function* func, struct Expr** args, size_t argCount) {
	for(size_t i = 0; i < func->paramCount; i++) {
		struct Variable* arg = func->context->variables[i];
		
		if(i < argCount) { 
			variable_setValue(arg, expr_exec(args[i]));
		}
		else {
			variable_resetValue(arg);
		}
	} 
	
	struct VarValue* functionVal = context_exec(func->context);
	return functionVal;
}

void function_free(struct Function* func) { 
	context_free(func->context); 
	free(func); 
}