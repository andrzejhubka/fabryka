#include "warehouse.h"
#include <cstring>
#include "utilities.h"
#include  <iostream>
#include <fstream>
#include <unistd.h>
#include <iomanip>
namespace warehouse
{
    // --------------------------KONSTRUKTURY/DESTRUKTORY--------------------------

    WarehouseManager::WarehouseManager(key_t ipckey, int semid)
    {
        // inicjacja ipc
        m_sharedid = utils::get_shared_id(ipckey);
        m_sharedptr = utils::dolacz_segment_pamieci(m_sharedid);
        m_semid = semid;

        std::cout << "Shared ptr: " << m_sharedptr << std::endl;
        // gdy mamy adres odczytujemy informacje
        m_data = reinterpret_cast<warehouse_data*>(m_sharedptr);

        std::cout << "Na polce x jest "<<m_data->x_wolne << " wolnych miejsc"<<std::endl;
        std::cout << "Na polce y jest "<<m_data->y_wolne << " wolnych miejsc"<<std::endl;
        std::cout << "Na polce z jest "<<m_data->z_wolne << " wolnych miejsc"<<std::endl;

        x_shelf_adress = m_sharedptr+m_data->x_offset;
        y_shelf_adress = m_sharedptr+m_data->y_offset;
        z_shelf_adress = m_sharedptr+m_data->z_offset;
    }
    WarehouseManager::WarehouseManager()
    {
        m_sharedid = -1;
        m_sharedptr = NULL;
    }

    WarehouseManager::~WarehouseManager()
    {
        // zapisujemy stan do pliku
        save_state("/home/andrzej/Documents/SO/fabryka/data/warehouse_state");
    }

    int WarehouseManager::initiailze(long capacity)
    {
        // inicjalizacja semaforow
        utils::semafor_set(m_semid, sem_shelf_x, 1 );
        utils::semafor_set(m_semid, sem_shelf_y, 1 );
        utils::semafor_set(m_semid, sem_shelf_z, 1 );

        m_data->capacity = capacity;
        m_data->products_per_shelf = capacity / 6;

        // obbliczamy ofssety, poczatki tablic
        m_data->x_offset = sizeof(warehouse_data);
        m_data->y_offset = m_data->x_offset + sizeof(utils::ProductX)*m_data->products_per_shelf;
        m_data->z_offset = m_data->y_offset + sizeof(utils::ProductY)*m_data->products_per_shelf;

        m_data->x_wolne = m_data->products_per_shelf;
        m_data->x_zajete = 0;

        m_data->y_wolne = m_data->products_per_shelf;
        m_data->y_zajete = 0;

        m_data->z_wolne = m_data->products_per_shelf;
        m_data->z_zajete = 0;

        //na poczatku ustawiamy czytanie i pisanie na poczatki tablic
        m_data->x_offset_czytanie = 0;
        m_data->x_offset_pisanie =  0;

        m_data->y_offset_czytanie = 0;
        m_data->y_offset_pisanie =  0;

        m_data->z_offset_czytanie = 0;
        m_data->z_offset_pisanie  = 0;


        // testowanie offsetów
        std::cout << "Adres pamieci: " << std::dec << reinterpret_cast<uintptr_t>(m_sharedptr) << std::endl;
        std::cout << "Adres pisania z: " << std::dec << reinterpret_cast<uintptr_t>(m_sharedptr+m_data->z_offset_pisanie) << std::endl;

        std::cout<<"Waga danych:"<<sizeof(warehouse_data)<<std::endl;
        std::cout<<"Offset X:"<<m_data->x_offset<<std::endl;
        std::cout<<"Offset y:"<<m_data->y_offset<<std::endl;
        std::cout<<"Offset z:"<<m_data->z_offset<<std::endl;
        std::cout<<"Pomyslnie zainicjowano pojemnosc magazynu: " << capacity<<std::endl;

        // ustawienie miejsca w magazynie
        utils::semafor_set(m_semid, sem_wolne_miejsca_x, m_data->products_per_shelf );
        utils::semafor_set(m_semid, sem_wolne_miejsca_y, m_data->products_per_shelf );
        utils::semafor_set(m_semid, sem_wolne_miejsca_z, m_data->products_per_shelf );
        return 0;
    }


    // --------------------------ZAPIS/ODCZYT DO PLIKU --------------------------
    void WarehouseManager::load_state(const std::string& filePath)
    {

    }

    void WarehouseManager::save_state(const std::string& filePath) const
    {

    }
    // --------------------------Zarzadzanie wskaznikami--------------------------
    int WarehouseManager::offset_move_to_next(int& offset, size_t object_size, int shelf_capacity)
    {
        int new_offset = (offset+object_size) % ( shelf_capacity*object_size);
        offset = new_offset;
        return 1;
    }



    // --------------------------POBIERANIE ZASOBOW --------------------------

