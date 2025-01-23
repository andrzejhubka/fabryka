#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <utilities.h>
#include <warehouse.h>
#include <signal.h>

bool machine_running = true;

void machine_stop(int signal)
{
    machine_running = false;

    // jesli maszyna czekala na produkty to ja obudz!
}

void machine(int speed)
{
    // handler dla sygnalu wylaczenia
    signal(SIGUSR1, machine_stop);

    // klucz do mechanizmow ipc
    key_t key_ipc = ftok("/tmp", 32);

    // klucz do zbioru semaforow
    int sem_id = utils::get_semid(key_ipc);

    // api do zarzadzania magazynem
    warehouse::WarehouseManager magazyn = warehouse::WarehouseManager(key_ipc, sem_id);

    // pojemniki do przechowywania pobranych produktow
    auto containter_x = utils::ProductX();
    auto containter_y = utils::ProductY();
    auto containter_z = utils::ProductZ();

    // glowna petla
    while (machine_running)
    {
        // pobierz produkty
        magazyn.grab_x(&containter_x);
        magazyn.grab_y(&containter_y);
        magazyn.grab_z(&containter_z);

        // symbolizuje produkcje
        usleep(speed*100);

        // pokaz wynik na ekranie
        std::cout<<"Wyprodukowano:" <<containter_x.m_weight+containter_y.m_weight+containter_z.m_weight<<" jako laczna waga."<<std::endl;;
    }
}









