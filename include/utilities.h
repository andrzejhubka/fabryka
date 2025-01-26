#ifndef UTILITIES_H
#define UTILITIES_H
#include <sys/ipc.h>
#include <sys/types.h>
#include <iostream>

// rodzaje bledow & wynikow
#define IPC_RESULT_ERROR -1
#define FORK_RESULT_ERROR -1
#define INVALID_USER_INPUT -1
#define WAIT_ERROR -1

// zwracane przez warehouse
#define MACHINE_RECIEVED_PRODUCT 0
#define WAREHOUSE_SUCCESFUL_INSERT 0
#define WAREHOUSE_CLOSED 1

// definicja semaforow
#define sem_dostepne_x 0
#define sem_dostepne_y 1
#define sem_dostepne_z 2

#define sem_wolne_miejsca_x 3
#define sem_wolne_miejsca_y 4
#define sem_wolne_miejsca_z 5

#define sem_shelf_x 6
#define sem_shelf_y 7
#define sem_shelf_z 8

#define sem_wareohuse_working 9
#define sem_factory_working 10

// rodzaje komend dyrektora
#define COMMAND_STOP_WAREHOUSE '1'
#define COMMAND_STOP_FACTORY '2'
#define COMMAND_STOP_WAREHOUSE_FACTORY_AND_SAVE '3'
#define COMMAND_STOP_WAREHOUSE_FACTORY_NO_SAVE '4'

namespace utils
{
    // ---------------------------- OBSLUGA BLEDOW ----------------------------- //
    void detect_issue(int condition, const char *message);
    long calculate_capacity_units(int argc, char *argv[]);

    // ---------------------------- SEMAFORY ----------------------------- //
    // zwiekszenie wartosci semafora
    void semafor_p(int semid, int sem, int value);

    // zmniejszenie wartosci semafora
    void semafor_v(int semid, int sem, int value);

    // zwiekszenie wartosci semafora
    void semafor_set(int semid, int sem, int value);

    // odczytanie wartosci semafora
    int semafor_value(int semid, int sem);

    // ---------------------------- PAMIEC ----------------------------- //
    // tworzenie segmentu pamieci
    int utworz_segment_pamieci_dzielonej(key_t klucz, long size);

    // dolaczenie segmentu pamieci -> zwraca adres
    char* dolacz_segment_pamieci(int shared_id);

    // pobranie informajci o rozmiarze segmentu
    size_t pobierz_rozmiar_pamieci(int shared_id);


    // ---------------------------- WSPOLNE OBIEKTY ------------------------- //
    // ------------- PRODUKTY
    class ProductX
    {
    public:
        ProductX();
        // paramety produktu // 2 bajty -> jedna jednostka
        short m_weight;
        void recreate();
    };

    class ProductY
    {
    public:
        ProductY();
        // paramety produktu 4 bajty -> dwie jednostki
        int m_weight;
        void recreate();
    };

    class ProductZ
    {
    public:
        ProductZ();
        // paramety produktu 4+2 bajty -> trzy jednostki
        int m_weight;
        char m_a;
        char m_b;
        void recreate();
    } __attribute__((packed));

    // ----------------- FUNKCJE POMOCNICZE -----------------------------
    // losowanie numeru z zakresu a, b
    int random_number(int min, int max);

}

#endif //UTILITIES_H
