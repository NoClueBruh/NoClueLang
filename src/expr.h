// Check if that token is an operation basically
char needs_expr_split(struct Token* t) {
	if(t->type != SYMBOL) {
		return 0;
	} 
	char* c = t->content;
	return equal(c, "+") || equal(c, "%") || equal(c, "-") || equal(c, "*") || equal(c, "/")  || equal(c, "^") || isComp(c);
}

// Check if we need to cast an expression from Int, to Float
char fromInt_toFloat(struct Token* currentToken) {
	return 	equal(currentToken->content, "/") || 
			equal(currentToken->content, "^") || 
			equal(currentToken->content, "%");
}

// Returns the weight of an operation
size_t operationWeight(char* str) {
	if(equal(str, "^")) return 10;
	else if(equal(str, "%") || equal(str, "*") || equal(str, "/")) return 9;   
	else if(equal(str, "+")) return 8;
	else if(equal(str, "==") || equal(str, "!=") || equal(str, ">=") || equal(str, "<=") || equal(str, "<") || equal(str, ">")) return 7;
	
	return 0;
}

// Push an expression member to the INITEXPR
void initexpr_pushMember(struct InitExpr* initexpr, struct ExprMember* member) {
	if(initexpr->memberCount + 1 > initexpr->memberSpace) {
		initexpr->memberSpace += 10;
		initexpr->members = realloc(initexpr->members, sizeof(struct ExprMember*) * initexpr->memberSpace); 
	}
	
	initexpr->members[initexpr->memberCount] = member;
	initexpr->memberCount++;
}

// Push an operation to the INITEXPR
void initexpr_pushOperation(struct InitExpr* initexpr, char* operation) {
	if(initexpr->operationCount + 1 > initexpr->operationSpace) {
		initexpr->operationSpace += 10;
		initexpr->operations = realloc(initexpr->operations, sizeof(char*) * initexpr->operationSpace);  
	}
	
	initexpr->operations[initexpr->operationCount] = operation;
	initexpr->operationCount++; 
}

// New Expression member!
struct ExprMember* exprMember_new(struct Scan* scan, struct Context* context, size_t from, size_t to, char negative) {
	struct ExprMember* member = malloc(sizeof(struct ExprMember));
	// useful for arithmetic
	member->t = NULL;
	member->negative = negative;
	
