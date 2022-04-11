#include <stdio.h>
#include <unistd.h>

int main() {
  char parent_name[] = "PARENT";
  char child_name[] = "CHILD";

  char *proc_name = parent_name;

  printf("%s: before fork\n", proc_name);

  pid_t pid = fork();
  if (pid == 0) {
    proc_name = child_name;
    printf("%s: after fork in block\n", proc_name);
    
    execve(path, argv, envp);
  } else {
    printf("%s: this should be in parent only\n", proc_name);
  }

  printf("%s: after fork\n", proc_name);
  return 0;
}
