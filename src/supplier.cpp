#include <thread>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include "utilities.h"
#include "config.h"
#include "warehouse.h"



static bool supplier_running = true;

static void supplier_stop(int signal)
{
    std::cout<<"DOSTAWCA: Proba wylaczenia dostawcy o pid: "<<getpid()<<std::endl;
    supplier_running = false;
    // jesli maszyna czekala na produkty to ja obudz!
}

template<typename Product>
void supplier(int speed)
{
    // handler dla sygnalu wylaczenia
    signal(SIGUSR1, supplier_stop);

    // klcuz do ipc
    key_t key_ipc = ftok("/tmp", 32);

    // pobierz zbior semaforow
    int sem_id = utils::get_semid(key_ipc);

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
}



