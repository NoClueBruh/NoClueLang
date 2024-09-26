struct Str {
	char *str;
	size_t len;
	size_t alen;
};

enum TokenType {
	INT,
	FLOAT,
	STRING,
	SYMBOL,
	LITERAL,
	KEYWORD
};

struct Token {
	char *content;
	enum TokenType type;
};

struct Scan {
	struct Token **tokens;
	size_t len;
	size_t alen;
}; 

enum VarType {
	TYPE_Int   			= 1,
	TYPE_Float 			= 2,
	TYPE_String 		= 3,
	TYPE_Void  			= 4, 
	TYPE_UNKNOWN 		= 5, 
};
 
union Value {
	struct Str* sval; 
	float fval;
	int ival;
};

struct VarValue {
	union Value data;
	enum VarType type;
}; 

struct Variable {
	char* name; 
	struct VarValue* value; 
};

enum InstructionType {
	SET_VARIABLE,
	CONDITION,
	WHILE_LOOP,
	BREAK,
	RETURN,
	RANDOM_EXPRESSION
};

struct Instruction {
	void* inst;
	enum InstructionType type;
};

struct SV_Instruction {
	struct Variable* var;
	struct Expr* expr;
};

struct C_Instruction {
	struct Context* context;
	struct Expr* expr;
	
	char isHead;
};

struct WL_Instruction {
	struct Context* context;
	struct Expr* expr;
}; 

struct Context {
	struct Context* parentContext;
	
	struct Variable** variables;
	size_t variableSpace;
	size_t variableCount;
	
	struct Function** functions; 
	size_t functionSpace;
	size_t functionCount;
	
	struct Instruction** instructions;
	size_t instructionSpace;
	size_t instructionCount;
	
	struct VarValue* result;
};

struct Function {
	char* name;  
	
	size_t paramCount; 
	struct Context* context;
}; 

struct NativeFunctionCollection {
	char* name;
	
	size_t native_function_count;
	struct NativeFunction** native_functions;
};

struct NativeFunction {
	char* name;  
	
	size_t paramCount;  
	struct VarValue** params;
	
	void (*ref)(struct NativeFunction*);
	struct VarValue* result;
};

struct NativeArrays {
	size_t arrayCount;
	size_t arraySize;
	
	struct NativeArray* arrays;
};

struct NativeArray {
	size_t count;
	size_t space;
	
	union Value* elements; 
};

struct FunctionCall {
	size_t params;
	
	struct Expr** expr;
	struct Function* func;
}; 

struct NativeFunctionCall {
	size_t params;
	
	struct Expr** expr;
	struct NativeFunction* func;
}; 

enum ExprMemberType {
	FLOAT_NUM,
	INT_NUM,
	STRING_CONST,
	EXPRESSION,
	FUNCTION_CALL,
	NATIVE_FUNCTION_CALL,
	REF
};

struct ExprMember {
	union {
		void* p;
		float fnum;
		int inum;
	} value;
	
	char negative;
	enum ExprMemberType type;
	float* t;
}; 

struct InitExpr {
	struct ExprMember** members;
	size_t memberCount;
	size_t memberSpace;  
	
	char** operations;
	size_t operationCount;
	size_t operationSpace; 
};

struct Expr {
	struct ExprMember** members; 
	size_t memberCount; 
	
	char** operations;
	float** p;
	size_t* order; 
	
	struct VarValue* result;
};