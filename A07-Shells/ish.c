/* Team members: Emily Tomz and David D'Attile
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <unistd.h>

/* Given a newline-terminated string of input, produce an array of the
   space-separated words from that input.

   The input string is modified in place, replacing each space character with a
   null (as well as the final newline).

   The returned array will be null terminated, i.e., suitable for execve.

   Sets num_words to number of words (array is actually one longer, for NULL
   termination).
*/
char **parse_line(char *line, ssize_t len, int *num_words) {
  assert(line);
  assert(len > 0);
  assert(line[len - 1] == '\n');

  // Count words (no escaping or anything fancy)
  *num_words = 1;
  for (int i = 0; i < len; i += 1) {
    if (line[i] == ' ') {
      *num_words += 1;
    }
  }

  // Allocate entries for each word, plus an extra one for NULL termination
  char **words = (char **)malloc(sizeof(char *) * (*num_words + 1));

  char *current_word = line;
  int word_index = 0;
  for (int i = 0; i < len; i += 1) {

    if (line[i] == ' ' || line[i] == '\n') {
      // Save the current word, null terminated
      line[i] = '\0';
      words[word_index] = current_word;

      word_index += 1;
      current_word = line + i + 1;
    }
  }

  words[*num_words] = NULL;

  return words;
}

typedef struct job {
  char *command;
  pid_t pid;
  int status;
  struct job *next;
} job_t;

job_t *jobs = NULL;

void add_job(char *command, pid_t pid) {
  job_t *j = (job_t *)malloc(sizeof(job_t));

  j->command = command;
  j->pid = pid;
  j->status = 0;
  j->next = jobs;

  jobs = j;
}

void free_job(job_t *j) {
  free(j->command);
  free(j);
}

/* Check on jobs

    Options will be used to waitpid to, e.g., determine blocking with WNOHANG.
*/
void check_jobs(int options) {
  // TODO #4: check each job in turn, outputing completion/status
  // set prev and curr jobs array temp
  job_t *prev = NULL;
  job_t *j = jobs;
  while (j != NULL) {
    // save job status and wait
    pid_t pid = waitpid(j->pid, &(j->status), options);

    // process completed
    if (pid > 0) {
      // report exit status
      if (WEXITSTATUS(j->status) == 0) { // print successful job
        fprintf(stderr, "job '%s' complete\n", j->command);
      } else { // print unsuccessful
        fprintf(stderr, "job '%s' status %d\n", j->command, WEXITSTATUS(j->status));
      }

      // update linked list (two cases)
      if (prev == NULL) {
        // move onto next
        jobs = j->next;
        free_job(j);
        j = jobs;
      } else {
        prev->next = j->next;
        free_job(j);
        j = prev->next;
      }
    } else if (options == WNOHANG) { // process running
      fprintf(stderr, "job '%s' still running\n", j->command);
      prev = j;
      j = j->next;
    }
  }
}

// TODO #6: Add got_sigchld variable and handle_sigchld function
int got_sigchld = 0;
void handle_sigchld(int updatedVal) {
  got_sigchld = 1;
}

/* Sets up signal handlers */
void init() {
  struct sigaction action;

  // // Ignore Ctrl-C
  action.sa_flags = 0;
  action.sa_handler = SIG_IGN;
  sigaction(SIGINT, &action, NULL);

  // TODO #6: handle SIGCHLD  
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_RESTART;
  action.sa_handler = handle_sigchld;
  sigaction(SIGCHLD, &action, NULL);
}

int main(int argc, char *argv[]) {
  char *buf = NULL;
  size_t linecap = 0;

  init();

  while (1) {

    // TODO #5: check on jobs, update the user
    // TODO #6: only update the user when something has changed
    if (got_sigchld != 0) {
      check_jobs(WNOHANG);
      got_sigchld = 0;
    }
    
    fprintf(stderr, "¢ ");
    errno = 0;
    ssize_t len = getline(&buf, &linecap, stdin);

    if (len == -1) {
      if (!errno) {
        // TODO #4: wait for all jobs to finish
        if (jobs != NULL) {
          // print jobs running
          fprintf(stderr, "\nJobs are still running...\n");

          // check running jobs
          int options = 0;
          check_jobs(options);
        }

        fprintf(stderr, "\nGoodbye!\n");
        return 0;
      }

      perror("I/O error reading line");
      return 1;
    }

    if (len > 1) {
      int bg = 0;
      char *command = NULL;

      // See if we're a background task; munge line
      if (buf[len - 2] == '&') {
        int last_non_space = len - 3;
        for (; last_non_space > 0; last_non_space -= 1) {
          if (buf[last_non_space] != ' ') {
            break;
          }
        }
        buf[last_non_space + 1] = '\n';
        buf[last_non_space + 2] = '\0';
        len = last_non_space + 2; // exclude null!

        bg = 1;

        // Save command name, but drop newline
        command = (char *)malloc(sizeof(char) * len);
        strncpy(command, buf, len - 1);
        command[len - 1] = '\0';
      }

      int num_words = 0;
      char **args = parse_line(buf, len, &num_words);
      assert(args);
      assert(args[num_words] == NULL);

      // TODO #1: run the command
      pid_t pid = fork();
      if (pid == 0) { // child
        // set args and env
        char *child_proc_envp[] = { NULL };

        // execute passed program
        execve(args[0], args, child_proc_envp);

        // if we reach here; error
        perror("ish: command error"); // should this still happen?
        return 1;
      } 

      // parent
      // TODO #3: run background jobs in the background
      if (bg) {
        add_job(command, pid);
      } else {
        // set child status store location
        int child_status;
        int options = 0;

        // wait for child execute
        // in parent process pid = child pid
        waitpid(pid, &child_status, options);

        // TODO #2: print the status if it's non-zero
        // if exited with failure (status nonzero)
        if (WIFEXITED(child_status) ==  0) {
          // print status code
          fprintf(stderr, "ish: status %d\n", WEXITSTATUS(child_status));
        }
      }

      free(args);
    }
  }

  return 0;
}
