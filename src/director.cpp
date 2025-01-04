#include <iostream>
#include "director.h"
#include "utilities.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    director zarzadca;
    return 0;
}

director::director()
{
    std::cout<<"\n=============== DIRECTOR: inicjalizacja ==============="<<std::endl;
    // generowaie klucza
    m_key_ipc = ftok("/tmp", 32);
    std::cout << "key_ipc: " << m_key_ipc << std::endl;
    // zainicjuj zbior semaforow
    if ((m_semid = utils::utworz_zbior_semaforow(m_key_ipc, 8)) < 0)
    {
        std::cerr << "Error in key_ipc" << std::endl;
        exit(-1);
    }

    std::cout << "semid: " << m_semid << std::endl;

    // stworz kolejke komunikatow miedzy fabryka a dostawca
    if ((m_memid= utils::utworz_kolejke(m_key_ipc)) < 0)
    {
        std::cerr << "Error in key_ipc" << std::endl;
        exit(-1);
    }

    std::cout << "memid: " << m_memid << std::endl;
    std::cout<<"======================= SUKCES =======================\n"<<std::endl;

    // zainicjuj semafor do komend:
    utils::semafor_set(m_semid, sem_command, 0);

    // rozpoczecie pracy

    // glowna petla
    main_loop();
}
director::~director()
{
    utils::usun_zbior_semaforow(this->m_semid);
    utils::usun_kolejke(this->m_memid);
}

void director::main_loop()
{
    int wybor;
    while (m_run)
    {
        std::cout << "\nWybierz polecenie dyrektora:" << std::endl;
        std::cout << "1. Zatrzymaj magazyn" << std::endl;
        std::cout << "2. Zatrzymaj fabryke" << std::endl;
        std::cout << "3. Zatrzymaj magazyn i fabryke. Zapisz stan magazynu" << std::endl;
        std::cout << "4. Zatrzymaj fabryke i nie zapamietuj stanu magazynu" << std::endl;
        std::cout << "5. Zakoncz prace dyrektora" << std::endl;
        std::cout << "Wprowadz polecenie: ";

        int wybor;
        std::cin >> wybor;
        std::string command;

        switch (wybor)
        {
            case 1:
            {
                command = utils::stop_magazyn;
                utils::semafor_v(m_memid, sem_command, 1);
                break;
            }
            case 2:
            {

                utils::semafor_v(m_memid, sem_command, 2);
                break;
            }
            case 3:
            {
                utils::semafor_v(m_memid, sem_command, 3);
                break;
            }
            case 4:
            {
                utils::semafor_v(m_memid, sem_command, 4);
                break;
            }
            case 5:
            {
                m_run = false;
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

