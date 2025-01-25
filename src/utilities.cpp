#include "utilities.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fstream>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>
#include <random>
#include <sys/msg.h>
#include <cstring>
#include <director.h>
#include <sys/shm.h>
#include "config.h"


namespace utils
{
    void detect_issue(int condition, const char* message)
    {
        if (condition)
        {
            perror(message);
            exit(EXIT_FAILURE); // Zakończenie programu
        }
    }
    long calculate_capacity_units(int argc, char* argv[])
    {
        if (argc < 2)
        {
            errno = EINVAL;
            utils::detect_issue(true, "Obliczanie jednostek pojemnosci");
        }


        long capacity;
        try
        {
            capacity = std::stol(argv[1]);
        }
        catch (const std::invalid_argument& e)
        {
            errno = EINVAL;
            utils::detect_issue(true, "Argument pojemności nie jest liczbą całkowitą");
        }
        catch (const std::out_of_range& e)
        {
            errno = ERANGE;
            utils::detect_issue(true, "Argument pojemności jest poza zakresem");
        }


        // liczba jednostek musi byc powyzej 6
        if (capacity < 6)
        {
            errno = ERANGE;
            utils::detect_issue(true, "Pojemnosc magazynu musi byc wieksza niz 6");
        }

        // jednostki pojemnosci magazynu -> musza byc podzielne przez 6
        if (capacity % 6 != 0)
        {
            capacity = (capacity / 6) * 6;
        }

        // sprawdzamy limity pamieci ram w komupterze i ustawiamy uzywane przez segment pamieci
        long pages = sysconf(_SC_PHYS_PAGES); // strony pamieci
        long page_size = sysconf(_SC_PAGE_SIZE); // Rozmiar strony w bajtach
        long total_memory = pages * page_size; // Całkowita pamięć RAM w bajtach
        int max_capacity = total_memory * MAX_RAM ;

        // jesli proponowana pojemnosc bedzie zbyt duza
        if (max_capacity < capacity * UNIT_SIZE)
        {
            errno = ENOMEM;
            detect_issue(true, "Za duza pojemnosc magazunu");
        }


        std::cout << "capacity: " << capacity << std::endl;
        return capacity;
    }

    bool does_pid_exist(int pid)
    {
        std::ifstream file("/proc/" + std::to_string(pid) + "/stat");
        return file.is_open();
    }


    void semafor_p(int semid, int sem, int value)
    {
        struct sembuf bufor_sem;
        bufor_sem.sem_num=sem;
        bufor_sem.sem_op=-1*value;
        bufor_sem.sem_flg=0;
        if (semop(semid,&bufor_sem,1)==-1)
        {
            if(errno == EINTR)
            {
                semafor_p(semid, sem, value);
            }
            else
            {
                printf("Blad dekrementacji semafora.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    void semafor_v(int semid, int sem, int value)
    {
        struct sembuf bufor_sem;
        bufor_sem.sem_num=sem;
        bufor_sem.sem_op=value;
        int result = semop(semid,&bufor_sem,1);
        detect_issue(result==IPC_RESULT_ERROR, "Blad dekrementaccji semafora");
    }

    int semafor_value(int semid, int sem)
    {
        // Odczytanie wartości semafora
        int result = semctl(semid, sem, GETVAL);
        detect_issue(result==IPC_RESULT_ERROR, "Blad sprawdzania wartosci semafora");

        return result;
    }

    void semafor_set(int semid, int sem, int value)
    {
        // inicjacja semafora
        int result = semctl(semid, sem, SETVAL, value);
        detect_issue(result==IPC_RESULT_ERROR, "Blad ustawiania wartosci semafora");
    }

    // random
    int random_number(int min, int max)
    {
        // Tworzymy obiekt generatora pseudolosowego
        std::random_device rd; // Zwykle bazuje na zewnętrznych źródłach entropii
        std::mt19937 gen(rd()); // Generator Mersenne Twister
        std::uniform_int_distribution<> dis(min, max); // Rozkład jednorodny w przedziale [min, max]

        // Zwracamy losową liczbę w zadanym zakresie
        return dis(gen);
    }

    char* dolacz_segment_pamieci(int shared_id)
    {
        char *adres = (char *)shmat(shared_id, 0, SHM_RND);
        detect_issue(adres==(char*)-1, "Blad dolaczania segmentu pamieci");

        return adres;
    }
    size_t pobierz_rozmiar_pamieci(int shared_id)
    {
        struct shmid_ds buf;

        // Pobranie informacji o segmencie pamięci
        int result = shmctl(shared_id, IPC_STAT, &buf) == -1;
        detect_issue(result==IPC_RESULT_ERROR, "Blad pobierania rozmiaru pamieci");
        // Zwrócenie rozmiaru pamięci współdzielonej
        return buf.shm_segsz;
    }
    ProductX::ProductX()
    {
        m_weight = random_number(1,60);
    }
    void ProductX::recreate()
    {
        m_weight = random_number(1,60);
    }
    ProductY::ProductY()
    {
        m_weight = random_number(1,60);
    }
    void ProductY::recreate()
    {
        m_weight = random_number(1,60);
    }
    ProductZ::ProductZ()
    {
        m_weight = random_number(1,60);
        m_a = '2';
        m_b = '3';
    }
    void ProductZ::recreate()
    {
        m_weight = random_number(1,60);
        m_a = '2';
        m_b = '3';
    }
}