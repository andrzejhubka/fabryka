#include "warehouse.h"
#include <cstring>
#include "utilities.h"
#include  <iostream>
#include <fstream>
#include <unistd.h>

namespace warehouse
{
    // --------------------------KONSTRUKTURY/DESTRUKTORY--------------------------

    WarehouseManager::WarehouseManager(key_t ipckey)
    {
        // inicjacja ipc
        m_sharedid = utils::get_shared_id(ipckey);
        m_sharedptr = utils::dolacz_segment_pamieci(m_sharedid);

        // gdy mamy adres odczytujemy informacje
        m_data = reinterpret_cast<warehouse_data*>(m_sharedptr);

        std::cout << "Na polce x jest "<<m_data->x_wolne << " wolnych miejsc"<<std::endl;
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

        std::cout<<"Pomyslnie zainicjowano pojemnosc magazynu: " << capacity<<std::endl;
        return 0;
    }


    // --------------------------ZAPIS/ODCZYT DO PLIKU --------------------------
    void WarehouseManager::load_state(const std::string& filePath)
    {

    }

    void WarehouseManager::save_state(const std::string& filePath) const
    {

    }

    // --------------------------POBIERANIE ZASOBOW --------------------------

    int WarehouseManager::grab_x(utils::ProductX container) //TODOSHARED
    {
        return 1;
    }

    int WarehouseManager::grab_y(utils::ProductY container) //TODOSHARED
    {
        return 1;
    }
    int WarehouseManager::grab_z(utils::ProductZ container) //TODOSHARED
    {
        return 1;
    }
    // --------------------------DODAWANIE ZASOBOW --------------------------
    int WarehouseManager::insert_x(utils::ProductX container) //TODOSHARED
    {
        return 1;
    }
    int WarehouseManager::insert_y(utils::ProductY container) //TODOSHARED
    {
        return 1;
    }
    int WarehouseManager::insert_z(utils::ProductZ container) //TODOSHARED
    {
        return 1;
    }
    // --------------------------DIAGNOSTYKA --------------------------
    void WarehouseManager::info()
    {
        std::cout<<"Warehouse Capacity: "<<m_data->capacity<<std::endl;
    }


}





