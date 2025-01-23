#include <thread>
#include <sys/ipc.h>
#include "utilities.h"
#include "warehouse.h"
#include<unistd.h>

static bool supplier_running = true;

void supplier_stop(int signal)
{
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

    // api do magazuni
    auto warehouse = warehouse::WarehouseManager(key_ipc, sem_id);

    // pojemnik na odbierane produkty
    Product towar;

    // glowna petla
    while(supplier_running)
    {
        usleep(speed*100000);
        towar.recreate();
        std::cout <<"TOWAR WAGA:"<< towar.m_weight << std::endl;
        warehouse.insert(&towar);
    }
}

void supplier_stop()
{
    supplier_running = false;
    // jesli dostawca czekal na wolne miejsce to go obudz!
}