	if(to - from == 1) { 
		// Processing a single-token member
		struct Token* token = scan_getTokenAt(scan, from);
	
		if(token->type == INT) {
			// Its just an integer
			member->value.inum = atoi(token->content);
			member->type = INT_NUM; 
		}
		else if(token->type == FLOAT) {
			// Its just a float
			member->value.fnum = atof(token->content);
			member->type = FLOAT_NUM; 
		}
		else if(token->type == STRING) {
			// Its just a string
			member->value.p = token->content;
			member->type = STRING_CONST; 
		}
		else if(token->type == LITERAL) { 
			// The last thing it could be is variable 
			struct Variable* variable = context_getVariableByName(context, token->content);
			if(variable == NULL) {
				// 
				printf("\n[ COMPILE ERROR ] Variable '%s' doesn't exist in this scope\n", token->content);
				exit(1);
			}
			member->value.p = variable;
			member->type = REF; 
		}
		else {
			printf("\n[ SYNTAX ERROR ] Invalid expression item %s\n", token->content);
			exit(1);
		}
	}
	else {  
		struct Token* startToken = scan_getTokenAt(scan, from); 
		struct Token* secondToken = scan_getTokenAt(scan, from + 1);
		
		// Checking if we are looking inside a collection
		if(secondToken->type == SYMBOL && equal(secondToken->content, "::")) { 
			// These symbols are only used when accessing a collection
			struct NativeFunctionCollection* collection = native_getCollection(startToken->content);
			// Check if that collection exists
			if(collection == NULL) {
				printf("\n[ COMPILE ERROR ] there doesn't exist a native collection by the name '%s'!\n", startToken->content);
				exit(1);
			}
			
			struct Token* thirdToken = scan_getTokenAt(scan, from + 2);
			// Function's name
			if(thirdToken->type != LITERAL) { 
				printf("\n[ SYNTAX ERROR ] function name is a literal bro\n");
				exit(1);
			}
			
			struct NativeFunction* func = nfc_getFunction(collection, thirdToken->content);
			// Checking if the function exists
			if(func == NULL) { 
				printf("\n[ COMPILE ERROR ] there doesn't exist a function by the name '%s' defined in the '%s' collection\n", thirdToken->content, startToken->content);
				exit(1);
			}
			 
			struct Token* nextToken = scan_getTokenAt(scan, from + 3);
			struct Token* endToken = scan_getTokenAt(scan, to - 1);
			
			// Double checking if its a function based on the last and the third token
			if(	endToken->type == SYMBOL && equal(endToken->content, ")") &&
				nextToken->type == SYMBOL && equal(nextToken->content, "(")	) { 
				
				// I know this is copy - paste from the normal functionCall
				// but it has some changes
				
				struct NativeFunctionCall* callInst = malloc(sizeof(struct NativeFunctionCall));
				callInst->expr = NULL;
				callInst->params = 0;
				
				// Processing arguments!
				if(func->paramCount > 0) {
					size_t pushedValues = 0;
					
					struct Expr** args = malloc(sizeof(struct Expr*) * func->paramCount);
					
					size_t j = from + 4;
					size_t i = scan_getNextToken(scan, from + 3, to - 1, ",", SYMBOL);
					
					while(i < to - 1) {
						if(pushedValues >= func->paramCount) {
							printf("\n[ COMPILE ERROR ] way too many arguments to call function %s!\n", func->name);
							exit(1);
						}
						
						struct Expr* expr = expr_new(scan, context, j, i);
						enum VarType paramType = func->params[pushedValues]->type;
						if(paramType != TYPE_UNKNOWN && expr->result->type != paramType) {
							printf("\n[ COMPILE ERROR ] wrong argument type for argument #%d of function!\n", pushedValues, func->name);
							exit(1);
						}
						args[pushedValues++] = expr;
						
						j = i + 1;
						
						i = scan_getNextToken(scan, i, to - 1, ",", SYMBOL);
					}
					i = to - 1;
					
					if(j != i) {
						if(pushedValues >= func->paramCount) {
							printf("\n[ COMPILE ERROR ] way too many arguments to call function %s!\n", func->name);
							exit(1);
						}
						
						struct Expr* expr = expr_new(scan, context, j, i); 
						enum VarType paramType = func->params[pushedValues]->type;
						if(paramType != TYPE_UNKNOWN && expr->result->type != paramType) {
							printf("\n[ COMPILE ERROR ] wrong argument type for argument #%d of function %s!\n", pushedValues, func->name);
							exit(1);
						} 
						args[pushedValues++] = expr;
					}
						
					callInst->expr = args;
					callInst->params = pushedValues;
				}
				else if(from + 3 != to - 2) { 
					// If there aren't any parameters, there shouldn't be any tokens between the '(' and ')' tokens!
					printf("\n[ SYNTAX ERROR ] Function %s doesn't take in any parameters!\n", func->name);
					exit(1);
				}
				
				callInst->func = func;
				
				member->value.p = callInst;
				member->type = NATIVE_FUNCTION_CALL;
			}
			else {
				printf("\n[ SYNTAX ERROR ] are you gonna call this function or what\n", startToken->content);
				exit(1);
			}
		}
		else if(startToken->type == LITERAL) {
			// Probably a function
			struct Function* func = context_getFunctionByName(context, startToken->content);
			if(func != NULL) { 
				// Yep its a function alright
				struct Token* nextToken = scan_getTokenAt(scan, from + 1);
				struct Token* endToken = scan_getTokenAt(scan, to - 1);
				
				// Double checking if its a function
				if(	endToken->type == SYMBOL && equal(endToken->content, ")") &&
					nextToken->type == SYMBOL && equal(nextToken->content, "(")	) { 
					
					struct FunctionCall* callInst = malloc(sizeof(struct FunctionCall));
					callInst->expr = NULL;
					callInst->params = 0;
					
					// Processing the parameters!!
					if(func->paramCount > 0) {
						size_t pushedValues = 0;
						
						struct Expr** args = malloc(sizeof(struct Expr*) * func->paramCount);
						
						size_t j = from + 2;
						size_t i = scan_getNextToken(scan, from + 1, to - 1, ",", SYMBOL);
						
						while(i < to - 1) {
							if(pushedValues >= func->paramCount) {
								printf("\n[ COMPILE ERROR ] way too many arguments to call function %s!\n", func->name);
								exit(1);
							}
							
							struct Expr* expr = expr_new(scan, context, j, i);
							enum VarType paramType = func->context->variables[pushedValues]->value->type;
							if(paramType != TYPE_UNKNOWN && expr->result->type != paramType) {
								printf("\n[ COMPILE ERROR ] wrong argument type for argument #%d of function %s!\n", pushedValues, func->name);
								exit(1);
							}
							args[pushedValues++] = expr;
							
							j = i + 1;
							
							i = scan_getNextToken(scan, i, to - 1, ",", SYMBOL);
						}
						i = to - 1;
						
						if(pushedValues >= func->paramCount) {
							printf("\n[ COMPILE ERROR ] way too many arguments to call function %s!\n", func->name);
							exit(1);
						}
						
						if(j != i) {
							struct Expr* expr = expr_new(scan, context, j, i); 
							enum VarType paramType = func->context->variables[pushedValues]->value->type;
							if(paramType != TYPE_UNKNOWN && expr->result->type != paramType) {
								printf("\n[ COMPILE ERROR ] wrong argument type for argument #%d of function %s!\n", pushedValues, func->name);
								exit(1);
							}
							args[pushedValues++] = expr;
						}
						
						callInst->expr = args;
						callInst->params = pushedValues;
					}
					else if(from + 1 != to - 2) {
						// If there aren't any parameters, there shouldn't be any tokens between the '(' and ')' tokens
						printf("\n[ SYNTAX ERROR ] Function %s doesn't take in any parameters!\n", func->name);
						exit(1);
					}
					callInst->func = func;
					
					member->value.p = callInst;
					member->type = FUNCTION_CALL;
				}
				else { 
					printf("\n[ SYNTAX ERROR ] are you gonna call this function or what\n", startToken->content);
					exit(1);
				}
			}
			else {
				printf("\n[ COMPILE ERROR ] function '%s' doesn't exist in this scope\n", startToken->content);
				exit(1);
			} 
		}
		else if(startToken->type == SYMBOL && equal(startToken->content, "(")) {
			// Just some parentheses 
			
			struct Token* endToken = scan_getTokenAt(scan, to - 1);
			if(endToken->type == SYMBOL && equal(endToken->content, ")")) {  
				member->value.p = expr_new(scan, context, from + 1, to - 1);
				member->type = EXPRESSION; 
			}
			else {
				printf("\n[ SYNTAX ERROR ] hey bro you forgot to close them parentheses\n");
				exit(1);
			}
		}
		else {
			printf("\n[ SYNTAX ERROR ] Unexpected token '%s'!\n", startToken->content);
			exit(1);
		}
	}
	
