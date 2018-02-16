#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "digits.h"
#include "q_shm.h"
#include "shm_helper.h"

static char sh_name_base[] = "/Q_SHARE_OBJ_";
static int sh_id = 0;

static char sem_name_base[] =  "/Q_SHARE_OBJ_SEM_";
static int sem_id = 0;

char *name();
char *sh_name();
char *sem_name();

int main(int argc, char ** argv){
	return (int) new_share(0);
}

SHARE * new_share(int size){
	char *nm;
	for (int i = 0; i < 1000; i++){
	nm = sh_name();
	printf("Name: %s.\n", nm);
	free(nm);
	nm = sem_name();
        printf("Sem name: %s.\n", nm);
        free(nm);
}
	return NULL;
}

char *name(char *name_base, int id){
	char *name_cp = malloc(sizeof(sh_name_base));
        strcpy(name_cp, name_base);
        printf("sh_id: %d.\n", id);
        printf("ch_digits(sh_id++): %s.\n", ch_digits(id));
        char *digs=ch_digits(id);
        strcat(name_cp, digs);
        free(digs);
        return name_cp;
}

char *sh_name(){
	return name(sh_name_base, ++sh_id);
}

char *sem_name(){
	return name(sem_name_base, ++sem_id);
	}
