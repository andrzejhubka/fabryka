#include <unistd.h>
#include <cstring>
#include  <iostream>
#include <fstream>
#include <iomanip>
#include <director.h>
#include <sys/shm.h>
#include "utilities.h"
#include "config.h"
#include "warehouse.h"



namespace warehouse
{
    // --------------------------KONSTRUKTURY/DESTRUKTORY--------------------------

    WarehouseManager::WarehouseManager(key_t ipckey, int semid)
    {
        // inicjacja ipc
        m_sharedid = shmget(ipckey, 0, 0);
        utils::detect_issue(m_sharedid==IPC_RESULT_ERROR, "WarehouseManager: blad pobierania semid");

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

        utils::semafor_set(m_semid, sem_wareohuse_working, 1);
        return 0;
    }

    // --------------------------ZAPIS/ODCZYT DO PLIKU --------------------------
    void WarehouseManager::save_to_file(const std::string& filePath) const
    {
        std::ofstream file(filePath, std::ios::binary);
        utils::detect_issue(!file, "Blad otwarcie pliku do zapisu");

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
    int WarehouseManager::offset_move_to_next(size_t& offset, size_t object_size, int shelf_capacity)
    {
        size_t new_offset = (offset+object_size) % ( shelf_capacity*object_size);
        offset = new_offset;
        return 1;
    }



    // --------------------------POBIERANIE ZASOBOW --------------------------

    int WarehouseManager::grab_x(utils::ProductX* container)
    {
        // czekaj na produkt
        utils::semafor_p(m_semid, sem_dostepne_x, 1);
        // magazyn mogl obudzic maszyne z powodu potrzeby jej wylaczenia
        if (utils::semafor_value(m_semid, sem_factory_working )!=1)
        {
            std::cout<<"Maszyna X: obudzono do wylaczenia"<<std::endl;
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
        std::cout<<"\033[33m"<<"MASZYNA: pobrano produkt X"<< "\033[0m" <<std::endl;
        offset_move_to_next(m_data->x_offset_czytanie, sizeof(utils::ProductX), m_data->products_per_shelf);
        m_data->x_wolne += 1;
        m_data->x_zajete -= 1;
        utils::semafor_v(m_semid, sem_shelf_x, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_x, 1);
        return MACHINE_RECIEVED_PRODUCT;
    }
    int WarehouseManager::grab_y(utils::ProductY* container)
    {
        // czekaj na produkt
        utils::semafor_p(m_semid, sem_dostepne_y, 1);
        // magazyn mogl obudzic maszyne z powodu potrzeby jej wylaczenia
        if (utils::semafor_value(m_semid, sem_factory_working )!=1)
        {
            std::cout<<"Maszyna Y: obudzono do wylaczenia"<<std::endl;
            return WAREHOUSE_CLOSED;
        }

        // zajmij polke i pobierz produkt
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
        std::cout<<"\033[33m"<<"MASZYNA: pobrano produkt Y"<< "\033[0m" <<std::endl;
        offset_move_to_next(m_data->y_offset_czytanie, sizeof(utils::ProductY), m_data->products_per_shelf);
        m_data->y_wolne += 1;
        m_data->y_zajete -= 1;
        utils::semafor_v(m_semid, sem_shelf_y, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_y, 1);
        return MACHINE_RECIEVED_PRODUCT;
    }
    int WarehouseManager::grab_z(utils::ProductZ* container)
    {
        // czekanie na produkt
        utils::semafor_p(m_semid, sem_dostepne_z, 1);
        // magazyn mogl obudzic maszyne z powodu potrzeby jej wylaczenia
        if (utils::semafor_value(m_semid, sem_factory_working )!=1)
        {
            std::cout<<"Maszyna Z: obudzono do wylaczenia"<<std::endl;
            return WAREHOUSE_CLOSED;
        }

        // zajmoj polke i wloz produkt
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
        std::cout<<"\033[33m"<<"MASZYNA: pobrano produkt Z"<< "\033[0m" <<std::endl;
        offset_move_to_next(m_data->z_offset_czytanie, sizeof(utils::ProductZ), m_data->products_per_shelf);
        m_data->z_wolne += 1;
        m_data->z_zajete -= 1;
        utils::semafor_v(m_semid, sem_shelf_z, 1);
        utils::semafor_v(m_semid, sem_wolne_miejsca_z, 1);
        return MACHINE_RECIEVED_PRODUCT;
    }
    // --------------------------DODAWANIE ZASOBOW --------------------------
    int WarehouseManager::insert(utils::ProductX* container) //TODOSHARED
    {
        // poczekaj na miejsce
        utils::semafor_p(m_semid, sem_wolne_miejsca_x, 1);
        // SPRAWDZ CZY MAGAZYN NIE OBUDZIL CIE Z INNEGO POWODU!
        if (utils::semafor_value(m_semid, sem_wareohuse_working )!=1)
        {
            std::cout<<"Dostawca X: wykryto zamkniecie magazynu"<<std::endl;
            //utils::semafor_v(m_semid, sem_wolne_miejsca_x, 1);
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
        std::cout<<"\033[34m"<<"DOSTAWCA: dostarczono produkt X"<< "\033[0m" <<std::endl;
        offset_move_to_next(m_data->x_offset_pisanie, sizeof(utils::ProductX), m_data->products_per_shelf);
        m_data->x_zajete += 1;
        m_data->x_wolne -= 1;
        utils::semafor_v(m_semid, sem_dostepne_x, 1);
        utils::semafor_v(m_semid, sem_shelf_x, 1);
        return WAREHOUSE_SUCCESFUL_INSERT;
    }
    int WarehouseManager::insert(utils::ProductY* container) //TODOSHARED
    {
        // sprawdz pierwsze czy w ogole masz miejsce!
        utils::semafor_p(m_semid, sem_wolne_miejsca_y, 1);
        // SPRAWDZ CZY MAGAZYN NIE OBUDZIL CIE Z INNEGO POWODU!
        if (utils::semafor_value(m_semid, sem_wareohuse_working )!=1)
        {
            //utils::semafor_v(m_semid, sem_wolne_miejsca_y, 1);
            std::cout<<"DOSTAWCA: wykryto zamkniecie magazynu"<<std::endl;
            return WAREHOUSE_CLOSED;
        }

        utils::semafor_p(m_semid, sem_shelf_y, 1);
        memcpy(y_shelf_adress+m_data->y_offset_pisanie, container, sizeof(utils::ProductY));
        std::cout<<"\033[34m"<<"MAGAZYN: dostarczono produkt Y"<< "\033[0m" <<std::endl;
        offset_move_to_next(m_data->y_offset_pisanie, sizeof(utils::ProductY), m_data->products_per_shelf);
        m_data->y_zajete += 1;
        m_data->y_wolne -= 1;
        utils::semafor_v(m_semid, sem_dostepne_y, 1);
        utils::semafor_v(m_semid, sem_shelf_y, 1);
        return WAREHOUSE_SUCCESFUL_INSERT;
    }
    int WarehouseManager::insert(utils::ProductZ* container) //TODOSHARED
    {
        // sprawdz pierwsze czy w ogole masz miejsce!
        utils::semafor_p(m_semid, sem_wolne_miejsca_z, 1);
        // SPRAWDZ CZY MAGAZYN NIE OBUDZIL CIE Z INNEGO POWODU!
        if (utils::semafor_value(m_semid, sem_wareohuse_working )!=1)
        {
            std::cout<<"Dostawca Z: wykryto zamkniecie magazynu"<<std::endl;
            //utils::semafor_v(m_semid, sem_wolne_miejsca_z, 1);
            return WAREHOUSE_CLOSED;
        }

        utils::semafor_p(m_semid, sem_shelf_z, 1);
        memcpy(z_shelf_adress+m_data->z_offset_pisanie, container, sizeof(utils::ProductZ));
        std::cout<<"\033[34m"<<"DOSTAWCA: dostarczono produkt Z"<< "\033[0m" <<std::endl;
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

    void WarehouseManager::wakeup_suppliers(int semid)
    {
        // zakoncz prace magazynu
        utils::semafor_set(semid, sem_wareohuse_working, 0);

        // obudz dostawcow
        utils::semafor_set(semid, sem_wolne_miejsca_x, 2);
        utils::semafor_set(semid, sem_wolne_miejsca_y, 2);
        utils::semafor_set(semid, sem_wolne_miejsca_z, 2);
    }

    void WarehouseManager::wakeup_machines(int semid)
    {
        // zakoncz prace fabryki
        utils::semafor_set(semid, sem_wareohuse_working, 0);

        // obudz dostawcow
        utils::semafor_set(semid, sem_dostepne_x, 2);
        utils::semafor_set(semid, sem_dostepne_y, 2);
        utils::semafor_set(semid, sem_dostepne_z, 2);
    }

}





