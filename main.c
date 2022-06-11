/***************************************************************************
                              Simple Shell
* Names:
* Aya Abdelaziz Mohamed Abdelaziz  (ID: 7)
* Ahmed Adel Sultan saad           (ID: 24)
* Omar Samy Megahed Ahmed          (ID: 135)
* Amr Sherif Elsayed Ghazy         (ID: 144)
* Fatma Amr Abdul-Mo'men           (ID: 151)
***************************************************************************
This is a simple program that preforms linux terminal commands.
***************************************************************************

/*****************************************************************************
*                              INCLUDES
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*****************************************************************************
                               DEFINES
****************************************************************************/
#define MAX_CHAR 100
#define MAX_WORD 10
#define SIZE 128
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

/*****************************************************************************
                               PROTOTYPES
****************************************************************************/
void print_prompt();
void read_line(char line[]);
int read_parse_line(char *args[], char line[]);
void remove_end_line(char line[]);
void parse_line(char *args[], char line[]);
void execute(char *command, char *args[]);
int is_background(char *args[]);
char *remove_leading_spaces(char *str);
void signal_handler(int sig);

/*****************************************************************************
                                MAIN FUNCTION
****************************************************************************/
int main()
{
  char *args[MAX_WORD];
  char line[MAX_CHAR];
  signal(SIGCHLD, signal_handler);
  // Keep taking user commands until he writes exit or press ctrl + D.
  while (read_parse_line(args, line))
  {
    if (!strcmp(args[0], "cd"))
    {
      chdir(args[1]);
    }
    else
    {
      execute(args[0], args);
    }
  }
}
/*****************************************************************************
                               FUNCTIONS
****************************************************************************/
void remove_end_line(char line[])
{
  // Remove \n from the line.
  int index_end_line = strlen(line);
  line[index_end_line - 1] = '\0';
}

void read_line(char line[])
{

  char *ret;
  do
  {
    // Print the prompt.
    print_prompt();
    ret = fgets(line, MAX_CHAR, stdin);
    // Read the line and store its address.
    remove_end_line(line);
    line = remove_leading_spaces(line);
  } while (!strlen(line));
  // Handle exit processing.
  if (!strcmp(line, "exit") || ret == NULL)
  {
    exit(0);
  }
}

void parse_line(char *args[], char line[])
{
  int index_args = 0;

  // Store the first command in args[0].
  args[index_args] = strtok(line, " ");
  //printf("args is %s\n", *args);

  // Check for other arguments in the line.
  while (args[index_args] != NULL)
  {
    index_args++;
    args[index_args] = strtok(NULL, " ");
  }
}

int read_parse_line(char *args[], char line[])
{
  // Read the line.
  read_line(line);

  // Parse the line.
  parse_line(args, line);

  return 1;
}

void execute(char *command, char *args[])
{
  int background = is_background(args);
  // Make a new process.
  int child_pid = fork();
  if (child_pid == 0)
  {
    // In the child process execute the entered command.
    if (execvp(args[0], args) == -1)
    {
      printf(ANSI_COLOR_RED "Command not found.\n");
    }
  }
  else
  {
    // In the parent process wait till the child process is finished.
    if (!background)
    {
      waitpid(child_pid, NULL, 0);
    }
  }
}

int is_background(char *args[])
{
  int background = 0;
  int argsLength = 0;
  while (args[argsLength] != NULL)
  {
    argsLength++;
  }
  if (strcmp(args[argsLength - 1], "&") == 0)
  {
    background = 1;
  }
  return background;
}

void print_prompt()
{
  char user[SIZE];
  char host[SIZE];
  char cwd[SIZE];
  cuserid(user);
  gethostname(host, SIZE);
  printf(ANSI_COLOR_GREEN "%s@%s:" ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET "$",
         user, host, getcwd(cwd, sizeof(cwd)));
}

char *remove_leading_spaces(char *str)
{
  static char new_str[MAX_CHAR];
  int count = 0, j, k;

  // Iterate until last leading space characterز
  while (str[count] == ' ')
  {
    count++;
  }

  // Removing leading white spaces.
  for (j = count, k = 0;
       str[j] != '\0'; j++, k++)
  {
    new_str[k] = str[j];
  }
  new_str[k] = '\0';
  return new_str;
}

void signal_handler(int sig)
{
  // Declare the file pointer.
  FILE *log_file;
  // Open file.
  log_file = fopen("log_file.txt", "a");
  if (log_file == NULL)
  {
    return;
  }
  // A line is appended in the log file after child process termination.
  fputs("Child process was terminated. \n", log_file);
  // Close file.
  fclose(log_file);
}