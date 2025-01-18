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
    utils::semafor_set(m_sem_id, sem_factory_working, 1);

    // api do zarzadzzania magazynem
    m_magazyn = warehouse::WarehouseManager(m_key_ipc, m_sem_id);

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
        if (m_magazyn.grab_x(&containter_x) != MACHINE_RECIEVED_PRODUCT)
        {
            // gdy sie nie uda, w sumie nic nie trzeba robic, ale moze w przyszlosci sie to zmieni
            continue;
        }

        // pobieramy y
        if (m_magazyn.grab_y(&containter_y) != MACHINE_RECIEVED_PRODUCT)
        {
            // gdy sie nie uda, w sumie nic nie trzeba robic, ale moze w przyszlosci sie to zmieni
            continue;
        }

        // pobieramy z
        if (m_magazyn.grab_z(&containter_z) != MACHINE_RECIEVED_PRODUCT)
        {
           // gdy sie nie uda, w sumie nic nie trzeba robic, ale moze w przyszlosci sie to zmieni
            continue;
        }

        // ----------------------------------PRACA MASZYNY JAKO LICZENIE W BLOKACH I MOZLIWOSC WCZESNIEJSZEGO SKONCZENIA GDY MASZYNA ZOSTANIE WYLACZONA
        // ps. szansa ze na tym etapie jednoczesnie wyprodukujemy produkt i wylaczymy maszyne jest raczej zerowa, ale dla pewnosci ja obslugujemy
        // idea: maszyna wykonuje bloki obliczen <u mnie dla uproszczenia iteracje petli> i po policzeniu ktoregos z nich sprawdzamy czy dalej mozemy i jak nie to wysadzamy maszyne
        int i=0;
        for (i; i < 100; i++)
        {
            if (machine_a_run)
            {
                usleep(speed_machine_a*10);
                // i cos sobie liczymy
            }
            // jesli jest stop to konczymy wczesniej
            else break;
        }

        // sprawdzamy czy policono wszystkie bloki i mozna oddac produkt
        if (i >= 99)
        {
            std::cout<<"Maszyna A: Wyprodukowano produkt z x, y, z. Wazy: "<<containter_x.m_weight+containter_y.m_weight+containter_z.m_weight<<" kg.\n";
        }
        else
        {
            std::cout<<"Maszyna A: Produkcja przerwana na bloku: "<<i<<". Powod: wylaczenie maszyny ";
        }
    }

    std::cout<<"Maszyna A: wylaczono\n";
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
        if (m_magazyn.grab_x(&containter_x) != MACHINE_RECIEVED_PRODUCT)
        {
            // gdy dostaniesz odmowe/nie uda sie proboj dalej lub sie wylacz
            continue;
        }

        // pobieramy y
        if (m_magazyn.grab_y(&containter_y) != MACHINE_RECIEVED_PRODUCT)
        {
            // gdy dostaniesz odmowe/nie uda sie proboj dalej lub sie wylacz
            continue;
        }
        // pobieramy z
        if (m_magazyn.grab_z(&containter_z) != MACHINE_RECIEVED_PRODUCT)
        {
            // gdy dostaniesz odmowe/nie uda sie proboj dalej lub sie wylacz
            continue;
        }

        // ----------------------------------PRACA MASZYNY JAKO LICZENIE W BLOKACH I MOZLIWOSC WCZESNIEJSZEGO SKONCZENIA GDY MASZYNA ZOSTANIE WYLACZONA
        // ps. szansa ze na tym etapie jednoczesnie wyprodukujemy produkt i wylaczymy maszyne jest raczej zerowa, ale dla pewnosci ja obslugujemy
        // idea: maszyna wykonuje bloki obliczen <u mnie dla uproszczenia iteracje petli> i po policzeniu ktoregos z nich sprawdzamy czy dalej mozemy i jak nie to wysadzamy maszyne
        int i=0;
        for (i; i < 100; i++)
        {
            if (machine_b_run)
            {
                usleep(speed_machine_b*10);
                // i cos sobie liczymy
            }
            // jesli jest stop to konczymy wczesniej
            else break;
        }

        // sprawdzamy czy policono wszystkie bloki i mozna oddac produkt
        if (i >= 99)
        {
            std::cout<<"Maszyna B: Wyprodukowano produkt z x, y, z. Wazy: "<<containter_x.m_weight+containter_y.m_weight+containter_z.m_weight<<" kg.\n";
        }
        else
        {
            std::cout<<"Maszyna B: Produkcja przerwana na bloku: "<<i<<". Powod: wylaczenie maszyny";
        }
    }

    std::cout<<"Maszyna B: wylaczono\n";

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


    // daj znac ze fabryka juz nie dziala
    utils::semafor_v(m_sem_id, sem_factory_working, 1);

    // obudz te maszyny ktore czekaja na produkt
    utils::semafor_v(m_sem_id, sem_dostepne_x, 1);
    utils::semafor_v(m_sem_id, sem_dostepne_x, 1);
    utils::semafor_v(m_sem_id, sem_dostepne_x, 1);

}






