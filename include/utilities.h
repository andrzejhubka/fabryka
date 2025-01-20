#ifndef UTILITIES_H
#define UTILITIES_H
#include <sys/ipc.h>
#include <sys/types.h>
#include <iostream>

// sciezka do pliku z magazynem
#define WAREHOUSE_PATH "warehouse_data"

// rodzaje bledow & wynikow
#define IPC_RESULT_ERROR -1
#define MACHINE_STOPPED -2

// zwracane przez warehouse
#define MACHINE_RECIEVED_PRODUCT 0
#define WAREHOUSE_SUCCESFUL_INSERT 0
#define WAREHOUSE_CLOSED 1
#define INSERT_DEADLOCK_RISK 2

// definicja semaforow
#define sem_dostepne_x 0
#define sem_dostepne_y 1
#define sem_dostepne_z 2

#define sem_wolne_miejsca_x 3
#define sem_wolne_miejsca_y 4
#define sem_wolne_miejsca_z 5

#define sem_shelf_x 7
#define sem_shelf_y 8
#define sem_shelf_z 9

#define sem_command 6

#define sem_factory_working 10
#define sem_wareohuse_working 11

// rodzaje komend dyrektora
#define COMMAND_STOP_WAREHOUSE 1
#define COMMAND_STOP_FACTORY 2
#define COMMAND_STOP_WAREHOUSE_FACTORY_AND_SAVE 3
#define COMMAND_STOP_WAREHOUSE_FACTORY_NO_SAVE 4

// czasy wykonywania czynnosci -> w milisekundach
#define speed_machine_a 0//10000
#define speed_machine_b 0//10000
#define speed_supply_x 0//1000
#define speed_supply_y 0//1000
#define speed_supply_z 0//1000



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

    // ---------------------------- KOLEJKI ----------------------------- //
    // tworzenie kolejki komunikatow
    int utworz_kolejke(key_t key);

    // uzysknanie msgid gdy kolejka istnieje; inaczej -1
    int get_msid(key_t key);

    // usuniecie kolejki
    int usun_kolejke(int id);
    // ---------------------------- PAMIEC ----------------------------- //
    typedef struct PamiecWspoldzielona
    {
        int id;
        size_t size;
        int flg;
        char *adres;
    } PamiecWspoldzielona;

    // tworzenie segmentu pamieci
    int utworz_segment_pamieci_dzielonej(PamiecWspoldzielona *pDzielona, key_t klucz, long size);

    // dolaczenie segmentu pamieci -> zwraca adres
    char* dolacz_segment_pamieci(int shared_id);

    // pobranie id segmentu pamieci
    int get_shared_id(key_t klucz);

    // odlaczenie segmentu z biezacego procesu
    int odlacz_segment_pamieci_dzielonej(char *adres);

    // ustawienie do usuniecia segmnetu
    int ustaw_do_usuniecia_segment(PamiecWspoldzielona *pDzielona);

    // pobranie informajci o rozmiarze segmentu
    size_t pobierz_rozmiar_pamieci(int shared_id);


    // ---------------------------- WSPOLNE OBIEKTY ----------------------------- //
    // ------------- PRODUKTY
    class ProductX
    {
    public:
        ProductX(short weight);
        // paramety produktu // 2 bajty -> jedna jednostka
        short m_weight;
    };

    class ProductY
    {
    public:
        ProductY(int weight);
        // paramety produktu 4 bajty -> dwie jednostki
        int m_weight;
    };

    class ProductZ
    {
    public:
        ProductZ(int weight, char a, char b);
        // paramety produktu 4+2 bajty -> trzy jednostki
        int m_weight;
        char m_a;
        char m_b;
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
