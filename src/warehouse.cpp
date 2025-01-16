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
}





