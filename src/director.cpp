#include <iostream>
#include <signal.h>
#include "utilities.h"
#include "config.h"
#include "director.h"
#include "warehouse.h"
#include "supplier.cpp"
bool director_run = true;



void director(int pid_x, int pid_y, int pid_z, int pid_a,int pid_b)
{
    // -------------------------- OBSLUGA MECHANIZMOW IPC
    key_t key_ipc = ftok("/tmp", 32 );
    int semid = utils::get_semid(key_ipc);
    int memid = utils::get_shared_id(key_ipc);
    warehouse::WarehouseManager warehouse(key_ipc, semid);

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
                kill(pid_x, SIGUSR1);
                kill(pid_y, SIGUSR1);
                kill(pid_z, SIGUSR1);
                break;
            }
            case COMMAND_STOP_FACTORY:
            {
                kill(pid_a, SIGUSR1);
                kill(pid_b, SIGUSR1);
                break;
            }
            case COMMAND_STOP_WAREHOUSE_FACTORY_AND_SAVE:
            {
                kill(pid_x, SIGUSR1);
                kill(pid_y, SIGUSR1);
                kill(pid_z, SIGUSR1);
                kill(pid_a, SIGUSR1);
                kill(pid_b, SIGUSR1);
                director_run = false;

                warehouse.close(true);
                break;
            }
            case COMMAND_STOP_WAREHOUSE_FACTORY_NO_SAVE:
            {
                kill(pid_x, SIGUSR1);
                kill(pid_y, SIGUSR1);
                kill(pid_z, SIGUSR1);
                kill(pid_a, SIGUSR1);
                kill(pid_b, SIGUSR1);
                director_run = false;

                warehouse.close(true);
                break;
            }
            case 5:
            {
                warehouse.info();
                break;
            }
            case 6:
            {
                kill(pid_x, SIGUSR1);
                kill(pid_y, SIGUSR1);
                kill(pid_z, SIGUSR1);
                kill(pid_a, SIGUSR1);
                kill(pid_b, SIGUSR1);
                warehouse.close(false);
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







