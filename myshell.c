#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CYAN	"\x1b[36m"
#define GREEN	"\x1b[32m"
#define MAGENTA	"\x1b[35m"
#define RED		"\x1b[31m"
#define RESET	"\x1b[0m"

const char * const DELIMITERS = " \t\n()<>|&;";

int main(void){
	char *input = malloc(sizeof(char)*500), *cwd = malloc(sizeof(char)*300), *iterator, *mode, *homedir = getenv("HOME"), *user = malloc(sizeof(char)*50);
	getlogin_r(user, 49);

	while(1){
		mode = NULL;
		getcwd(cwd, sizeof(char)*299); 
		if(strstr(cwd, homedir)){
			printf(MAGENTA "# " GREEN "%s@myshell " CYAN "~%s \n" MAGENTA "$ " RESET, user, cwd+strlen(homedir));
		}else{
			printf(MAGENTA "# " GREEN "%s@myshell " CYAN "%s \n" MAGENTA "$ " RESET, user, cwd);
		}
		if(fgets(input, 499, stdin) && *input != '\n'){
			iterator = strstr(input, ">>"); 
			if(iterator){
				mode = "a";
			}else{
				iterator = strstr(input, ">"); 
				if(iterator){
					mode = "w";
				}else{
					iterator = strstr(input, "<"); 
					if(iterator){
						mode = "r";
					}
				}
			}
			iterator = strtok(input, DELIMITERS);
			if(!strcmp(iterator, "exit")){
				free(input);
				free(user);
				free(cwd);
				exit(EXIT_SUCCESS);
			}else if(!strcmp(iterator, "cd")){
				iterator = strtok(NULL, DELIMITERS);
				if(!iterator){
					iterator = homedir;
				}
				if(chdir(iterator) == -1){
						printf(RED "cd: %s\n" RESET, strerror(errno));
				}
			}else{
				pid_t pid = fork();
				if(pid == 0){
					char *arguments[20];
					int i = 0;
					arguments[0] = iterator;
					while (arguments[i]){
						arguments[++i] = strtok(NULL, DELIMITERS);
 					}
 					if(mode){
 						FILE *f;
 						if(arguments[--i]){
							if(mode == "r"){
								f = freopen(arguments[i], "r", stdin);
							}else{
                                f = freopen(arguments[i], mode, stdout);
                            }
							arguments[i] = NULL;
						}
						if(f == NULL){
							printf(RED "Error: %s\n" RESET, strerror(errno));
						}
 					}
 					if(execvp(arguments[0], arguments) == -1){
 						if(errno == 2){
 							printf(RED "Error: Command not found: %s\n" RESET, arguments[0]);
 						}else{
 							printf(RED "Error: %s\n" RESET, strerror(errno));
 						}
					}
					free(input);
					free(user);
					free(cwd);
					exit(EXIT_SUCCESS);
				}else if(pid > 0){
					if(wait(NULL) == -1){
						printf(RED "Error: %s\n" RESET, strerror(errno));
					}
				}else if(pid == -1){
					printf(RED "Error: %s\n" RESET, strerror(errno));
				}
			}
		}
	}
	free(input);
	free(user);
	free(cwd);
	return 0;
}