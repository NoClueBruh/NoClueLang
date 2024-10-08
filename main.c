#include<stdio.h>
#include<stdlib.h> 
#include<string.h>
#include<ctype.h>
#include<math.h>

#ifdef _WIN32
	#include<conio.h> 
#endif


char equal(char* a, char* b) {
	return strcmp(a, b) == 0;
} 

size_t allocation_counter = 0;

// yeah
void* p_malloc(size_t size) {
	void* t = malloc(size); 
	allocation_counter++;
	if(t == NULL) {
		printf("\n[ ALLOCATION FAILED ] Insufficient memory available, i tried allocating %d bytes damn\n", size);
		exit(1);
		//killing myself
	}
	return t;
}

void* p_realloc(void* p, size_t size) {
	void* t = realloc(p, size);
	if(t == NULL) {
		printf("\n[ REALLOCATION FAILED ] Insufficient memory available \n");
		exit(1);
		//killing myself
	}
	return t;
} 

void p_free(void* p) {
	free(p);
	allocation_counter--;
}

#define malloc p_malloc 
#define realloc p_realloc
#define free p_free  
// please dont burn me

#include"bro/structs.h"
#include"bro/functions.h"

#include"src/stringBuilder.h"
#include"src/scan.h"
#include"src/helper.h"
#include"bro/native.h"
#include"bro/array.h"
#include"bro/native_things.h"
#include"src/expr.h"
#include"src/variable.h"
#include"src/function.h" 
#include"src/context.h"
 
struct Str* readFile(char *path) {
    struct Str* buffer = str_new();

    FILE *f = fopen(path, "r");
	if(f == NULL) {
		return NULL;
	}
	
    while(1) {
        char c;
        fread(&c, 1, 1, f);
        if(feof(f)) break;
 
        str_pushChar(buffer, c);
    }
    fclose(f);

    return buffer;
}

int main (int argc, char *argv[]) {  
	if(argc != 2) {
		printf("Invalid use of NoClueLang interpreter!\n"); 
		exit(1);
	}
	 
	struct Str* fileContent = readFile(argv[1]);
	if(fileContent == NULL) {
		printf("File '%s' doesn't exist!\n", argv[1]); 
		exit(1);
	}
	
	struct Scan* scan = scan_new(fileContent->str);
	str_free(fileContent, 0); 
	
	//token_trace(scan, 0, scan->len, 2);
	
	native_define();
	
	struct Context* mainContext = context_new(NULL); 
	context_bind(mainContext, scan, TYPE_UNKNOWN, 0, scan->len); 
	 
	struct VarValue* val = context_exec(mainContext);   
	if(val != NULL) { 
		switch(val->type) {
			case TYPE_Int:
				printf("\n[ MAIN CONTEXT OUTPUT ] %d\n", val->data.ival);
			break;
			case TYPE_Float:
				printf("\n[ MAIN CONTEXT OUTPUT ] %f\n", val->data.fval);
			break;
			case TYPE_String:
				printf("\n[ MAIN CONTEXT OUTPUT ] '%s'\n", val->data.sval->str);
			break;
		}
	} 
	 
	native_free();   
	context_free(mainContext);  
	scan_free(scan); 
	
	//printf("ALLOCATIONS STATUS ( MEMORY LEAKS ): %d\n", allocation_counter);
	return 0;
}
