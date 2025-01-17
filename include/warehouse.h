#ifndef WAREHOUSE_H
#define WAREHOUSE_H
#include <iostream>
#include <mutex>
#include <utilities.h>
#include <condition_variable>

namespace warehouse
{
    // ---------- STRUKTURA INFORMACJI O MAGAZYNIE

    // informacja o polce w magazynie
    struct warehouse_data
    {
        long capacity;
        int products_per_shelf;
        // dane polki x
        int x_offset; // wzgledem poczatku zegmentu pamieci wspoldzielnej
        int x_offset_pisanie; // ozgledem poczatku tablicy;
        int x_offset_czytanie; // wzgledem poczatku polki x; zeby modulo dzialalo
        int x_wolne;
        int x_zajete;

        // dane polki y
        int y_offset;
        int y_offset_pisanie;
        int y_offset_czytanie;
        int y_wolne;
        int y_zajete;
        // dane polki z
        int z_offset;
        int z_offset_pisanie;
        int z_offset_czytanie;
        int z_wolne;
        int z_zajete;
    };

    class WarehouseManager
    {
    public:
        // konstruktory & destruktory
        WarehouseManager(key_t ipckey, int semid);
        WarehouseManager();
        ~WarehouseManager();

        // ipc
        int m_semid;
        int m_sharedid ;
        char* m_sharedptr;

        // stan magazynu
        warehouse_data *m_data;

        // indywidualne adresy polek dla kazdego procesu
        char* x_shelf_adress;
        char* y_shelf_adress;
        char* z_shelf_adress;

        // zakonczenie pracy
        int close(bool save);

        // po zapisaniu/odczytaniu przesuwamy wskaznik odczytu/zapisu na kolejny objekt
        int offset_move_to_next(int &offset, size_t object_size, int shelf_capacity);

        // inicjalizacja wskaznikow i danych o magazynie -> raczej tylko dyrektor jej uzywa
        int initiailze(long capacity);

        // dodanie czegos do magazynu
        int insert_x(utils::ProductX* container);
        int insert_y(utils::ProductY* container);
        int insert_z(utils::ProductZ* container);

        // wydawanie produktow pracownikom
        int grab_x(utils::ProductX* container);
        int grab_y(utils::ProductY* container);
        int grab_z(utils::ProductZ* container);

        // zapisanie stanu do pliku
        void save_to_file(const std::string& filePath) const;
        // wczytanie stanu z pliku
        void load_from_file(const std::string& filePath);

        // informacje o magazynie
        void info();
        // obudz czekajace na produkty maszyny, zeby sprawdzily czy sa tez wylaczone
        void wake_machines();
    };

};




#endif //WAREHOUSE_H
