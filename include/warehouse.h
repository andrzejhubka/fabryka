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
        int x_offset;
        char* x_pisanie;
        char* x_czytanie;
        int x_wolne;
        int x_zajete;

        // dane polki y
        int y_offset;
        char* y_pisanie;
        char* y_czytanie;
        int y_wolne;
        int y_zajete;
        // dane polki z
        int z_offset;
        char* z_pisanie;
        char* z_czytanie;
        int z_wolne;
        int z_zajete;
    };

    class WarehouseManager
    {
    public:
        // konstruktory & destruktory
        WarehouseManager(key_t ipckey);
        WarehouseManager();
        ~WarehouseManager();

        // kontrola pamieci wszpoldzielonej
        int m_sharedid ;
        char* m_sharedptr;

        // stan magazynu
        warehouse_data *m_data;

        // inicjalizacja wskaznikow i danych o magazynie -> raczej tylko dyrektor jej uzywa
        int initiailze(long capacity);

        // dodanie czegos do magazynu
        int insert_x(utils::ProductX container);
        int insert_y(utils::ProductY container);
        int insert_z(utils::ProductZ container);

        // wydawanie produktow pracownikom
        int grab_x(utils::ProductX container);
        int grab_y(utils::ProductY container);
        int grab_z(utils::ProductZ container);

        // zapisanie stanu do pliku
        void save_state(const std::string& filePath) const;
        // wczytanie stanu z pliku
        void load_state(const std::string& filePath);

        // informacje o magazynie
        void info();
        // obudz czekajace na produkty maszyny, zeby sprawdzily czy sa tez wylaczone
        void wake_machines();
    };

};




#endif //WAREHOUSE_H