	return member;
} 

void exprMember_free(struct ExprMember* member) { 
	if(member->type == EXPRESSION) { 
		expr_free(member->value.p);  
	} 
	else if(member->type == FUNCTION_CALL) {
		struct FunctionCall* call = member->value.p;
		for(size_t i = 0; i < call->params; i++) {
			if(call->expr[i] != NULL) {
				expr_free(call->expr[i]);
			}
		}
		if(call->expr != NULL) {
			free(call->expr);
		}
		free(call);
	}
	else if(member->type == NATIVE_FUNCTION_CALL) {
		// same thing, i know
		struct NativeFunctionCall* call = member->value.p;
		for(size_t i = 0; i < call->params; i++) {
			if(call->expr[i] != NULL) {
				expr_free(call->expr[i]);
			}
		}
		if(call->expr != NULL) {
			free(call->expr);
		}
		free(call);
	}
	if(member->t != NULL) {
		free(member->t);
	}  
	
	free(member);
}

// Returns if the type changed from something to string 
// Because we need to check the operations 
char expr_typeCheck(struct VarValue* value, struct ExprMember* member) {
	enum VarType otherType = TYPE_Void;
	if(member->type == FLOAT_NUM) {
		otherType = TYPE_Float;
	}
	else if(member->type == INT_NUM) {
		otherType = TYPE_Int;
	}
	else if(member->type == STRING_CONST) {
		otherType = TYPE_String;
	}
	else if(member->type == EXPRESSION) {
		otherType = ((struct Expr*) member->value.p)->result->type;
	}
	else if(member->type == REF) {
		otherType = ((struct Variable*)member->value.p)->value->type;
	}
	else if(member->type == FUNCTION_CALL) {
		//                                     omg wtf is this
		otherType = ((struct FunctionCall*)member->value.p)->func->context->result->type;
	}
	else if(member->type == NATIVE_FUNCTION_CALL) { 
		otherType = ((struct NativeFunctionCall*)member->value.p)->func->result->type;
	}
	
	if(otherType == TYPE_String && value->type != TYPE_String) {
		value->type = TYPE_String;
		return 1;
	}
	
	if(value->type == TYPE_Void || otherType == TYPE_String) {
		value->type = otherType;
	} 
	else if(otherType == TYPE_Float && value->type == TYPE_Int) {
		value->type = TYPE_Float;
	} 
	
	return 0;
}

