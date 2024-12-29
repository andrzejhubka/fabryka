#ifndef UTILITIES_H
#define UTILITIES_H

#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <iostream>

// definicja semaforow
    #define sem_ordered_x 0
    #define sem_ordered_y 1
    #define sem_ordered_z 2

    #define sem_available_x 3
    #define sem_available_y 4
    #define sem_available_z 5

    #define sem_warehouse 6
    #define sem_queue 7

namespace utils
{

    // zwiekszenie wartosci semafora
    void semafor_p(int semid, int sem, int value);

    // zmniejszenie wartosci semafora
    void semafor_v(int semid, int sem, int value);

    // zwiekszenie wartosci semafora
    void semafor_set(int semid, int sem, int value);

    // tworzenie zbioru semaforow - inicjacja zerami, zwraca semid
    int utworz_zbior_semaforow(key_t key, int count);

    // usuwanie zbioru semaforow
    int usun_zbior_semaforow(key_t key);

    // uzysknanie semid gdy zbior semaforow istnieje; inaczej -1
    int get_semid(key_t);

    // tworzenie kolejki komunikatow
    int utworz_kolejke(key_t key);

    // uzysknanie msgid gdy kolejka istnieje; inaczej -1
    int get_msid(key_t key);

    // usuniecie kolejki
    int usun_kolejke(int id);

    // losowanie numeru z zakresu a, b
    int random_number(int min, int max);

    enum product_type
    {
        X,
        Y,
        Z
    };

    class Product
    {
        public:

        Product(int id, product_type type, int weight);

        enum product_type m_type;
        int m_id;
        int m_weight;

        void set_weight(int weight);
        void set_id(int id);
        void set_type(product_type type);
        void describe() const;

    };

    struct Message
    {
        long mtype;                     // Typ wiadomości
        char data[sizeof(Product)];
    };

    void send_product_to_queue(int msgid, const Product& prod, long type);

    Product receive_product_from_queue(int msgid, long type);
}




#endif //UTILITIES_H
