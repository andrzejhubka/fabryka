#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utilities.h"
#include "machine.h"
#include "supplier.cpp"
#include "director.h"

int main(int argc, char *argv[])
{
  // Inicjacja IPC;
  {
    long capacity = std::stoi(argv[1]);
    if (capacity % 6 != 0)
    {
      capacity = (capacity / 6) * 6;
    }
    key_t key_ipc = ftok("/tmp", 32 );
    //TODO OBSLUGA BLEDU
    int semid = utils::utworz_zbior_semaforow(key_ipc, 12);
    //TODO OBSLUGA BLEDU
    long segment_size = capacity*UNIT_SIZE + sizeof(warehouse::warehouse_data);
    int memid = utils::utworz_segment_pamieci_dzielonej(key_ipc, segment_size);
    //TODO OBSLUGA BLEDU
    auto warehouse = warehouse::WarehouseManager(key_ipc, semid);
    warehouse.initiailze(capacity);
    utils::semafor_set(semid, sem_wareohuse_working, 1);
  }

  // PROCESY DOSTAWCY
  pid_t pid_x = fork();
  if (pid_x == 0)
  {
    supplier<utils::ProductX>(speed_supply_x);

    std::exit(0);
  }
  pid_t pid_y = fork();
  if (pid_y == 0)
  {
    supplier<utils::ProductY>(speed_supply_y);

    std::exit(0);
  }
  pid_t pid_z = fork();
  if (pid_z == 0)
  {
    supplier<utils::ProductZ>(speed_supply_z);

    std::exit(0);
  }

  // PROCESY MASZYN
  pid_t pid_a = fork();
  if (pid_a == 0)
  {
    machine(speed_machine_a);
    std::exit(0);
  }

  pid_t pid_b = fork();
  if (pid_b == 0)
  {
    machine(speed_machine_b);
    std::exit(0);
  }

  // PROCES DYREKTORA
  pid_t pid_d = fork();
  if (pid_d == 0)
  {
    director(pid_x, pid_y, pid_z, pid_a, pid_b);
    std::exit(0);
  }



  // czekamj na procesy potomne
  waitpid(pid_x, NULL, 0);
  waitpid(pid_y, NULL, 0);
  waitpid(pid_z, NULL, 0);
  waitpid(pid_a, NULL, 0);
  waitpid(pid_b, NULL, 0);
  waitpid(pid_d, NULL, 0);

  exit(0);
}