struct Expr* expr_new(struct Scan* scan, struct Context* context, size_t start, size_t end) {
	struct InitExpr* initexpr = malloc(sizeof(struct InitExpr));
	
	// init
	initexpr->memberCount = 0;
	initexpr->memberSpace = 10;
	initexpr->members = malloc(sizeof(struct ExprMember*) * initexpr->memberSpace);
	
	initexpr->operationCount = 0;
	initexpr->operationSpace = 10;
	initexpr->operations = malloc(sizeof(char*) * initexpr->operationSpace); 
	//
	 
	struct VarValue* exprResult = malloc(sizeof(struct VarValue)); 
	exprResult->type = TYPE_Void;
	
	if(start != end) {
		struct Token* lead = scan_getTokenAt(scan, start);
		
		enum VarType t = toType(lead->content);
		if(lead->type == KEYWORD && t > 0) {
			if(t == TYPE_Void) {
				printf("\n[ COMPILE ERROR ] Cannot cast an expression to a void, why would you even do that\n");
				exit(1);
			}
			
			// casting an expression
			exprResult->type = t;
			start++;
		}
	}
	
	//printf("\n[ EXPR ] start %d, end %d\n", start, end);
	//token_trace(scan, start, end, 2);
	
	// The start of each token is here
	size_t j = start;
	// The negative flag 
	char neg = 0; 
	// The comparator flag
	// if true, the return type of this expression is forced to be an int!
	char comp = 0;
	 
	size_t currentLayer = 0;
	for(size_t i = start; i < end; i++) { 
		struct Token* currentToken = scan_getTokenAt(scan, i);
		// Checking if we change layers!
		char layerShift = scan_shiftLayer(scan, i); 
		if(!layerShift && !currentLayer) {
			printf("\n[ SYNTAX ERROR ] Unexpected token '%s'!\n", currentToken->content);
			exit(1);
		}
		currentLayer += layerShift - 1;
		
		// If we are on the correct layer and we hit any operator symbol
		if(currentLayer == 0 && needs_expr_split(currentToken)) {   
			// If we are concatenating strings, only the '+' symbol is valid
			if(exprResult->type == TYPE_String && !equal(currentToken->content, "+")) {
				printf("\n[ SYNTAX ERROR ] you can only concatenate strings using the + symbol, i wasn't paid to make this language ok?\n");
				exit(1);
			}
			else if(isComp(currentToken->content)) { 
				comp = 1;
			}
			else if((exprResult->type == TYPE_Int || exprResult->type == TYPE_Void) && fromInt_toFloat(currentToken)) {
				// Casting Expression to Float
				exprResult->type = TYPE_Float;
			}
			
			if(j == i) {
				// Check if we just need to flip the negative flag
				if(equal(currentToken->content, "-")) {
					neg = !neg; 
				}
				else {
					// If its not the '-' symbol, then what the balls is it
					printf("\n[ SYNTAX ERROR ] Unexpected Token '%s'!\n", currentToken->content);
					exit(1);
				}
			}
			else { 
				// Preparing the expression member
				struct ExprMember* member = exprMember_new(scan, context, j, i, neg); 
				
				// Check if the operator is the '-' symbol
				// If so, we flip the negative flag
				// Thats because there isn't such thing as subtraction here!
				// There exists addition with negative numbers though!
				
				if(equal(currentToken->content, "-")) {
					neg = 1;
					initexpr_pushOperation(initexpr, "+"); 
				}
				else {
					neg = 0;
					initexpr_pushOperation(initexpr, currentToken->content);
				}
				
				// If that expression member made this expression of type String, 
				// we need to recheck all other symbols because the only valid one now is the '+' symbol
				if(expr_typeCheck(exprResult, member)) { 
					for(size_t n = 0; n < initexpr->operationCount; n++) {
						if(!equal(initexpr->operations[n], "+")) {
							printf("\n[ SYNTAX ERROR ] you can only concatenate strings using the + symbol, i wasn't paid to make this language ok?\n");
							exit(1);
						} 
					}
				}
				initexpr_pushMember(initexpr, member);
			}
			j = i + 1;
		} 
	}
	
	// Pushing the last member
	struct ExprMember* member = exprMember_new(scan, context, j, end, neg); 
	
