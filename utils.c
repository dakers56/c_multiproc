#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

int main(int argc, char *argv){
	printf("Testing util functions.\n");
	int test[] = {123, 4567, 89012};
	int size_test = sizeof(test) / sizeof(test[0]); 
	for(int i = 0; i < size_test; i++){
		printf("Number of digits in %d: %d.\n", test[i], n_digits(test[i]));
	}		
}

int n_digits(int a){
	int digs = 1;
	while((a /= 10) > 0) digs++;	
	return digs;
}
