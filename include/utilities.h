#ifndef UTILITIES_H
#define UTILITIES_H
#include <sys/ipc.h>
#include <sys/types.h>
#include <iostream>

namespace utils
{
    // ---------------------------- OBSLUGA BLEDOW ----------------------------- //
    void detect_issue(int condition, const char *message);
    long calculate_capacity_units(int argc, char *argv[]);
    bool does_pid_exist(int pid);

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
    // FAKT ZE WCZESNIEJ INT + 2*CHAR = 8 BAJTOW KOSZTOWAL WIELE GODZIN CIEKAWEJ ZABAWY

    // ----------------- FUNKCJE POMOCNICZE -----------------------------
    // losowanie numeru z zakresu a, b
    int random_number(int min, int max);

}

#endif //UTILITIES_H
