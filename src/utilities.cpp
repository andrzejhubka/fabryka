#include "utilities.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>
#include <random>
#include <sys/msg.h>
#include <cstring>
#include <sys/shm.h>


namespace utils
{
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
        // TA FLAGA WYSADZILA MI 1000 RAZY PROGRAM BO DOSTAWCY/FABRYKA COFALI SWOJE OPERACJE bufor_sem.sem_flg=SEM_UNDO;
        if (semop(semid,&bufor_sem,1)==-1)
        {
            printf("Koniec procesu.\n");
            exit(EXIT_FAILURE);
        }
    }

    int semafor_value(int semid, int sem)
    {
        // Odczytanie wartości semafora
        int value = semctl(semid, sem, GETVAL);

        // Jeśli operacja zwróci -1, oznacza to błąd
        if (value == -1)
        {
            throw std::runtime_error("Błąd odczytu wartości semafora.");
        }

        return value;
    }

    void semafor_set(int semid, int sem, int value)
    {
        // inicjacja semafora
        int wynikInicjalizacji = semctl(semid, sem, SETVAL, value);
        if (wynikInicjalizacji == -1)
        {
            perror("Nie udalo sie zainicjializowac wartosci semafora");
        }
    }

    int utworz_zbior_semaforow(key_t key, int count)
    {
        // tworzenie zbioru semaforow
        int semid = semget(key, count, IPC_CREAT | 0600);

        if (semid == -1)
        {
            perror("NIe udalo sie utworzyc zbioru semaforow");
            return -1;
        }

        return semid;
    }

    int usun_zbior_semaforow(int semid)
    {
        if(semctl(semid, 0, IPC_RMID) == -1)
        {
            perror("Nie udalo sie usunac zbioru semaforow");
            return IPC_RESULT_ERROR;
        }
        return 0;
    }

    int get_semid(key_t key)
    {
        int semid = semget(key, 0, 0);

        while(semid==-1)
        {
            semid = semget(key, 0, 0);
            //perror("Nie zbior semaforow nie istnieje, czekam na jego utworzenie");
            std::cout<<"Brak zbioru semaforow. Dyrektor musi go zainicjowac"<<"\n";
            sleep(10);
        }
        std::cout<<"Udalo sie podlaczyc do zbioru semaforow\n";
        return semid;
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

    int utworz_segment_pamieci_dzielonej(key_t klucz, long size)
    {
        // utworzenie i uzyskanie dostepu segmentu pamieci dzielonej
        int createid = shmget(klucz, size, IPC_CREAT | 0600);
        if (createid == IPC_RESULT_ERROR)
        {
            perror("Blad tworzenie segmentu pamieci\n");
            return IPC_RESULT_ERROR;
        }
        return createid;
    }
    int odlacz_segment_pamieci_dzielonej(char *adres)
    {
        int result = shmdt(adres);
        if(result == -1)
        {
            perror("Blad odlaczania segmentu\n");
            return -1;
        }
        return 0;
    }
    int ustaw_do_usuniecia_segment(int memid)
    {
        int result = shmctl(memid, IPC_RMID, NULL);
        if (result == -1 )
        {
            printf("Blad usuwania segmentu");
            return -1;
        }
        return 0;
    }
    int get_shared_id(key_t klucz)
    {
        int id = shmget(klucz, 0, 0);

        while(id==IPC_RESULT_ERROR)
        {
            id = msgget(klucz, 0);
            std::cout<<"Brak segmentu. Dyrektor musi go zainicjowac"<<"\n";
            sleep(10);
        }
       return id;
    }

    char* dolacz_segment_pamieci(int shared_id)
    {
        char *adres = (char *)shmat(shared_id, 0, SHM_RND);
        if (adres == (char*)-1)
        {
            std::cerr << "Błąd przy dołączaniu segmentu pamięci współdzielonej: "
                      << strerror(errno) << std::endl;
            return nullptr;
        }
        std::cout<<"Udalo sie dolaczyc do pamieci wspoldzielonej\n";
        return adres;
    }
    size_t pobierz_rozmiar_pamieci(int shared_id)
    {
        struct shmid_ds buf;

        // Pobranie informacji o segmencie pamięci
        if (shmctl(shared_id, IPC_STAT, &buf) == -1)
        {
            std::cerr << "Błąd: nie udało się pobrać informacji o pamięci współdzielonej: "
                      << strerror(errno) << std::endl;
            return IPC_RESULT_ERROR; // Zwróć 0 w przypadku błędu
        }
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