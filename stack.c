#include "node.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "stack.h"
#include <stdlib.h>


struct Stack {
	uint32_t top;
	uint32_t capacity;
	Node **items;

};
Stack *stack_create(uint32_t capacity){
	Stack * s = (Stack *)malloc(sizeof(Stack));
	s-> top = 0;
	s-> capacity = capacity;
	s->items = (Node **)malloc(capacity * sizeof(Node*));
	if(!s->items){
		free(s);
		s = NULL;
	}

	return s;
}

void stack_delete(Stack **s){
	uint64_t index = 0;
	while(index < (*s)->top){

		if((*s)->items[index] != NULL){
			
			node_print((*s)->items[index]);
			node_delete(&((*s)->items[index]));
			
		}
		index++;
	}
	free((*s)->items);
	free(*s);
	*s = NULL;

}

bool stack_empty(Stack *s){
	if(s-> top == 0){
		return true;
	}
	return false;
}

bool stack_full(Stack *s){
	if(s->top == s->capacity){
		return true;
	}
	return false;
}

uint32_t stack_size(Stack *s){
	return s->top;

}

bool stack_push(Stack *s, Node *n){
	if(stack_full(s)){
		return false;
	}
	//pushed value will now be in the top of the stack
	s->items[s->top] = n;
	s->top++;
	return true;
}

bool stack_pop(Stack *s, Node **n){
	if(stack_empty(s)){
		return false;
	}

	s->top--;
	//pop the value so we will have a new top value
	*n = s->items[s->top];
	//makes the node ** from a  *
	
	 
	return true;

}

void stack_print(Stack *s){
	for(uint32_t x = 0; x < s->top; x++){
		node_print(s->items[x]);
	}

}
