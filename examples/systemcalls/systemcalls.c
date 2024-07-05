#include "systemcalls.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
	int ret;
	//Execute command using system()
	ret = system(cmd);
	//checking the return value of system()
	if(ret)
		return false;

	return true;

}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

	pid_t pid;
	int status;

	pid = fork(); //create new process using fork()
	
	
	if(pid == 0){ //Child process, execution using execv()
		if(execv(command[0], command) == -1)
			exit(EXIT_FAILURE);
	}else if(pid > 0){ // Parent process
		wait(&status);
	}

    va_end(args);
	//check if the child process exited normally
	if(WIFEXITED(status)){ 
		if(WEXITSTATUS(status)){
			return false;
		}
	}

    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
	//open the output file for writing, create if doesnt exist and truncate if it does. 
	int fd = open(outputfile, O_RDWR|O_CREAT|O_TRUNC);
	pid_t pid;
	int status;

	if(fd < 0){
		return false;
	}

	pid = fork();
	
	if(pid == 0){ //Child Process
		if(dup2(fd, 1) < 0){
			return false;
		}
		close(fd);

		execv(command[0], command);
		exit(-1);
	}else if(pid > 0){ // Parent Process
		wait(&status);
		close(fd);
	}

    va_end(args);

	if(WIFEXITED(status)){
		if(WEXITSTATUS(status)){
			return false;
		}
	}

    return true;
}
