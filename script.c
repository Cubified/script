/*
 * script.c: general script dispatcher
 *
 * Usage: script [dir]
 *   Where dir is a folder containing init scripts to be run,
 *   such as init.d/
 * 
 * TODO:
 *  - Properly handle overflow (i.e. when more lines are printed than are present onscreen)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>

#include "config.h"

//////////////////////////////
// PREPROCESSOR
//
#define VER "0.0.1"

#define ESC "\x1b["

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

#define IS_NOT_DIR(name) ((name[0] != '.' && name[1] != '\0') && \
                          (name[0] != '.' && name[1] != '.' && name[2] != '\0'))

#define IS_IN_RUNLEVEL(name) (runlevel <= 9 ? name[0] == '0'+runlevel : (name[0]-'0' > 9 || name[0]-'0' < 0))

//////////////////////////////
// ENUMS AND TYPEDEFS
//
enum script_state {
  STATE_RUNNING=-1,
  STATE_SUCCESS=0,
  STATE_FAILURE=1
};

typedef struct child {
  char *name;
  pid_t pid;
  int cur_y;
} child;

//////////////////////////////
// STATIC DEFINITIONS
//
void sighandler(int signo);
void esc_clear();
void esc_set_cursor(int x, int y);
void esc_toggle_cursor(int state);
void ui_init();
void ui_fill(int len, char c, char *color);
void ui_header();
void ui_job(char *title, int state);
void pid_init();
void pid_add(char *name, pid_t pid);
child *pid_find(pid_t pid);
int dir_scan(char *dir);

//////////////////////////////
// GLOBAL VARIABLES
//
int ui_w, ui_h,
    ui_x, ui_y;
int job_count = 0,
    job_count_runlevel = 0,
    job_ind = 0;
volatile sig_atomic_t job_compl = 0;
int n_failures = 0;
double progress;
clock_t start;

child **active_pids;

//////////////////////////////
// SIGNALS
//
void sighandler(int signo){
  pid_t pid;
  int status;
  child *proc;

  if((pid = waitpid(-1, &status, WNOHANG)) != -1){
    if((proc=pid_find(pid)) != NULL){
      job_compl++;
      if(status != STATE_SUCCESS){
        n_failures++;
      }

      ui_header();
      esc_set_cursor(ui_x, proc->cur_y);
      ui_job(proc->name, status!=STATE_SUCCESS);
      esc_set_cursor(ui_x, ui_y);
      
      free(proc->name);
      free(proc);
    }
  }
}

//////////////////////////////
// ESCAPE SEQUENCES
//
void esc_clear(){
  puts(ESC "2J");
}

void esc_set_cursor(int x, int y){
  printf(ESC "%i;%iH", y, x);
}

void esc_toggle_cursor(int state){
  printf(ESC "?25%c", (state == 1 ? 'h' : 'l'));
}

//////////////////////////////
// UI
//
void ui_init(){
  struct winsize s;

  esc_clear();
  esc_toggle_cursor(0);

  ioctl(1, TIOCGWINSZ, &s);
  ui_w = s.ws_col;
  ui_h = s.ws_row;

  ui_x = 0;
  ui_y = 3;

  progress = 0;
}

void ui_fill(int len, char c, char *color){
  int i;
  printf("%s", color);
  for(i=0;i<len;i++){
    printf("%c", c);
  }
  printf(RESET);
}

void ui_header(){
  int new_w,
      prog_len,
      i;

  progress = (double)job_compl/(double)(job_count==0?1:job_count);
  new_w = ui_w-GREETING_LEN-5;
  prog_len = round(new_w*progress);

  esc_set_cursor(0, 0);

  printf(CYAN "<= " GREETING CYAN " ");
  ui_fill(prog_len, '=', GREEN);
  ui_fill(new_w-prog_len, '.', CYAN);
  printf("%s>\n\n" RESET, progress==1?GREEN:CYAN);
}

void ui_job(char *title, int state){
  char *state_text;

  printf("%s ", title);
  ui_fill(ui_w-strlen(title)-8, '.', CYAN);

  switch(state){
    case STATE_RUNNING:
      state_text = "  ";
      ui_y++;
      break;
    case STATE_SUCCESS:
      state_text = GREEN "OK" RESET;
      break;
    case STATE_FAILURE:
      state_text = RED "!!" RESET;
      break;
  }

  printf(BLUE " [ %s " BLUE "]\n" RESET, state_text);
}

//////////////////////////////
// CHILD PROCESSES
//
void pid_init(){
  active_pids = malloc(job_count*sizeof(child*));
}

void pid_add(char *name, pid_t pid){
  active_pids[job_ind] = malloc(sizeof(child));
  active_pids[job_ind]->name = name;
  active_pids[job_ind]->pid = pid;
  active_pids[job_ind++]->cur_y = ui_y;
  job_count_runlevel++;
}

child *pid_find(pid_t pid){
  int i;
  for(i=0;i<job_count;i++){
    if(active_pids[i] != NULL &&
       active_pids[i]->pid == pid){
      return active_pids[i];
    }
  }
  return NULL;
}

int dir_scan(char *dir){
  DIR *dp;
  int fd, i, runlevel = 0;
  pid_t pid;
  char buf[256];
  struct dirent *ep;
  dp = opendir(dir);

  if(dp != NULL){
    start = clock();

    while(ep = readdir(dp)){
      job_count++;
    }
    job_count -= 2;
    closedir(dp);
    pid_init();

    while(runlevel <= 10){
      while(job_compl < job_count_runlevel);

      dp = opendir(dir);
      while(ep = readdir(dp)){
        if(IS_NOT_DIR(ep->d_name) &&
           IS_IN_RUNLEVEL(ep->d_name)){
          if((pid=fork()) == 0){
            sprintf(buf, "%s/%s", dir, ep->d_name);

            fd = open("/dev/null", O_WRONLY);
            dup2(fd, 1);
            dup2(fd, 2);
            close(fd);

            execv(buf, NULL);
          } else {
            pid_add(strdup(ep->d_name), pid);
            ui_job(ep->d_name, STATE_RUNNING);
          }
        }
      }
      closedir(dp);

      runlevel++;
    }

    while(job_compl < job_count);

    free(active_pids);

    esc_toggle_cursor(1);

    printf(CYAN "\nDone in " MAGENTA "%f" CYAN " seconds with %s%i" CYAN " failed job%s.\n" RESET, ((double)(clock()-start))/CLOCKS_PER_SEC, (n_failures==0?GREEN:RED), n_failures, (n_failures==1?"":"s"));

    return 0;
  } else {
    printf(RED "Unable to open directory \"" MAGENTA "%s" RED "\" for reading.\n" RESET, dir);
    return 1;
  }
}

//////////////////////////////
// MAIN
//
int main(int argc, char **argv){
  if(argc < 2){
    printf(CYAN "script: a generic script dispatcher\n" YELLOW "Usage: script [directory with scripts]\n" BLUE "  Runs every executable file in " MAGENTA "[directory]" BLUE ", grouped by runlevel.\n" CYAN "  Runlevel is determined by the first character of each file's name.\n" RESET);
    return 0;
  }

  signal(SIGCHLD, sighandler);

  ui_init();
  ui_header();

  return dir_scan(argv[1]);
}