	// If that expression member made this expression of type String, 
	// we need to recheck all other symbols because the only valid one now is the '+' symbol
	if(expr_typeCheck(exprResult, member)) { 
		for(size_t p = 0; p < initexpr->operationCount; p++) {
			if(!equal(initexpr->operations[p], "+")) {
				printf("\n[ SYNTAX ERROR ] you can only concatenate strings using the + symbol, i wasn't paid to make this language ok!?\n");
				exit(1);
			} 
		}
	}	
	initexpr_pushMember(initexpr, member);   
	
	if(comp) {
		exprResult->type = TYPE_Int;
	}
 
	varValue_default(exprResult);	
	
	// InitExpr -> Expr 
	// Saving some bytes I guess 
	
	struct Expr* expr = malloc(sizeof(struct Expr));
	expr->result = exprResult;
	expr->memberCount = initexpr->memberCount;
	expr->members = malloc(sizeof(struct ExprMember*) * initexpr->memberCount);
	
	expr->operations = NULL;
	expr->order = NULL;
	expr->p = NULL;
	
	// If there is just one... no need for most of the things here lmao
	if(initexpr->memberCount == 1) {
		expr->members[0] = initexpr->members[0]; 
		if(exprResult->type != TYPE_String) {
			expr->members[0]->t = malloc(sizeof(float));
		}
	} 
	else if(exprResult->type == TYPE_String) {  
		// If the expression is of type String, there isn't any order and we don't need the operations array
		// we know that we can only concatenate the strings with the '+' operator and we go left to right!
		for(size_t k = 0; k < expr->memberCount; k++) {
			expr->members[k] = initexpr->members[k]; 
		}
	}
	else {  
		expr->operations = malloc(sizeof(char*) * (initexpr->memberCount - 1)); 
		expr->order = malloc(sizeof(size_t*) * (initexpr->memberCount - 1)); 
		expr->p = malloc(sizeof(float*) * initexpr->memberCount);
		
		for(size_t k = 0; k < expr->memberCount; k++) {
			expr->members[k] = initexpr->members[k]; 
			expr->members[k]->t = malloc(sizeof(float)); 
			
			if(k < expr->memberCount - 1) {
				expr->operations[k] = initexpr->operations[k]; 
				
				expr->order[k] = k;
				// Setting the order here
				for (int j = 0; j < k; j++) {
					if(operationWeight(expr->operations[k]) > operationWeight(expr->operations[expr->order[j]])) {
						int temp = expr->order[k];
						expr->order[k] = expr->order[j];
						expr->order[j] = temp;
					}
				}
			}
		} 
	} 
	 
	free(initexpr->operations);
	free(initexpr->members);
	free(initexpr);
	 
	/*
		printf("[ MEMBERS ]\n");
		for(size_t k = 0; k < expr->memberCount; k++) {
			struct ExprMember* member = expr->members[k];
			int val = member->value.inum;
			if(member->negative) {
				val = -val;
			}
			
			printf("  member: %d\n", val);
		}
		printf("[ OPERATIONS ]\n");
		for(size_t k = 0; k < expr->memberCount - 1; k++) {
			printf("  operation: %s\n", expr->operations[k]);
		}
		printf("[ ORDER ]\n");
		for(size_t k = 0; k < expr->memberCount - 1; k++) {
			printf("  order: %d\n", expr->order[k]);
		}
	*/
	
	return expr;
} 

char castBuffer[50];
// casts a float to a string
void float_toString(float input) { 
	sprintf(castBuffer, "%f", input);
}

// casts a int to a string
void int_toString(int input) { 
	sprintf(castBuffer, "%d", input);
}

struct VarValue* expr_exec(struct Expr* expr) { 
	if(expr->result->type == TYPE_String) { 
		// STRING CONCATENATION PART
	
		struct Str* str = expr->result->data.sval;  
		str_newContent(str, 0); 
		
		for(size_t i = 0; i < expr->memberCount; i++) {
			struct ExprMember* member = expr->members[i];
			struct VarValue* val = NULL;
			
			switch(member->type) {
				case STRING_CONST: 
					str_concat(str, member->value.p);
				break;
				case INT_NUM:
					int_toString(member->value.inum);
					str_concat(str, castBuffer);
				break;
				case FLOAT_NUM:
					float_toString(member->value.fnum);
					str_concat(str, castBuffer);
				break;
				case REF:
					struct Variable* var = member->value.p;
					val = var->value; 
				break;
				case EXPRESSION:
					struct Expr* expr = member->value.p; 
					val = expr_exec(expr); 
				break;
				case FUNCTION_CALL:
					struct FunctionCall* call = member->value.p; 
					struct VarValue* functionVal = function_exec(call->func, call->expr, call->params);
					val = functionVal; 
				break; 
				case NATIVE_FUNCTION_CALL:
					struct NativeFunctionCall* native_call = member->value.p; 
					struct VarValue* nfunctionVal = nativeFunction_exec(native_call->func, native_call->expr, native_call->params);
					val = nfunctionVal;  
				break;
			}
			
			if(val != NULL) {
				switch(val->type) {
					case TYPE_String:
						str_concat(str, val->data.sval->str);
					break;
					case TYPE_Int:
						int_toString(val->data.ival);
						str_concat(str, castBuffer);
					break;
					case TYPE_Float:
						float_toString(val->data.fval);
						str_concat(str, castBuffer);
					break;
				}
			}
		} 
		return expr->result;
	}

