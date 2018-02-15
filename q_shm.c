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
	char id = sh_id + '\0';
	printf("Name: %s.\n", name());
	return NULL;
}

char *name(){
	return strcat(sh_name, ch_digits(sh_id++));
}
