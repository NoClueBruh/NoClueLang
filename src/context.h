void instruction_free(struct Instruction* instruction) { 
	switch(instruction->type) {
		case SET_VARIABLE:
			struct SV_Instruction* sv = instruction->inst;
			expr_free(sv->expr);
		break;
		case CONDITION:
			struct C_Instruction* ci = instruction->inst;
			if(ci->expr != NULL) {
				expr_free(ci->expr);
			}
			context_free(ci->context);
		break;
		case RETURN:  
			expr_free(instruction->inst); 
		break;
		case WHILE_LOOP:
			struct WL_Instruction* wl = instruction->inst;
			if(wl->expr != NULL) {
				expr_free(wl->expr);
			}
			context_free(wl->context);
		break;
		case RANDOM_EXPRESSION: 
			expr_free(instruction->inst); 
		break;
	}
	
	if(instruction->type != RANDOM_EXPRESSION && instruction->type != BREAK && instruction->type != RETURN) {
		free(instruction->inst);
	}
	
	free(instruction); 
}

struct Context* context_new(struct Context* parent) { 
	struct Context* context = malloc(sizeof(struct Context));
	context->parentContext = parent;
	
	context->variableCount = 0;
	context->variableSpace = 10;
	context->variables = malloc(sizeof(struct Variable*) * context->variableSpace);
	
	context->functionCount = 0;
	context->functionSpace = 10;
	context->functions = malloc(sizeof(struct Function*) * context->functionSpace);
	
	context->instructionCount = 0;
	context->instructionSpace = 10;
	context->instructions = malloc(sizeof(struct Instruction*) * context->instructionSpace);
	
	context->result = malloc(sizeof(struct VarValue));
	context->result->type = TYPE_Void;
	
	return context;
}

struct Variable* context_getVariableByName(struct Context* context, char* name) {
	for(size_t i = 0; i < context->variableCount; i++) {
		struct Variable* variable = context->variables[i];
		if(equal(variable->name, name)) return variable;
	}
	
	if(context->parentContext != NULL) {
		return context_getVariableByName(context->parentContext, name);
	}
	
	return NULL;
}

struct Function* context_getFunctionByName(struct Context* context, char* name) {
	for(size_t i = 0; i < context->functionCount; i++) {
		struct Function* function = context->functions[i];
		if(equal(function->name, name)) return function;
	}
	
	if(context->parentContext != NULL) {
		return context_getFunctionByName(context->parentContext, name);
	}
	
	return NULL;
}


void context_pushVariable(struct Context* context, struct Variable* variable) {
	if(context->variableCount + 1 > context->variableSpace) {
		context->variableSpace += 10;
		context->variables = realloc(context->variables, sizeof(struct Variable*) * context->variableSpace);
	}
	
	context->variables[context->variableCount] = variable;
	context->variableCount++;
}

void context_pushFunction(struct Context* context, struct Function* function) {
	if(context->functionCount + 1 > context->functionSpace) {
		context->functionSpace += 10;
		context->functions = realloc(context->functions, sizeof(struct Function*) * context->functionSpace);
	} 
	
	context->functions[context->functionCount] = function; 
	context->functionCount++; 
}

void context_pushInstruction(struct Context* context, struct Instruction* inst) {
	if(context->instructionCount + 1 > context->instructionSpace) {
		context->instructionSpace += 10;
		context->instructions = realloc(context->instructions, sizeof(struct Instruction*) * context->instructionSpace);
	} 
	
	context->instructions[context->instructionCount] = inst; 
	context->instructionCount++; 
} 