    int WarehouseManager::grab_x(utils::ProductX* container) //TODOSHARED
    {
        // jesli produkt jest dostepny:
        utils::semafor_p(m_semid, sem_dostepne_x, 1);
        utils::semafor_p(m_semid, sem_shelf_x, 1);
        if (container != nullptr)
        {
            memcpy(container,x_shelf_adress+m_data->x_offset_czytanie, sizeof(utils::ProductX));
        }
        else
        {
            std::cerr << "Błąd: Wskaźniki są niezainicjowane!" << std::endl;
            return -1;
        }
        std::cout<<"MAGAZYN: pobrano produkt X"<<std::endl;
        offset_move_to_next(m_data->x_offset_czytanie, sizeof(utils::ProductX), m_data->products_per_shelf);
        utils::semafor_v(m_semid, sem_shelf_x, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_x, 1);
        return 1;
    }

    int WarehouseManager::grab_y(utils::ProductY* container) //TODOSHARED
    {
        // jesli produkt jest dostepny:
        utils::semafor_p(m_semid, sem_dostepne_y, 1);
        utils::semafor_p(m_semid, sem_shelf_y, 1);
        if (container != nullptr)
        {
            memcpy(container,y_shelf_adress+m_data->y_offset_czytanie, sizeof(utils::ProductY));
        }
        else
        {
            std::cerr << "Błąd: Wskaźniki są niezainicjowane!" << std::endl;
            return -1;
        }
        std::cout<<"MAGAZYN: pobrano produkt Y"<<std::endl;
        offset_move_to_next(m_data->y_offset_czytanie, sizeof(utils::ProductY), m_data->products_per_shelf);
        utils::semafor_v(m_semid, sem_shelf_y, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_y, 1);
        return 1;
    }
    int WarehouseManager::grab_z(utils::ProductZ* container) //TODOSHARED
    {
        // jesli produkt jest dostepny:
        utils::semafor_p(m_semid, sem_dostepne_z, 1);
        utils::semafor_p(m_semid, sem_shelf_z, 1);
        if (container != nullptr)
        {
            memcpy(container,z_shelf_adress+m_data->z_offset_czytanie, sizeof(utils::ProductZ));
        }
        else
        {
            std::cerr << "Błąd: Wskaźniki są niezainicjowane!" << std::endl;
            return -1;
        }
        std::cout<<"MAGAZYN: pobrano produkt Z"<<std::endl;
        offset_move_to_next(m_data->z_offset_czytanie, sizeof(utils::ProductZ), m_data->products_per_shelf);
        utils::semafor_v(m_semid, sem_shelf_z, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_z, 1);
        return 1;
    }
    // --------------------------DODAWANIE ZASOBOW --------------------------
    int WarehouseManager::insert_x(utils::ProductX* container) //TODOSHARED
    {
        // sprawdz pierwsze czy w ogole masz miejsce!
        utils::semafor_p(m_semid, sem_wolne_miejsca_x, 1);
        utils::semafor_p(m_semid, sem_shelf_x, 1);
        if (container != nullptr)
        {
        memcpy(x_shelf_adress+m_data->x_offset_pisanie, container, sizeof(utils::ProductX));
        }
        else
        {
            std::cerr << "Błąd: Wskaźniki są niezainicjowane!" << std::endl;
        }
        std::cout<<"MAGAZYN: dostarczono produkt X"<<std::endl;
        offset_move_to_next(m_data->x_offset_pisanie, sizeof(utils::ProductX), m_data->products_per_shelf);
        utils::semafor_v(m_semid, sem_shelf_x, 1);
        utils::semafor_v(m_semid, sem_dostepne_x, 1);
        return 1;
    }
    int WarehouseManager::insert_y(utils::ProductY* container) //TODOSHARED
    {
        // sprawdz pierwsze czy w ogole masz miejsce!
        utils::semafor_p(m_semid, sem_wolne_miejsca_y, 1);
        utils::semafor_p(m_semid, sem_shelf_y, 1);
        memcpy(y_shelf_adress+m_data->y_offset_pisanie, container, sizeof(utils::ProductY));
        std::cout<<"MAGAZYN: dostarczono produkt Y"<<std::endl;
        offset_move_to_next(m_data->y_offset_pisanie, sizeof(utils::ProductY), m_data->products_per_shelf);
        utils::semafor_v(m_semid, sem_shelf_y, 1);
        utils::semafor_v(m_semid, sem_dostepne_y, 1);
        return 1;
    }
    int WarehouseManager::insert_z(utils::ProductZ* container) //TODOSHARED
    {
        // sprawdz pierwsze czy w ogole masz miejsce!
        utils::semafor_p(m_semid, sem_wolne_miejsca_z, 1);
        utils::semafor_p(m_semid, sem_shelf_z, 1);
        memcpy(z_shelf_adress+m_data->z_offset_pisanie, container, sizeof(utils::ProductZ));
        std::cout<<"MAGAZYN: dostarczono produkt Z"<<std::endl;
        offset_move_to_next(m_data->z_offset_pisanie, sizeof(utils::ProductZ), m_data->products_per_shelf);
        utils::semafor_v(m_semid, sem_shelf_z, 1);
        utils::semafor_v(m_semid, sem_dostepne_z, 1);
        return 1;
    }
    // --------------------------DIAGNOSTYKA --------------------------
    void WarehouseManager::info()
    {
        std::cout<<"Warehouse Capacity: "<<m_data->capacity<<std::endl;
    }



}





