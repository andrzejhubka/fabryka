#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utilities.h"
#include "machine.h"
#include "supplier.cpp"


int main(int argc, char *argv[])
{
  // PROCESY DOSTAWCY
  pid_t pid1 = fork();
  if (pid1 == 0)
  {
    supplier<utils::ProductX>(800);

    std::exit(0);
  }
  pid_t pid2 = fork();
  if (pid2 == 0)
  {
    supplier<utils::ProductY>(800);

    std::exit(0);
  }
  pid_t pid3 = fork();
  if (pid3 == 0)
  {
    supplier<utils::ProductZ>(800);

    std::exit(0);
  }

  // PROCESY MASZYN
  pid_t pid4 = fork();
  if (pid4 == 0)
  {
    machine(7000);
    std::exit(0);
  }

  pid_t pid5 = fork();
  if (pid5 == 0)
  {
    machine(3000);
    std::exit(0);
  }


  // czekamj na procesy potomne
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
  waitpid(pid3, NULL, 0);
  waitpid(pid4, NULL, 0);
  waitpid(pid5, NULL, 0);


  exit(0);
}

