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
bool manager_run{true}; // Flaga zatrzymania maszyny B

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
        bool poprawne_pobranie_produktow = true;
        //pobieramy X
        switch (m_magazyn.grab_x(&containter_x))
        {
            case MACHINE_RECIEVED_PRODUCT:
            {
                break;
            }
            case WAREHOUSE_CLOSED:
            {
                machine_a_run = false;
                poprawne_pobranie_produktow = false;
                std::cout<<"Maszyna A: wykryto zamkniecie magazynu. Wylaczam sie" << std::endl;
                break;
            }
            case MACHINE_STOPPED:
            {
                poprawne_pobranie_produktow = false;
                false;
            }
            default:
            {
                break;
            }
        }
        //pobieramy y
        switch (m_magazyn.grab_y(&containter_y))
        {
        case MACHINE_RECIEVED_PRODUCT:
            {
                break;
            }
        case WAREHOUSE_CLOSED:
            {
                machine_a_run = false;
                poprawne_pobranie_produktow = false;
                std::cout<<"Maszyna A: wykryto zamkniecie magazynu. Wylaczam sie" << std::endl;
                break;
            }
        case MACHINE_STOPPED:
            {
                machine_a_run = false;
                poprawne_pobranie_produktow = false;
            }
        default:
            {
                break;
            }
        }
        //pobieramy Z
        switch (m_magazyn.grab_z(&containter_z))
        {
        case MACHINE_RECIEVED_PRODUCT:
            {
                break;
            }
        case WAREHOUSE_CLOSED:
            {
                machine_a_run = false;
                std::cout<<"Maszyna A: wykryto zamkniecie magazynu. Wylaczam sie" << std::endl;
                break;
            }
        case MACHINE_STOPPED:
            {
                machine_a_run = false;
                poprawne_pobranie_produktow = false;
            }
        default:
            {
                break;
            }
        }

        // pomijamy produkcje jak nie pobralismy produktow
        if (!poprawne_pobranie_produktow)
        {
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
    while (machine_b_run)
    {
        bool poprawne_pobranie_produktow = true;
        //pobieramy X
        switch (m_magazyn.grab_x(&containter_x))
        {
        case MACHINE_RECIEVED_PRODUCT:
            {
                break;
            }
        case WAREHOUSE_CLOSED:
            {
                machine_b_run = false;
                poprawne_pobranie_produktow = false;
                std::cout<<"Maszyna B: wykryto zamkniecie magazynu. Wylaczam sie" << std::endl;
                break;
            }
        case MACHINE_STOPPED:
            {
                poprawne_pobranie_produktow = false;
                false;
            }
        default:
            {
                break;
            }
        }
        //pobieramy y
        switch (m_magazyn.grab_y(&containter_y))
        {
        case MACHINE_RECIEVED_PRODUCT:
            {
                break;
            }
        case WAREHOUSE_CLOSED:
            {
                machine_b_run = false;
                poprawne_pobranie_produktow = false;
                std::cout<<"Maszyna B: wykryto zamkniecie magazynu. Wylaczam sie" << std::endl;
                break;
            }
        case MACHINE_STOPPED:
            {
                machine_b_run = false;
                poprawne_pobranie_produktow = false;
            }
        default:
            {
                break;
            }
        }
        //pobieramy Z
        switch (m_magazyn.grab_z(&containter_z))
        {
        case MACHINE_RECIEVED_PRODUCT:
            {
                break;
            }
        case WAREHOUSE_CLOSED:
            {
                machine_b_run = false;
                std::cout<<"Maszyna B: wykryto zamkniecie magazynu. Wylaczam sie" << std::endl;
                break;
            }
        case MACHINE_STOPPED:
            {
                machine_b_run = false;
                poprawne_pobranie_produktow = false;
            }
        default:
            {
                break;
            }
        }

        // pomijamy produkcje jak nie pobralismy produktow
        if (!poprawne_pobranie_produktow)
        {
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
    while (manager_run)
    {
        // czekaj az semafor polecenia zostanie zmieniony
        utils::semafor_p(m_sem_id, sem_command, 1);
        utils::semafor_v(m_sem_id, sem_command, 1);

        command = utils::semafor_value(m_sem_id, sem_command);

        switch (command)
        {
            case COMMAND_STOP_WAREHOUSE:
                {
                    std::cout<<"Fabryka: wykryto zamykanie magazynu\n";
                    manager_run = false;
                    break;
                }
            case COMMAND_STOP_FACTORY:
                {
                    std::cout<<"Fabryka: otrzymano polecenie zamkniecia\n";
                    stop_workring();
                    manager_run = false;
                    break;
                }
            case COMMAND_STOP_WAREHOUSE_FACTORY_AND_SAVE:
                {
                    std::cout<<"Fabryka: otrzymano polecenie zamkniecia\n";
                    stop_workring();
                    manager_run = false;
                    break;
                }
            case COMMAND_STOP_WAREHOUSE_FACTORY_NO_SAVE:
                {
                    std::cout<<"Fabryka: otrzymano polecenie zamkniecia\n";
                    manager_run = false;
                    break;
                }
        }
        utils::semafor_set(m_sem_id, sem_command, 0);
    }
    std::cout<<"Manager: wylaczono"<<std::endl;
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
    utils::semafor_v(m_sem_id, sem_dostepne_x, 2);
    utils::semafor_v(m_sem_id, sem_dostepne_y, 2);
    utils::semafor_v(m_sem_id, sem_dostepne_z, 2);

    // rozwaz sytuacje w ktorej magazun sie zapelnia, co robia wtedy dostawcy?

}






