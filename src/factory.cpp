#include "factory.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <utilities.h>
#include <warehouse.h>
#include <atomic>

// Definicja zmiennych atomowych
bool machine_a_run{true}; // Flaga zatrzymania maszyny A
bool machine_b_run{true}; // Flaga zatrzymania maszyny B

int main()
{
    Factory fabryka;
    return 0;
}

Factory::Factory()
{
    std::cout<<"\n=============== Factory: inicjalizacja ==============="<<std::endl;

    // generujemy klucz ipc
    m_key_ipc = ftok("/tmp", 32);

    // probojemy podlaczyc sie do semaforow:
    m_sem_id = utils::get_semid(m_key_ipc);

    // api do zarzadzzania magazynem
    m_magazyn = warehouse::WarehouseManager(m_key_ipc);

    // watki
    manager_THREAD = std::thread(&Factory::thread_manager, this);
    worker_a_THREAD = std::thread(&Factory::thread_worker_a,  this);
    worker_b_THREAD = std::thread(&Factory::thread_worker_b, this);

    std::cout<<"======================= SUKCES =======================\n"<<std::endl;

}

Factory::~Factory()
{
    worker_a_THREAD.join();
    worker_b_THREAD.join();
    manager_THREAD.join();
}


void Factory::thread_worker_a()
{
    // gdy pobierzemy produkty przechowywujemy je tutaj
    auto containter_x = utils::ProductX(0);
    auto containter_y = utils::ProductY(0);
    auto containter_z = utils::ProductZ(0, 0);

    //
    while (machine_a_run)
    {
        //pobieramy X
        if (m_magazyn.grab_x(containter_x) == -1)
        {
            // gdy dostaniesz odmowe/nie uda sie proboj dalej lub sie wylacz
            continue;
        }

        // pobieramy y
        if (m_magazyn.grab_y(containter_y) == -1)
        {
            // gdy dostaniesz odmowe/nie uda sie proboj dalej lub sie wylacz
            continue;
        }

        // pobieramy z
        if (m_magazyn.grab_z(containter_z) == -1)
        {
            // gdy dostaniesz odmowe/nie uda sie proboj dalej lub sie wylacz
            continue;
        }

        // gdy masz juz produkty wyprodokuj cos i napisz na konsoli
        sleep(speed_machine_a);
        std::cout<<"Maszyna A: Wyprodukowano produkt z x, y, z. Wazy: "<<containter_x.m_weight+containter_y.m_weight+containter_z.m_weight<<" kg.\n";
    }

    std::cout<<"Maszyna A: zakonczono produkcje\n";

}

void Factory::thread_worker_b()
{
    // gdy pobierzemy produkty przechowywujemy je tutaj
    auto containter_x = utils::ProductX(0);
    auto containter_y = utils::ProductY(0);
    auto containter_z = utils::ProductZ(0, 0);
    //
    while (machine_a_run)
    {
        //pobieramy X
        if (m_magazyn.grab_x(containter_x) == -1)
        {
            // gdy dostaniesz odmowe/nie uda sie proboj dalej lub sie wylacz
            continue;
        }

        // pobieramy y
        if (m_magazyn.grab_y(containter_y) == -1)
        {
            // gdy dostaniesz odmowe/nie uda sie proboj dalej lub sie wylacz
            continue;
        }
        // pobieramy z
        if (m_magazyn.grab_z(containter_z) == -1)
        {
            // gdy dostaniesz odmowe/nie uda sie proboj dalej lub sie wylacz
            continue;
        }

        // gdy masz juz produkty wyprodokuj cos i napisz na konsoli
        sleep(speed_machine_b);
        std::cout<<"Maszyna B: Wyprodukowano produkt z x, y, z. Wazy: "<<containter_x.m_weight+containter_y.m_weight+containter_z.m_weight<<" kg.\n";
    }

    std::cout<<"Maszyna A: zakonczono produkcje\n";

}
void Factory::thread_manager()
{
    int command;
    while (true)
    {
        // czekaj az semafor polecenia zostanie zmieniony
        utils::semafor_p(m_sem_id, sem_command, 1);

        command = utils::semafor_value(m_sem_id, sem_command);

        switch (command)
        {
            case 0:
                {
                    std::cout<<"Fabryka: otrzymano polecenie 1\n";
                    break;
                }
            case 1:
                {
                    std::cout<<"Fabryka: otrzymano polecenie 2\n";
                    stop_workring();
                    break;
                }
            case 2:
                {
                    std::cout<<"Fabryka: otrzymano polecenie 3\n";
                    stop_workring(); // KOLEJNOSC JEST WAZNA! gdybym zapisal magazyn do pliku i maszyna zdazyla pobrac produkt, po odczytaiu stanu magazynu od nowa ten produkt by sie zdublowal
                    break;
                }
            case 3:
                {
                    std::cout<<"Fabryka: otrzymano polecenie 4\n";
                    break;
                }
        }
        utils::semafor_set(m_sem_id, sem_command, 0);
    }
}

void Factory::stop_workring()
{
    std::cout<<"Fabryka: proba zakonczenia pracy maszyn\n";
    // maszyny moga czekac na surowce!! trzeba je zwolnic
    machine_a_run = false;
    machine_b_run = false;

    // obudz te maszyny ktore czekaja na produkt
}






