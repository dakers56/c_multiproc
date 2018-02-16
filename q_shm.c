#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "digits.h"
#include "q_shm.h"
#include "shm_helper.h"

static char sh_name[] = "/Q_SHARE_OBJ_";
static int sh_id = 1;

char *name();

int main(int argc, char ** argv){
	return (int) new_share(0);
}

SHARE * new_share(int size){
	char *nm;
	for (int i = 0; i < 1000; i++){
	nm = name();
	printf("Name: %s.\n", nm);
	free(nm);
}
	return NULL;
}

char *name(){
	char *name_cp = malloc(sizeof(sh_name));
	strcpy(name_cp, sh_name);
	printf("sh_id: %d.\n", sh_id);
	printf("ch_digits(sh_id++): %s.\n", ch_digits(sh_id));
	char *digs=ch_digits(sh_id);
	strcat(name_cp, digs);
	free(digs);
	sh_id++;
	return name_cp;
}