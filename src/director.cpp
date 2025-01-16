#include <iostream>
#include "director.h"
#include <cstring>
#include "utilities.h"
#include "warehouse.h"
#include <unistd.h>


int main(int argc, char *argv[])
{
    // weryfikujemy liczbe argumentow
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

    std::cout << "Max shared Capacity: " << max_capacity << std::endl;
    director zarzadca(capacity);
    return 0;

}

director::director(long warehouse_capacity)
{
    std::cout<<"\n=============== DIRECTOR: inicjalizacja ==============="<<std::endl;
    // generowaie klucza
    if ((m_key_ipc=ftok("/tmp", 32 ))==IPC_RESULT_ERROR)
    {
        std::cerr << "Blad tworzenie zbioru semafgorow" << std::endl;
        exit(-1);
    }

    // zainicjuj zbior semaforow
    if ((m_semid = utils::utworz_zbior_semaforow(m_key_ipc, 10)) == IPC_RESULT_ERROR)
    {
        std::cerr << "Blad tworzenie zbioru semafgorow" << std::endl;
        exit(-1);
    }
    std::cout << "semid: " << m_semid << std::endl;

    // pojemnosc magazynu musi byc podzielna przez 12 ;
    if (warehouse_capacity % 12 != 0)
    {
        warehouse_capacity = (warehouse_capacity / 12) * 12;
    }
    int shelf_capacity = warehouse_capacity / 12;

    // otworz segment pamieciwspoldzielonej dla magazynu
    if  (utils::utworz_segment_pamieci_dzielonej(&m_shared, m_key_ipc, warehouse_capacity) == IPC_RESULT_ERROR)
    {
        std::cerr << "Blad tworzenia segmentu" << std::endl;
        exit(-1);
    }
    std::cout << "pamiec wspoldzielona id: " << m_shared.id << std::endl;

    // dolacz segment
    m_shared.adres = utils::dolacz_segment_pamieci(m_shared.id);

    // zainicjuj dane magazynu
    //memcpy(m_shared)


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
    utils::ustaw_do_usuniecia_segment(&m_shared);
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

