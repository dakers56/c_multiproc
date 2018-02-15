#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "digits.h"


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

int n_digits(int a){
	int digs = 1;
	while((a /= 10) > 0) digs++;	
	return digs;
}

int *digits(int a){
	if(a < 0) a = -1 * a;
	int size = n_digits(a);
	int *digs = malloc(size);
	if(!digs){
		printf("Error allocating space for digits array.\n");
		return NULL;
	}
	//*digs = a % 10;
	int *cur = digs + size;
	int i = 0;
	while(i < size){
	//printf("i: %d.\n", i);
	//printf("a % 10: %d.\n", a % 10);
	//printf("cur before increment: %d.\n", cur);
	cur--;
	//printf("cur after increment: %d.\n", cur - i);
	*cur  = a % 10;
	a /= 10;
	//printf("*(cur): %d.\n", *(cur));
	i++;
	} 
	return digs;	
}