// This function basically makes all the instructions that are inside a collection of tokens!
// We also need to know the context's return type!
void context_bind(struct Context* context, struct Scan* scan, enum VarType returnType, size_t from, size_t to) {
	size_t i = from; 
	while(i < to) {
		struct Token* token = scan_getTokenAt(scan, i);
		if(token->type == KEYWORD) { 
			char typeNum = toType(token->content);
			if(typeNum > 0) { 
				if(i + 1 == to) {
					printf("\n[ SYNTAX ERROR ] Unexpected type\n");
					exit(1);
				}
				
				struct Token* nameToken =   scan_getTokenAt(scan, i + 1);
				if(nameToken->type != LITERAL) { 
					printf("\n[ SYNTAX ERROR ] Variable name cannot be a keyword, symbol, number or string\n");
					exit(1);
				}
				  
				char* varName 			=	nameToken->content;  
				enum VarType varType 	=	typeNum;
				
				if(context_getVariableByName(context, varName) != NULL) {
					printf("\n[ COMPILE ERROR ] Variable '%s' already exists in this scope\n", varName);
					exit(1);
				} 
				
				struct Variable* variable = variable_new(varName, varType);
				context_pushVariable(context, variable);  
				
				struct Token* nt = scan_getTokenAt(scan, i + 2);
				if(nt->type == SYMBOL && equal(nt->content, "=")) {
					struct Instruction* instruction = malloc(sizeof(struct Instruction));
					struct SV_Instruction* sv = malloc(sizeof(struct SV_Instruction));
					sv->var = variable;
					 
					size_t end = scan_getNextSemicolon(scan, i + 2, to); 
					scan_checkOutOfBounds(end, to, "Expected ;");
					
					sv->expr = expr_new(scan, context, i + 3, end);
					if(sv->expr->result->type != variable->value->type) {
						printf("\n[ COMPILE ERROR ] variable '%s' is of type %s!\n", variable->name, ofType(variable->value->type));
						exit(1);
					}
					i = end - 1;
					
					instruction->inst = sv;
					instruction->type = SET_VARIABLE;  
					
					context_pushInstruction(context, instruction);
				}
				else if(nt->type == SYMBOL && equal(nt->content, ";")) { 
					i++;
				}
				else {
					printf("\n[ SYNTAX ERROR ] Expected ; or = after variable declaration! (not %s)\n", nt->content);
					exit(1);
				} 
				
				//printf("\nSUCCESSFULLY ADDED VARIABLE OF NAME %s AND TYPE %s\n", nameToken->content, token->content);
				
				i++;
			}
			else if(equal(token->content, "function")) {  
				size_t end = scan_getEnd(scan, i, to);
				scan_checkOutOfBounds(end, to, "Expected end");
				 
				// this function also pushes the function into the context
				function_new(scan, context, i, end); 
				
				i = end;
			}
			else if(equal(token->content, "while")) {
				size_t expr_end = scan_getNextToken(scan, i, to, "do", KEYWORD); 
				scan_checkOutOfBounds(expr_end, to, "Expected do");
				
				struct Expr* expr = NULL;
				if(expr_end > i + 1) {
					expr = expr_new(scan, context, i + 1, expr_end); 
					if(expr->result->type != TYPE_Int) {
						printf("\n[ COMPILE ERROR ] while condition must be of type int\n");
						exit(1);
					}
				}
				size_t end = scan_getEnd(scan, expr_end, to); 
				scan_checkOutOfBounds(end, to, "Expected end");
				
				struct Context* whileBody = NULL;
				whileBody  = context_new(context); 
				context_bind(whileBody, scan, TYPE_UNKNOWN, expr_end + 1, end);
				
				if(whileBody->result->type != TYPE_Void) {
					if(returnType == TYPE_UNKNOWN || returnType == TYPE_Void) {
						returnType = whileBody->result->type;
					}
					else if(whileBody->result->type != returnType) {
						printf("\n[ COMPILE ERROR ] Context is supposed to return value of type %s, not %s\n", ofType(returnType), ofType(whileBody->result->type));
						exit(1);
					}
				}
				
				struct Instruction* instruction = malloc(sizeof(struct Instruction));
				struct WL_Instruction* wl = malloc(sizeof(struct WL_Instruction));
				
				wl->expr = expr;
				wl->context = whileBody; 
				
				instruction->type = WHILE_LOOP;
				instruction->inst = wl;

				context_pushInstruction(context, instruction);
				
				i = end;
			}
			else if(equal(token->content, "break")) {
				struct Instruction* instruction = malloc(sizeof(struct Instruction)); 
				instruction->inst = NULL; 
				instruction->type = BREAK;

				context_pushInstruction(context, instruction);
				break;
			}
			else if(equal(token->content, "return")) { 
				size_t end = scan_getNextSemicolon(scan, i, to); 
				scan_checkOutOfBounds(end, to, "Expected ;"); 
				
				// Check if the return statement is empty
				if(i + 1 == end) {
					struct Instruction* instruction = malloc(sizeof(struct Instruction)); 
					instruction->inst = NULL; 
					instruction->type = BREAK;

					context_pushInstruction(context, instruction);
					break;
				}
				
				struct Instruction* instruction = malloc(sizeof(struct Instruction));  
				struct Expr* returnExpr = expr_new(scan, context, i + 1, end);
				
				if(returnExpr->result->type != TYPE_Void) {
					if(returnType == TYPE_UNKNOWN || returnType == TYPE_Void) {
						returnType = returnExpr->result->type;
					}
					else if(returnExpr->result->type != returnType) {
						printf("\n[ COMPILE ERROR ] Context is supposed to return value of type %s, not %s\n", ofType(returnType), ofType(returnExpr->result->type));
						exit(1);
					}
				}

				instruction->inst = returnExpr;
				instruction->type = RETURN;
				
				context_pushInstruction(context, instruction);
				break;
				// no need to make the next instructions since they are not gonna run anyways
			}
			else if(equal(token->content, "if")) {  
				size_t s = i;
				char trigger = 0;
				
				i++;
				while(i < to) {
					struct Token* nt = scan_getTokenAt(scan, i);
					if(nt->type == KEYWORD) {
						if(scan_hasBody(nt->content)) {
							i = scan_getEnd(scan, i + 1, to) + 1;
							scan_checkOutOfBounds(i, to, "Expected end");
							continue;
						}
						else if(equal(nt->content, "end")) {
							trigger = 2;
						} 
						else if(equal(nt->content, "elseif")) {
							trigger = 1;
						}
						else if(equal(nt->content, "else")) {
							trigger = 1;
						}  
					}  
					
					if(trigger > 0) {  
						struct Instruction* instruction = malloc(sizeof(struct Instruction));
						struct C_Instruction* ci = malloc(sizeof(struct C_Instruction));
						struct Expr* expr = NULL;
						struct Context* body = context_new(context);
					
						struct Token* leadToken = scan_getTokenAt(scan, s);
						if(leadToken->type != KEYWORD) {
							printf("\n[ SYNTAX ERROR ] expected elseif or else\n");
							exit(1);
						}
						else {
							if(equal(leadToken->content, "if") || equal(leadToken->content, "elseif")) {
								size_t expr_end = scan_getNextToken(scan, s, to, "do", KEYWORD); 
								scan_checkOutOfBounds(expr_end, to, "Expected do");
								
								if(s + 1 < expr_end) {
									expr = expr_new(scan, context, s + 1, expr_end); 
									if(expr->result->type != TYPE_Int) {
										printf("\n[ COMPILE ERROR ] condition must be of type int\n");
										exit(1);
									}
								} 
								context_bind(body, scan, TYPE_UNKNOWN, expr_end + 1, i);
							}
							else if(equal(leadToken->content, "else")) { 
								context_bind(body, scan, TYPE_UNKNOWN, s + 1, i);
							}
							else {
								printf("\n[ SYNTAX ERROR ] ok bro what the sigma\n");
								exit(1);
							}
						} 
						
						if(body->result->type != TYPE_Void) {
							if(returnType == TYPE_UNKNOWN || returnType == TYPE_Void) {
								returnType = body->result->type;
							}
							else if(body->result->type != returnType) {
								printf("\n[ COMPILE ERROR ] Context is supposed to return value of type %s, not %s\n", ofType(returnType), ofType(body->result->type));
								exit(1);
							}
						} 
						
						ci->isHead = equal(leadToken->content, "if");
						ci->expr = expr;
						ci->context = body;
						
						instruction->inst = ci;
						instruction->type = CONDITION;
						context_pushInstruction(context, instruction);
						
						s = i;
						
						if(trigger == 2) {
							break;
						}
						trigger = 0;
					}
					
					i++;
				} 
			} 
			else { 
				printf("\n[ SYNTAX ERROR ] Unexpected keyword %s!\n", token->content);
				exit(1);
			}
		}
		else if(token->type == LITERAL && i + 1 < to){ 
			struct Function* func = context_getFunctionByName(context, token->content);
			struct NativeFunctionCollection* collection = native_getCollection(token->content);
			if(func != NULL || collection != NULL) { 
				size_t semicolon = scan_getNextSemicolon(scan, i, to); 
				scan_checkOutOfBounds(semicolon, to, "Expected ;");
				
				struct Expr* expr = expr_new(scan, context, i, semicolon); 
				
				struct Instruction* instruction = malloc(sizeof(struct Instruction));
				instruction->inst = expr;
				instruction->type = RANDOM_EXPRESSION;  
				context_pushInstruction(context, instruction);
				
				i = semicolon;
			}
			else { 
				struct Token* nextToken = scan_getTokenAt(scan, i + 1);
				if(nextToken->type == SYMBOL && equal(nextToken->content, "=")) {
					struct Variable* targetVar = context_getVariableByName(context, token->content);
					if(targetVar == NULL) { 
						printf("\n[ COMPILE ERROR ] variable '%s' doesn't exist bro\n", token->content);
						exit(1);
					}
					
					size_t semicolon = scan_getNextSemicolon(scan, i + 1, to); 
					scan_checkOutOfBounds(semicolon, to, "Expected ;");
					
					struct Expr* expr = expr_new(scan, context, i + 2, semicolon); 
					if(expr->result->type != targetVar->value->type) {
						printf("\n[ COMPILE ERROR ] variable '%s' if of type %s!\n", targetVar->name, ofType(targetVar->value->type));
						exit(1);
					} 
					
					struct Instruction* instruction = malloc(sizeof(struct Instruction));
					struct SV_Instruction* sv = malloc(sizeof(struct SV_Instruction));
					
					sv->var = targetVar; 
					sv->expr = expr;
					
					instruction->inst = sv;
					instruction->type = SET_VARIABLE;  
					
					context_pushInstruction(context, instruction);
					
					i = semicolon;
				}
				else {
					printf("\n[ SYNTAX ERROR ] Unexpected token %s!\n", token->content);
					exit(1);
				}
			} 
		}
		else {
			printf("\n[ SYNTAX ERROR ] Unexpected token %s!\n", token->content);
			exit(1);
		}
		i++; 
	}
	
	if(returnType == TYPE_UNKNOWN) {
		returnType = TYPE_Void; 
	}
	
	context->result->type = returnType; 
	varValue_default(context->result);
}

