#include "warehouse.h"
#include <cstring>
#include <director.h>

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

        // gdy mamy adres odczytujemy informacje
        m_data = reinterpret_cast<warehouse_data*>(m_sharedptr);

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
    }

    int WarehouseManager::initiailze(long capacity)
    {
        // umozliw dostep do półek
        utils::semafor_set(m_semid, sem_shelf_x, 1 );
        utils::semafor_set(m_semid, sem_shelf_y, 1 );
        utils::semafor_set(m_semid, sem_shelf_z, 1 );


        // jesli sie da to zaladuj -> jesli nie to ustawienia domyslne
        if (load_from_file(WAREHOUSE_PATH)==LOAD_FILE_DOESNT_EXIST)
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

            //na poczatku ustawiamy czytanie i pisanie na poczatki tablic
            m_data->x_offset_czytanie = 0;
            m_data->x_offset_pisanie =  0;

            m_data->y_offset_czytanie = 0;
            m_data->y_offset_pisanie =  0;

            m_data->z_offset_czytanie = 0;
            m_data->z_offset_pisanie  = 0;
        }



        std::cout<<"Pomyslnie zainicjowano pojemnosc magazynu: " << capacity<<std::endl;

        // ustawienie miejsca w magazynie
        utils::semafor_set(m_semid, sem_wolne_miejsca_x, m_data->y_wolne);
        utils::semafor_set(m_semid, sem_wolne_miejsca_y, m_data->y_wolne);
        utils::semafor_set(m_semid, sem_wolne_miejsca_z, m_data->x_wolne);
        utils::semafor_set(m_semid, sem_dostepne_x, m_data->x_zajete);
        utils::semafor_set(m_semid, sem_dostepne_y, m_data->y_zajete);
        utils::semafor_set(m_semid, sem_dostepne_z, m_data->z_zajete);
        return 0;
    }


    // --------------------------ZAPIS/ODCZYT DO PLIKU --------------------------
    void WarehouseManager::save_to_file(const std::string& filePath) const
    {
        std::ofstream file(filePath, std::ios::binary);
        std::cout<<"Sciezka:"<<WAREHOUSE_PATH<<std::endl;
        if (!file)
        {
            utils::odlacz_segment_pamieci_dzielonej(m_sharedptr); // Odłącz segment pamięci przed zgłoszeniem błędu
            throw std::runtime_error("Nie udało się otworzyć pliku do zapisu.");
        }

        long memory_size = sizeof(warehouse_data) + m_data->capacity * sizeof(UNIT_SIZE);
        file.write(m_sharedptr, memory_size);
        file.close();
    }

    int WarehouseManager::load_from_file(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ios::binary);
        if (!file)
        {
            return LOAD_FILE_DOESNT_EXIST;
        }

        long memory_size = sizeof(warehouse_data) + m_data->capacity * sizeof(UNIT_SIZE);
        file.read(m_sharedptr, memory_size);
        return STATE_LOADED;
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
        // ------------------------ CZEKANIE NA PRODUKT Z MOZLIWOSCIA WYBUDZENIA MASZYN
        utils::semafor_p(m_semid, sem_dostepne_x, 1);
        //UWAGA! MASZYNA MOGLA ZOSTAC WYBUDZONA GDY CZEKALA NA PRODUKT WIEC TRZRBA TO SPRAWDZIC
        if (utils::semafor_value(m_semid, sem_factory_working)!=1)
        {
            utils::semafor_v(m_semid, sem_dostepne_x, 1);
            return MACHINE_STOPPED;
        }
        // w trakcie pobierania produktu moglo sie okazac ze magazyn jest zamkniety
        if (utils::semafor_value(m_semid, sem_wareohuse_working )!=1)
        {
            utils::semafor_v(m_semid, sem_dostepne_x, 1);
            return WAREHOUSE_CLOSED;
        }

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
        m_data->x_wolne += 1;
        m_data->x_zajete -= 1;
        utils::semafor_v(m_semid, sem_shelf_x, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_x, 1);
        return MACHINE_RECIEVED_PRODUCT;
    }
    int WarehouseManager::grab_y(utils::ProductY* container) //TODOSHARED
    {
        // ------------------------ CZEKANIE NA PRODUKT Z MOZLIWOSCIA WYBUDZENIA MASZYNY
        utils::semafor_p(m_semid, sem_dostepne_y, 1);
        // UWAGA! MASZYNA MOGLA ZOSTAC WYBUDZONA GDY CZEKALA NA PRODUKT WIEC TRZRBA TO SPRAWDZIC
        if (utils::semafor_value(m_semid, sem_factory_working)!=1)
        {
            utils::semafor_v(m_semid, sem_dostepne_y, 1);
            return MACHINE_STOPPED;
        }
        // w trakcie pobierania produktu moglo sie okazac ze magazyn jest zamkniety
        if (utils::semafor_value(m_semid, sem_wareohuse_working )!=1)
        {
            utils::semafor_v(m_semid, sem_dostepne_y, 1);
            return WAREHOUSE_CLOSED;
        }

        // ------------------------ TRZEBA JAKOS ZAPEWNIC ZEBY POLKI NIGDY SIE NIE ZATKALY
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
        m_data->y_wolne += 1;
        m_data->y_zajete -= 1;
        utils::semafor_v(m_semid, sem_shelf_y, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_y, 1);
        return MACHINE_RECIEVED_PRODUCT;
    }
    int WarehouseManager::grab_z(utils::ProductZ* container) //TODOSHARED
    {
        // ------------------------ CZEKANIE NA PRODUKT Z MOZLIWOSCIA WYBUDZENIA MASZYNY
        utils::semafor_p(m_semid, sem_dostepne_z, 1);
        // UWAGA! MASZYNA MOGLA ZOSTAC WYBUDZONA GDY CZEKALA NA PRODUKT WIEC TRZRBA TO SPRAWDZIC
        if (utils::semafor_value(m_semid, sem_factory_working)!=1)
        {
            utils::semafor_v(m_semid, sem_dostepne_z, 1);
            return MACHINE_STOPPED;
        }
        // w trakcie pobierania produktu moglo sie okazac ze magazyn jest zamkniety
        if (utils::semafor_value(m_semid, sem_wareohuse_working )!=1)
        {
            utils::semafor_v(m_semid, sem_dostepne_z, 1);
            return WAREHOUSE_CLOSED;
        }

        // UWAGA! MASZYNA MOGLA ZOSTAC WYBUDZONA GDY CZEKALA NA PRODUKT WIEC TRZRBA TO SPRAWDZIC
        if (utils::semafor_value(m_semid, sem_factory_working)!=1)
        {
            return MACHINE_STOPPED;
        }

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
        m_data->z_wolne += 1;
        m_data->z_zajete -= 1;
        utils::semafor_v(m_semid, sem_shelf_z, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_z, 1);
        return MACHINE_RECIEVED_PRODUCT;
    }
    // --------------------------DODAWANIE ZASOBOW --------------------------
    int WarehouseManager::insert_x(utils::ProductX* container) //TODOSHARED
    {
        // JESLI FABRYKA NIE PRACUJE I NIE MA MIEJSCA TO LEPIEJ NIE OPUSZCZAJ SEMAFORA BO SIE NIE DOCZEKASZ
        if ((utils::semafor_value(m_semid, sem_factory_working)!=1) && (utils::semafor_value(m_semid, sem_wolne_miejsca_x)<1))
        {
            return INSERT_DEADLOCK_RISK;
        }

        // poczekaj na miejsce
        utils::semafor_p(m_semid, sem_wolne_miejsca_x, 1);
        // SPRAWDZ CZY MAGAZYN NIE OBUDZIL CIE Z INNEGO POWODU!
        if (utils::semafor_value(m_semid, sem_wareohuse_working )!=1)
        {
            std::cout<<"Dostawca X: wykryto zamkniecie magazynu"<<std::endl;
            utils::semafor_v(m_semid, sem_wolne_miejsca_x, 1);
            return WAREHOUSE_CLOSED;
        }

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
        m_data->x_zajete += 1;
        m_data->x_wolne -= 1;
        utils::semafor_v(m_semid, sem_dostepne_x, 1);
        utils::semafor_v(m_semid, sem_shelf_x, 1);
        return WAREHOUSE_SUCCESFUL_INSERT;
    }
    int WarehouseManager::insert_y(utils::ProductY* container) //TODOSHARED
    {
        // JESLI FABRYKA NIE PRACUJE I NIE MA MIEJSCA TO LEPIEJ NIE OPUSZCZAJ SEMAFORA BO SIE NIE DOCZEKASZ
        if ((utils::semafor_value(m_semid, sem_factory_working)!=1) && (utils::semafor_value(m_semid, sem_wolne_miejsca_y)<1))
        {
            return INSERT_DEADLOCK_RISK;
        }

        // sprawdz pierwsze czy w ogole masz miejsce!
        utils::semafor_p(m_semid, sem_wolne_miejsca_y, 1);
        // SPRAWDZ CZY MAGAZYN NIE OBUDZIL CIE Z INNEGO POWODU!
        if (utils::semafor_value(m_semid, sem_wareohuse_working )!=1)
        {
            utils::semafor_v(m_semid, sem_wolne_miejsca_y, 1);
            std::cout<<"Dostawca y: wykryto zamkniecie magazynu"<<std::endl;
            return WAREHOUSE_CLOSED;
        }

        utils::semafor_p(m_semid, sem_shelf_y, 1);
        memcpy(y_shelf_adress+m_data->y_offset_pisanie, container, sizeof(utils::ProductY));
        std::cout<<"MAGAZYN: dostarczono produkt Y"<<std::endl;
        offset_move_to_next(m_data->y_offset_pisanie, sizeof(utils::ProductY), m_data->products_per_shelf);
        m_data->y_zajete += 1;
        m_data->y_wolne -= 1;
        utils::semafor_v(m_semid, sem_dostepne_y, 1);
        utils::semafor_v(m_semid, sem_shelf_y, 1);
        return WAREHOUSE_SUCCESFUL_INSERT;
    }
    int WarehouseManager::insert_z(utils::ProductZ* container) //TODOSHARED
    {
        // JESLI FABRYKA NIE PRACUJE I NIE MA MIEJSCA TO LEPIEJ NIE OPUSZCZAJ SEMAFORA BO SIE NIE DOCZEKASZ
        if ((utils::semafor_value(m_semid, sem_factory_working)!=1) && (utils::semafor_value(m_semid, sem_wolne_miejsca_z)<1))
        {
            return INSERT_DEADLOCK_RISK;
        }

        // sprawdz pierwsze czy w ogole masz miejsce!
        utils::semafor_p(m_semid, sem_wolne_miejsca_z, 1);
        // SPRAWDZ CZY MAGAZYN NIE OBUDZIL CIE Z INNEGO POWODU!
        if (utils::semafor_value(m_semid, sem_wareohuse_working )!=1)
        {
            std::cout<<"Dostawca Z: wykryto zamkniecie magazynu"<<std::endl;
            utils::semafor_v(m_semid, sem_wolne_miejsca_z, 1);
            return WAREHOUSE_CLOSED;
        }

        utils::semafor_p(m_semid, sem_shelf_z, 1);
        memcpy(z_shelf_adress+m_data->z_offset_pisanie, container, sizeof(utils::ProductZ));
        std::cout<<"MAGAZYN: dostarczono produkt Z"<<std::endl;
        offset_move_to_next(m_data->z_offset_pisanie, sizeof(utils::ProductZ), m_data->products_per_shelf);
        m_data->z_zajete += 1;
        m_data->z_wolne -= 1;
        utils::semafor_v(m_semid, sem_dostepne_z, 1);
        utils::semafor_v(m_semid, sem_shelf_z, 1);
        return WAREHOUSE_SUCCESFUL_INSERT;
    }
    // --------------------------DIAGNOSTYKA --------------------------
    void WarehouseManager::info()
    {
        std::cout<<"-----------INFORMACJE O MAGAZYNIE---------------- "<<std::endl;
        std::cout<<"Pojemnosc : "<<m_data->capacity<<" jednostek"<<std::endl;
        std::cout<<"Pojemnosc polek: "<<m_data->products_per_shelf<<std::endl;
        std::cout<<"Przechowywane produkty x: "<<m_data->x_zajete<<std::endl;
        std::cout<<"Przechowywane produkty y: "<<m_data->y_zajete<<std::endl;
        std::cout<<"Przechowywane produkty z: "<<m_data->z_zajete<<std::endl;
        std::cout<<"-------------------------------------------------"<<std::endl;
    }
    // --------------------------KONIEC PRACY --------------------------
    int WarehouseManager::close(bool save)
    {
        // aby zamknac magazyn musimy zablokowac wszystkie polki -> MAMY DO NIEGO CALKOWITY DOSTEP
        utils::semafor_p(m_semid, sem_shelf_x, 1);
        utils::semafor_p(m_semid, sem_shelf_y, 1);
        utils::semafor_p(m_semid, sem_shelf_z, 1);

        // zapisujemy go do pliku
        if (save)
        {
            save_to_file(WAREHOUSE_PATH);
        }
        else
        {
            std::remove(WAREHOUSE_PATH);
        }

        // daj znac ze magazyn juz nie dziala
        utils::semafor_v(m_semid, sem_wareohuse_working, WAREHOUSE_CLOSED);

        // obudz dostawcow
        utils::semafor_v(m_semid, sem_wolne_miejsca_x, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_y, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_z, 1);

        // obudz te maszyny ktore czekaja na produkt
        utils::semafor_v(m_semid, sem_dostepne_x, 2);
        utils::semafor_v(m_semid, sem_dostepne_y, 2);
        utils::semafor_v(m_semid, sem_dostepne_z, 2);

        utils::semafor_v(m_semid, sem_shelf_x, 1);
        utils::semafor_v(m_semid, sem_shelf_y, 1);
        utils::semafor_v(m_semid, sem_shelf_z, 1);

        return 0;
        //


    }

}





