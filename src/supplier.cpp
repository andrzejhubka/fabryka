#include <thread>
#include <signal.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/ipc.h>
#include "utilities.h"
#include "config.h"
#include "warehouse.h"
#include <sys/prctl.h>


static bool supplier_running = true;

static key_t key_ipc;
static int sem_id;

static void supplier_stop(int signal)
{

    std::cout<<"DOSTAWCA: Proba wylaczenia dostawcy o pid: "<<getpid()<<std::endl;
    supplier_running = false;

    // jesli maszyna czekala na produkty to ja obudz!

    warehouse::WarehouseManager::wakeup_suppliers(sem_id);
    exit(EXIT_SUCCESS);
}

template<typename Product>
void supplier(int speed)
{
    // handler dla sygnalu wylaczenia
    signal(SIGUSR1, supplier_stop);

    // w przypadku smierci rodzica, tez zakoncz prace.
    prctl(PR_SET_PDEATHSIG, SIGTERM);
    // klucz do mechanizmow ipc
    key_ipc = ftok("/tmp", 32);
    utils::detect_issue(key_ipc==IPC_RESULT_ERROR, "Blad generowania ftok");

    // klucz do zbioru semaforow
    sem_id = semget(key_ipc, 0, 0);
    utils::detect_issue(sem_id==IPC_RESULT_ERROR, "Blad pobierania id zbioru semaforow");

    // api do magazynu
    auto warehouse = warehouse::WarehouseManager(key_ipc, sem_id);

    // pojemnik na odbierane produkty
    Product towar;

    // glowna petla
    while(supplier_running)
    {
        usleep(speed*1000);
        towar.recreate();
        warehouse.insert(&towar);
    }

    exit(EXIT_SUCCESS);
}