	for(size_t i = 0; i < expr->memberCount; i++) {
		struct ExprMember* member = expr->members[i]; 
		struct VarValue* val = NULL;
		
		switch(member->type) {
			case FLOAT_NUM:
				*member->t = member->value.fnum;
				break;
			case INT_NUM:
				*member->t = member->value.inum;
				break; 
			case REF: 
				struct Variable* var = member->value.p; 
				val = var->value; 
				break;
			case EXPRESSION:
				struct VarValue* exprVal = expr_exec(member->value.p);  
				val = exprVal;
				break; 
			case FUNCTION_CALL:
				struct FunctionCall* call = member->value.p; 
				struct VarValue* functionVal = function_exec(call->func, call->expr, call->params);
				val = functionVal;  
			break;
			case NATIVE_FUNCTION_CALL:
				struct NativeFunctionCall* native_call = member->value.p; 
				struct VarValue* nfunctionVal = nativeFunction_exec(native_call->func, native_call->expr, native_call->params);
				val = nfunctionVal;  
			break;
		}
		
		if(val != NULL) {
			if(val->type == TYPE_Void) {
				*member->t = 0;
			}
			else if(val->type == TYPE_Float) {
				*member->t = val->data.fval;
			}
			else { 
				*member->t = val->data.ival;
			}
		}
		
		if(member->negative) { 
			*member->t = -*member->t;
		} 
		
		if(expr->p == NULL) {
			varValue_set(expr->result, member->t);
			
			return expr->result;
		}
		expr->p[i] = member->t;
	}
	
	 
	for(size_t n = 0; n < expr->memberCount - 1; n++) {
		size_t i = expr->order[n];
		
		char* operation = expr->operations[i]; 
		
		float *a = expr->p[i];
		float *b = expr->p[i + 1];
		float r = 0;
	
		if(equal(operation, "+")) {
			r = *a + *b;
		}
		else if(equal(operation, "*")) {
			r = (*a) * (*b);
		}
		else if(equal(operation, "/")) {
			if(*b == 0) {
				printf("\n[ RUNTIME ERROR ] why so serious\n");
				exit(1);
			}
			r = *a / *b;
		}
		else if(equal(operation, "^")) {
			r = pow(*a, *b);
		}
		else if(equal(operation, "&&")) {
			r = *a && *b;
		}
		else if(equal(operation, "||")) {
			r = *a || *b;
		}
		else if(equal(operation, "==")) {
			r = *a == *b;
		}
		else if(equal(operation, "!=")) {
			r = *a != *b;
		}
		else if(equal(operation, ">=")) {
			r = *a >= *b;
		}
		else if(equal(operation, "<=")) {
			r = *a <= *b;
		}
		else if(equal(operation, ">")) {
			r = *a > *b;
		}
		else if(equal(operation, "<")) {
			r = *a < *b;
		}
		else if(equal(operation, "%")) {
			r = fmod(*a, *b);
		}
		
		// trust the process okay?
		*a = r; 
		*b = r;
		expr->p[i + 1] = a;  
		
		if(n == expr->memberCount - 2) {  
			varValue_set(expr->result, a); 
			return expr->result; 
		}
	}
	return expr->result;
}

void expr_free(struct Expr* expr) {
	for(size_t i = 0; i < expr->memberCount; i++) {
		exprMember_free(expr->members[i]);
	} 
	free(expr->members);
	
	if(expr->p != NULL) {
		free(expr->p);
	}
	
	if(expr->operations != NULL) {
		free(expr->operations);
	}
	
	if(expr->order != NULL) {
		free(expr->order);
	} 
	varValue_free(expr->result);
	free(expr);
} 