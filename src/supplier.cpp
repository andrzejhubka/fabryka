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
}

template<typename Product>
void supplier(int speed)
{
    // przekierowanie
    if (REDIRECT_LOGS_TO_FILE)
    {
        std::freopen(LOG_PATH, "a", stdout);

    }

    // handler dla sygnalu wylaczenia
    utils::detect_issue(signal(SIGUSR1, supplier_stop)==SIG_ERR, "blad ustawiania sygnalu");

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



