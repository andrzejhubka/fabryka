#include <iostream>
#include "director.h"
#include "utilities.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    director zarzadca;
    sleep(500);
    return 0;
}

director::director()
{
    std::cout<<"\n=============== DIRECTOR: inicjalizacja ==============="<<std::endl;
    // generowaie klucza
    key_ipc = ftok("/tmp", 32);
    std::cout << "key_ipc: " << key_ipc << std::endl;
    // zainicjuj zbior semaforow
    if ((semid = utils::utworz_zbior_semaforow(key_ipc, 8)) < 0)
    {
        std::cerr << "Error in key_ipc" << std::endl;
        exit(-1);
    }

    std::cout << "semid: " << semid << std::endl;

    // stworz kolejke komunikatow miedzy fabryka a dostawca
    if ((memid= utils::utworz_kolejke(key_ipc)) < 0)
    {
        std::cerr << "Error in key_ipc" << std::endl;
        exit(-1);
    }

    std::cout << "memid: " << memid << std::endl;
    std::cout<<"======================= SUKCES =======================\n"<<std::endl;

    // zainicjuj semafor do komend:
    utils::semafor_set(semid, sem_command, 0);

    // glowna petla
    main_loop();
}
director::~director()
{
    utils::usun_zbior_semaforow(this->semid);
    utils::usun_kolejke(this->memid);
}

void director::main_loop()
{
    int wybor;
    bool run = true;

    while (run)
    {
        std::cout << "\nWybierz polecenie dyrektora:" << std::endl;
        std::cout << "1. Zatrzymaj magazyn" << std::endl;
        std::cout << "2. Zatrzymaj fabryke" << std::endl;
        std::cout << "3. Zatrzymaj magazyn i fabryke. Zapisz stan magazynu" << std::endl;
        std::cout << "4. Zatrzymaj fabryke i nie zapamietuj stanu magazynu" << std::endl;
        std::cout << "Wprowadz polecenie: ";

        int wybor;
        std::cin >> wybor;
        std::string command;

        switch (wybor)
        {
            case 1:
            {
                command = utils::stop_magazyn;
                utils::semafor_v(memid, sem_command, 1);
                break;
            }
            case 2:
            {

                utils::semafor_v(memid, sem_command, 2);
                break;
            }
            case 3:
            {
                utils::semafor_v(memid, sem_command, 3);
                break;
            }
            case 4:
            {
                utils::semafor_v(memid, sem_command, 4);
                break;
            }
            default:
            {
                std::cout << "[DIRECTOR] Invalid command. Please try again." << std::endl;
                continue;
            }
        }
    }

}