struct VarValue* context_exec(struct Context* context) {
	size_t i = 0; 
	while(i < context->instructionCount) {
		struct Instruction* instruction = context->instructions[i]; 
		
		switch(instruction->type) {
			case SET_VARIABLE:
				struct SV_Instruction* sv = instruction->inst;
				struct Variable* var = sv->var;
				struct Expr* expr = sv->expr; 
				
				variable_setValue(var, expr_exec(expr));  
			break;
			case WHILE_LOOP:
				struct WL_Instruction* wl = instruction->inst; 
				while(wl->expr == NULL || expr_exec(wl->expr)->data.ival) { 
					struct VarValue* val = context_exec(wl->context);  
					if(val != NULL) {
						if(val->type == TYPE_Void) break;
						else return val;
					}
				}
			break;
			case CONDITION:
				struct C_Instruction* ci = instruction->inst;
				struct Expr* condition = ci->expr;
				struct Context* body = ci->context;
				
				if(condition == NULL || expr_exec(condition)->data.ival) { 
					struct VarValue* val = context_exec(body); 
					if(val != NULL) { 
						return val;
					}
					
					//skip next else and elseif conditions
					i++;
					while(i < context->instructionCount) {
						struct Instruction* ni = context->instructions[i];
						if(ni->type == CONDITION) {
							struct C_Instruction* ci = ni->inst;
							if(ci->isHead) {
								break;
							}
						}
						else {
							break;
						}
						i++;
					}
					i--;
				}
			break;
			case RETURN:
				struct Expr* result = instruction->inst; 
				return expr_exec(result);
			break;
			case BREAK: 
				return context->result;
			break;
			case RANDOM_EXPRESSION:
				expr_exec(instruction->inst);
			break;
		}
		
		i++;
	} 
	return NULL;
}

void context_free(struct Context* context) {   
	for(size_t i = 0; i < context->variableCount; i++) {
		variable_free(context->variables[i]);
	}   
	for(size_t i = 0; i < context->functionCount; i++) {
		function_free(context->functions[i]);
	}  
	for(size_t i = 0; i < context->instructionCount; i++) {
		instruction_free(context->instructions[i]);
	} 
	 
	varValue_free(context->result); 
	free(context->instructions); 
	free(context->functions); 
	free(context->variables); 
	free(context); 
}