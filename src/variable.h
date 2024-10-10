// NEW VARIABLE BOOM
struct Variable* variable_new(char* name, enum VarType type) {
	struct Variable* variable = malloc(sizeof(struct Variable));
	struct VarValue* val = malloc(sizeof(struct VarValue));
	
	val->type = type; 
	varValue_default(val);
	
	variable->name = name;
	variable->value = val;
	
	return variable;
}

// Set variable's value
void variable_setValue(struct Variable* var, struct VarValue* val) {  
	if(varValue_assign(var->value, val)) { 
		printf("\n[ RUNTIME ERROR ] variable '%s' is of type %s!\n", var->name, ofType(var->value->type));
		exit(1);
	}
}

// Reset variable's value
void variable_resetValue(struct Variable* var) {
	if(var->value->type == TYPE_String){
		str_newContent(var->value->data.sval, 0);
	}
	else {
		varValue_default(var->value);
	}
}

void variable_free(struct Variable* var) { 
	varValue_free(var->value);
	free(var);
}
