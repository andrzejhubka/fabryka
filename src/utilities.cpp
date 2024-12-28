#include "utilities.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>
#include <random>

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
        bufor_sem.sem_flg=SEM_UNDO;
        if (semop(semid,&bufor_sem,1)==-1)
        {
            printf("Blad inkrementacji semafora.\n");
            exit(EXIT_FAILURE);
        }
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
            return -1;
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

}