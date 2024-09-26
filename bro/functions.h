enum VarType toType(char* str) {
	if(equal(str, "float")) return TYPE_Float;
	else if(equal(str, "int")) return TYPE_Int;
	else if(equal(str, "string")) return TYPE_String;
	else if(equal(str, "void")) return TYPE_Void;
	return 0;
}

enum VarType toType_c(char c){
	if(c == 'f') return TYPE_Float;
	else if(c == 'i') return TYPE_Int;
	else if(c == 's') return TYPE_String;
	else if(c == 'v') return TYPE_Void;
	return 0;
}

char* ofType(enum VarType type) {
	if(type == TYPE_Float) return "float";
	else if(type == TYPE_Int) return "int";
	else if(type == TYPE_String) return "string";
	else if(type == TYPE_Void) return "void";
	return "unknown";
} 

// wow
 
struct 	Expr* 		expr_new					(struct Scan*, struct Context*, size_t, size_t); 
struct 	VarValue* 	expr_exec					(struct Expr*); 
void 				expr_free					(struct Expr*);

struct 	Context* 	context_new					(struct Context*);
struct 	VarValue* 	context_exec				(struct Context*);
struct 	Function* 	context_getFunctionByName	(struct Context*, char*);
struct 	Variable* 	context_getVariableByName	(struct Context*, char*);
void   				context_pushVariable		(struct Context*, struct Variable*);
void 				context_pushFunction		(struct Context*, struct Function*);
void 				context_pushInstruction		(struct Context*, struct Instruction*);
void 				context_bind				(struct Context*, struct Scan*, enum VarType, size_t, size_t);
void   				context_free				(struct Context*);


void 				variable_setValue			(struct Variable*, struct VarValue*);
void 				variable_resetValue			(struct Variable*);
void 				variable_free				(struct Variable*);


struct Function*	function_new				(struct Scan*, struct Context*, size_t, size_t);
struct VarValue* 	function_exec 				(struct Function*, struct Expr**, size_t);
void 				function_free				(struct Function*);