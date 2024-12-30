#include "utilities.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>
#include <random>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>

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
        std::cout<<"Udalo sie podlaczyc do zbioru semaforow\n";
        return semid;
    }

    // kolejki komunikatow
    int utworz_kolejke(key_t key)
    {
        int msid = msgget(key, IPC_CREAT | 0600);
        if (msid == -1)
        {
            perror("NIe udalo sie utworzyc kolejki komunikatow");
            return -1;
        }
        return msid;

    }

    int get_msid(key_t key)
    {
        int msid = msgget(key, 0);

        while(msid==-1)
        {
            msid = msgget(key, 0);
            std::cout<<"Brak kolejki. Dyrektor musi ja zainicjowac"<<"\n";
            sleep(10);
        }
        std::cout<<"Udalo sie podlaczyc do kolejki\n";
        return msid;
    }

    int usun_kolejke(int id)
    {
        if(msgctl(id, 0, IPC_RMID) == -1)
        {
            perror("Nie udalo sie usunac kolejki komunikatow");
            return -1;
        }
        return 0;
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

    Product::Product(int id, product_type type, int weight)
        : m_id(id), m_type(type), m_weight(weight)
    {

    }

    void Product::set_id(int id)
    {
        m_id = id;
    }
    void Product::set_type(product_type type)
    {
        m_type = type;
    }
    void Product::set_weight(int weight)
    {
        m_weight = weight;
    }
    void Product::describe() const
    {
        std::cout<<"Sending Product ID: "<<m_id<<" Product type: "<<m_type<<" Product weight: "<<m_weight<<"\n";
    }

    void send_product_to_queue(int msgid, const Product& prod, long type)
    {
        Message msg;
        msg.mtype = type;
        std::memcpy(msg.data, &prod, sizeof(prod)); // Serializacja obiektu

        if (msgsnd(msgid, &msg, sizeof(msg.data), 0) == -1)
        {
            perror("msgsnd blad");
        }
    }
    void receive_product_from_queue(int msg_id, Product& prod, long type)
    {
        Message msg;

        // Odbieranie wiadomości
        if (msgrcv(msg_id, &msg, sizeof(msg.data), type, 0) == -1) {
            perror("msgrcv error");
        } else {
            // Deserializacja obiektu
            std::memcpy(&prod, msg.data, sizeof(prod));
            std::cout << "Product received successfully.\n";
        }
    }






}