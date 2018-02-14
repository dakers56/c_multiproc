#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "digits.h"


/*
int main(int argc, char *argv){
	printf("Testing util functions.\n");
	int test[] = {123, 4567, 89012};
	int size_test = sizeof(test) / sizeof(test[0]); 
	for(int i = 0; i < size_test; i++){
		int digs = n_digits(test[i]);
		printf("Number of digits in %d: %d.\n", test[i], digs);
		int * d = digits(test[i]);
		printf("Final result of \'digits\': \n");
		for(int j = 0; j < digs; j++){
			printf("%d\n", d[j]);
		}	
	}		
	return 0;
}
*/
int n_digits(int a){
	int digs = 1;
	while((a /= 10) > 0) digs++;	
	return digs;
}

int *digits(int a){
	int size = n_digits(a);
	int *digs = malloc(size);
	if(!digs){
		printf("Error allocating space for digits array.\n");
		return NULL;
	}
	return digs;	
}
