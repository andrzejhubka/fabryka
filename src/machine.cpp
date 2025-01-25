#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <utilities.h>
#include <warehouse.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "config.h"
#include <sys/prctl.h>

bool machine_running = true;

static key_t key_ipc;
static int sem_id;

void machine_stop(int signal)
{
    machine_running = false;
    // jesli maszyna czek1ala na produkty to ja obudz!
    warehouse::WarehouseManager::wakeup_machines(sem_id);
}

void machine(int speed)
{
    // handler dla sygnalu wylaczenia
    signal(SIGUSR1, machine_stop);

    // w przypadku smierci rodzica, tez zakoncz prace.
    prctl(PR_SET_PDEATHSIG, SIGTERM);

    // klucz do mechanizmow ipc
    key_ipc = ftok("/tmp", 32);
    utils::detect_issue(key_ipc==IPC_RESULT_ERROR, "Blad generowania ftok");

    // klucz do zbioru semaforow
    sem_id = semget(key_ipc, 0, 0);
    utils::detect_issue(sem_id==IPC_RESULT_ERROR, "Blad pobierania id zbioru semaforow");

    // infromacja ze przynajmniej jedna maszyna dziala
    utils::semafor_set(sem_id, sem_factory_working, 1);

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
        usleep(speed*1000);

        // pokaz wynik na ekranie
        std::cout<<"\033[33m"<<"MASZYNA: przetworzono produkty. Waga wyrobu: " <<containter_x.m_weight+containter_y.m_weight+containter_z.m_weight<<"\033[0m"<<std::endl;;
    }

    std::cout<<"Maszyna zakonczyla produkcje. "<<std::endl;
    exit(EXIT_SUCCESS);
}









