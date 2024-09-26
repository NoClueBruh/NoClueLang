void nativeArrays_make(struct NativeArrays* arrays) {
	arrays->arrayCount = 0;
	arrays->arraySize = 10;
	
	arrays->arrays = malloc(sizeof(struct NativeArray) * arrays->arraySize);
} 

void nativeArrays_newArray(struct NativeArrays* arrays) {
	if(arrays->arrayCount + 1 > arrays->arraySize) {
		arrays->arraySize += 10; 
		arrays->arrays = realloc(arrays->arrays, sizeof(struct NativeArray) * arrays->arraySize);
	}
	
	struct NativeArray* arr = &arrays->arrays[arrays->arrayCount++];
	arr->count = 0;
	arr->space = 10;
	arr->elements = malloc(sizeof(union Value) * arr->space); 
}

void nativeArray_push(struct NativeArray* array, struct VarValue* element, size_t index) {
	if(array->count + 1 > array->space) {
		array->space += 10;  
		array->elements = realloc(array->elements, sizeof(union Value) * array->space);  
	}
	
	for(size_t i = array->count; i > index; i--) {
		switch(element->type) {
			case TYPE_Float:
				array->elements[i].fval = array->elements[i - 1].fval;
			break;
			case TYPE_Int:
				array->elements[i].ival = array->elements[i - 1].ival;
			break;
			case TYPE_String:
				array->elements[i].sval = array->elements[i - 1].sval;
			break;
		}
	}
	
	switch(element->type) {
		case TYPE_Float:
			array->elements[index].fval = element->data.fval;
		break;
		case TYPE_Int:
			array->elements[index].ival = element->data.ival;
		break;
		case TYPE_String:
			array->elements[index].sval = str_newCopy(element->data.sval);
		break;
	}
	array->count++; 
}

void nativeArray_allocate(struct NativeArray* array, size_t len) {
	if(array->space < len) {
		array->space = len;
		array->elements = realloc(array->elements, sizeof(union Value) * array->space); 
	}
}

void nativeArray_free(struct NativeArray* array, char str) {
	if(str) {
		for(size_t i = 0; i < array->count; i++) { 
			str_free(array->elements[i].sval, 0); 
		}
	}
	free(array->elements); 
} 

void nativeArray_removeElement(struct NativeArray* array, size_t index, enum VarType type) { 
	if(type == TYPE_String) {
		str_free(array->elements[index].sval, 0); 
	}
	
	for(size_t i = index; i < array->count - 1; i++) {
		switch(type) {
			case TYPE_Float:
				array->elements[i].fval = array->elements[i + 1].fval;
			break;
			case TYPE_Int:
				array->elements[i].ival = array->elements[i + 1].ival;
			break;
			case TYPE_String: 
				array->elements[i].sval = array->elements[i + 1].sval; 
			break;
		}
	}

	array->count--;
}

void nativeArray_removeElements(struct NativeArray* array, size_t index, size_t len, enum VarType type) { 
	if(index + len > array->count - 1) {
		len = array->count - index;
	}
	
	if(type == TYPE_String) {
		for(size_t i = index; i < index + len; i++) {
			str_free(array->elements[i].sval, 0); 
		}
	}
	
	for(size_t i = index; i < array->count - len; i++) {
		switch(type) {
			case TYPE_Float:
				array->elements[i].fval = array->elements[i + len].fval;
			break;
			case TYPE_Int:
				array->elements[i].ival = array->elements[i + len].ival;
			break;
			case TYPE_String: 
				array->elements[i].sval = array->elements[i + len].sval; 
			break;
		}
	}

	array->count -= len;
}

void nativeArrays_free(struct NativeArrays* arrays, char str) {
	for(size_t i = 0; i < arrays->arrayCount; i++) {
		nativeArray_free(&arrays->arrays[i], str);
	}
	free(arrays->arrays);
}