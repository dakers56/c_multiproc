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
	printf("Testing ch_digits.\n");
	for(int i = 0; i < size_test; i++){
		int digs = n_digits(test[i]);
                char *d = ch_digits(test[i]);
                printf("Final result of \'ch_digits\': %s \n", d);
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

char *ch_digits(int a){
	int *int_digs = digits(a);
	int num_digs = n_digits(a);

	char *as_ch = malloc(1 + (num_digs * sizeof(char)));	
	if(!as_ch){
		printf("Could not allocate memory for character digits.\n");
		return NULL;
	}

	for(int i = 0; i < num_digs; i++){
		*(as_ch + i) = (char) *(int_digs + i) + 48;
		printf("*(int_digs + i): %d.\n", *(int_digs + i));
	}
	*(as_ch + num_digs) = '\0';
	return as_ch;	
}
