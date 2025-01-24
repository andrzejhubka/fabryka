#ifndef UTILITIES_H
#define UTILITIES_H
#include <sys/ipc.h>
#include <sys/types.h>
#include <iostream>

namespace utils
{

    // ---------------------------- SEMAFORY ----------------------------- //
    // zwiekszenie wartosci semafora
    void semafor_p(int semid, int sem, int value);

    // zmniejszenie wartosci semafora
    void semafor_v(int semid, int sem, int value);

    // zwiekszenie wartosci semafora
    void semafor_set(int semid, int sem, int value);

    // odczytanie wartosci semafora
    int semafor_value(int semid, int sem);

    // tworzenie zbioru semaforow - inicjacja zerami, zwraca semid
    int utworz_zbior_semaforow(key_t key, int count);

    // usuwanie zbioru semaforow
    int usun_zbior_semaforow(key_t key);

    // uzysknanie semid gdy zbior semaforow istnieje; inaczej -1
    int get_semid(key_t);


    // ---------------------------- PAMIEC ----------------------------- //
    // tworzenie segmentu pamieci
    int utworz_segment_pamieci_dzielonej(key_t klucz, long size);

    // dolaczenie segmentu pamieci -> zwraca adres
    char* dolacz_segment_pamieci(int shared_id);

    // pobranie id segmentu pamieci
    int get_shared_id(key_t klucz);

    // odlaczenie segmentu z biezacego procesu
    int odlacz_segment_pamieci_dzielonej(char *adres);

    // ustawienie do usuniecia segmnetu
    int ustaw_do_usuniecia_segment(int memid);

    // pobranie informajci o rozmiarze segmentu
    size_t pobierz_rozmiar_pamieci(int shared_id);


    // ---------------------------- WSPOLNE OBIEKTY ----------------------------- //
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

    // ----------ENUMY
    // typ produktu
    enum product_type
    {
        X, Y, Z
    };
    // komendy dyrektora
    enum Command
    {
        stop_magazyn,
        stop_fabryka,
        stop_magazyn_fabryka,
        stop_bez_zapisu
    };

    // ----------------- FUNKCJE POMOCNICZE -----------------------------
    // losowanie numeru z zakresu a, b
    int random_number(int min, int max);

}




#endif //UTILITIES_H
