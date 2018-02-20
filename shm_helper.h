#ifndef SHM_HELPER_H
#define SHM_HELPER_H

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

int helper_open(char *path, int oflag,  mode_t mode, int size);
int helper_open_default(char *path, int size);
int helper_unlink(char *path);

#endif
