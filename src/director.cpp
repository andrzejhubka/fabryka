#include <iostream>
#include "director.h"
#include <cstring>
#include "utilities.h"
#include "warehouse.h"
#include <unistd.h>
#include <cstdlib>
#include <signal.h>

bool director_run = true;

void mainloop();

int main(int argc, char *argv[])
{
    // -------------------------- WERYFIKACJA WPROWADZONEJ POJEMNOSCI MAGAZYNU
    if (argc != 2)
    {
        std::cerr << "Uzycie: " << argv[0] << " <pojemnosc magazynu>" << std::endl;
        return 1;
    }
    long capacity = std::stoi(argv[1]); // Konwersja na int
    if (capacity <= 0)
    {
        std::cerr << "Pojemnosc nie moze byc ujemna!";
        return 1;
    }

    // sprawdzamy limity pamieci ram w komupterze i ustawiamy uzywane przez segment pamieci
    long pages = sysconf(_SC_PHYS_PAGES); // strony pamieci
    long page_size = sysconf(_SC_PAGE_SIZE); // Rozmiar strony w bajtach
    long total_memory = pages * page_size; // Całkowita pamięć RAM w bajtach
    int max_capacity = total_memory * MAX_SHARED_RAM ;

    if (capacity > max_capacity)
    {
        std::cerr << "Magazyn jest zbyt duzy. Dokup ram albo zwieksz limity w kodzie. Obecne maksimum:"<<max_capacity<<" b"<<std::endl;
        return 1;
    }

    // ilosc jednostek pojemnosci magazynu musi byc podzielna przez 6 ;
    if (capacity % 6 != 0)
    {
        capacity = (capacity / 6) * 6;
    }
    long segment_size = capacity*UNIT_SIZE + sizeof(warehouse::warehouse_data);

    // -------------------------- OBSLUGA MECHANIZMOW IPC
    key_t key_ipc = ftok("/tmp", 32 );
    //TODO OBSLUGA BLEDU
    int semid = utils::utworz_zbior_semaforow(key_ipc, 12);
    //TODO OBSLUGA BLEDU
    int memid = utils::utworz_segment_pamieci_dzielonej(key_ipc, segment_size);
    //TODO OBSLUGA BLEDU
    auto warehouse = warehouse::WarehouseManager(key_ipc, semid);
    warehouse.initiailze(capacity);


    // glowna petla
    int wybor;
    while (director_run)
    {
        std::cout << "Wybierz polecenie dyrektora:" << std::endl;
        std::cout << "1. Zatrzymaj magazyn" << std::endl;
        std::cout << "2. Zatrzymaj fabryke" << std::endl;
        std::cout << "3. Zatrzymaj magazyn i fabryke. Zapisz stan magazynu i zakoncz symulacje" << std::endl;
        std::cout << "4. Zatrzymaj fabryke i nie zapamietuj stanu magazynu i zakoncz symulacje" << std::endl;
        std::cout << "5. Uruchom tryb monitorowania magazynu" << std::endl;
        std::cout << "6. Zakoncz prace dyrektora" << std::endl;
        std::cout << "Wprowadz polecenie: ";

        std::cin >> wybor;
        std::string command;

        switch (wybor)
        {
            case COMMAND_STOP_WAREHOUSE:
            {
                warehouse.close(false);

                break;
            }
            case COMMAND_STOP_FACTORY:
            {
                kill(9, 9);
                break;
            }
            case COMMAND_STOP_WAREHOUSE_FACTORY_AND_SAVE:
            {
                warehouse.close(true);
                director_run = false;
                break;
            }
            case COMMAND_STOP_WAREHOUSE_FACTORY_NO_SAVE:
            {
                warehouse.close(false);
                director_run = false;
                break;
            }
            case 5:
            {
                warehouse.info();
                break;
            }
            case 6:
            {
                director_run = false;
                break;
            }
            default:
            {
                std::cout << "[DIRECTOR] Invalid command. Please try again." << std::endl;
                continue;
            }
        }
    }

    utils::usun_zbior_semaforow(semid);
    utils::ustaw_do_usuniecia_segment(memid);
}







