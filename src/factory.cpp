#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "utilities.h"
#include "machine.h"
#include "supplier.cpp"
#include "director.h"

void cleanup(int signal);

int main(int argc, char *argv[])
{
  // Inicjacja IPC;
  {
    long capacity_units = utils::calculate_capacity_units(argc, argv);
    utils::detect_issue(capacity_units==INVALID_USER_INPUT, "Nieprawidlowa pojemnosc magazynu.");

    // klucz
    key_t key_ipc = ftok("/tmp", 32 );
    utils::detect_issue(key_ipc==IPC_RESULT_ERROR, "Blad przy tworzeniu klucza ftok");

    // zbior semaforow
    int semid = semget(key_ipc, 12, IPC_CREAT | 0600);
    utils::detect_issue(semid==IPC_RESULT_ERROR, "Blad przy tworzeniu zbioru semaforow");

    // pamiec wspoldzielona
    long segment_size = capacity_units*UNIT_SIZE + sizeof(warehouse::warehouse_data);
    int memid =  shmget(key_ipc, segment_size, IPC_CREAT | 0600);
    utils::detect_issue(memid==IPC_RESULT_ERROR, "Blad przy tworzeniu segmentu pamieci");

    // api magazynu
    auto warehouse = warehouse::WarehouseManager(key_ipc, semid);
    warehouse.initiailze(capacity_units);
    utils::semafor_set(semid, sem_wareohuse_working, 1);

    // po zainicjowaniu, nie ma sensu kopiowac tych obiektow do nowycnh procesow
  }

  // PROCESY DOSTAWCY
  pid_t pid_x = fork();
  if (pid_x == 0){
    supplier<utils::ProductX>(speed_supply_x);
    std::exit(0);
  }
  utils::detect_issue(pid_x==FORK_RESULT_ERROR, "Blad operacji fork");

  pid_t pid_y = fork();
  if (pid_y == 0){
    supplier<utils::ProductY>(speed_supply_y);

    std::exit(0);
  }
  utils::detect_issue(pid_y==FORK_RESULT_ERROR, "Blad operacji fork");

  pid_t pid_z = fork();
  if (pid_z == 0){
    supplier<utils::ProductZ>(speed_supply_z);

    std::exit(0);
  }
  utils::detect_issue(pid_z==FORK_RESULT_ERROR, "Blad operacji fork");

  // PROCESY MASZYN
  pid_t pid_a = fork();
  if (pid_a == 0){
    machine(speed_machine_a);
    std::exit(0);
  }
  utils::detect_issue(pid_a==FORK_RESULT_ERROR, "Blad operacji fork");

  pid_t pid_b = fork();
  if (pid_b == 0){
    machine(speed_machine_b);
    std::exit(0);
  }
  utils::detect_issue(pid_b==FORK_RESULT_ERROR, "Blad operacji fork");

  // PROCES DYREKTORA
  pid_t pid_d = fork();
  if (pid_d == 0){
    director(pid_x, pid_y, pid_z, pid_a, pid_b);
    std::exit(0);
  }
  utils::detect_issue(pid_d==FORK_RESULT_ERROR, "Blad operacji fork");

  // obsluga sygnalow pozwalajaca na zwolnienie zasobow
  utils::detect_issue(signal(SIGTERM, cleanup)==SIG_ERR, "Blad ustawiania handlera syngalu");
  utils::detect_issue(signal(SIGINT, cleanup)==SIG_ERR, "Blad ustawiania handlera syngalu");

  // czekanie na procesy potomne
  utils::detect_issue(waitpid(pid_x, NULL, 0)==WAIT_ERROR, "Blad czekania na pid_x");
  utils::detect_issue(waitpid(pid_y, NULL, 0)==WAIT_ERROR, "Blad czekania na pid_y");
  utils::detect_issue(waitpid(pid_z, NULL, 0)==WAIT_ERROR, "Blad czekania na pid_z");
  utils::detect_issue(waitpid(pid_a, NULL, 0)==WAIT_ERROR, "Blad czekania na pid_a");
  utils::detect_issue(waitpid(pid_b, NULL, 0)==WAIT_ERROR, "Blad czekania na pid_b");
  utils::detect_issue(waitpid(pid_d, NULL, 0)==WAIT_ERROR, "Blad czekania na pid_d");

  // usun wszystkie mechanizmy ipc
  cleanup(0);
}

void cleanup(int signal)
{
  key_t key_ipc = ftok("/tmp", 32);
  utils::detect_issue(key_ipc==IPC_RESULT_ERROR, "SPRZATANIE: Blad przy tworzeniu klucza ftok");
  int semid = semget(key_ipc, 0, 0);
  //utils::detect_issue(semid==IPC_RESULT_ERROR, "SPRZATANIE: Blad przy pobieraniu id semaforow");

  int memid = shmget(key_ipc, 0, 0);
  utils::detect_issue(memid==IPC_RESULT_ERROR, "SPRZATANIE: Blad przy pobieraniu id pamieci");

  int result1 = semctl(semid, 0, IPC_RMID);
  utils::detect_issue(result1==IPC_RESULT_ERROR, "SPRZATANIE: Blad przy usuwaniu zbioru semaforow");

  int result2 = shmctl(memid, IPC_RMID, NULL);
  utils::detect_issue(result2==IPC_RESULT_ERROR, "SPRZATANIE: Blad przy usuwaniu segmentu pameci");

  exit(0);
}


