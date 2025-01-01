#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <iostream>
#include <mutex>
#include <sys/sem.h>
#include <vector>
#include <utilities.h>
#include <condition_variable>


class warehouse
{
public:
    warehouse(int capacity=20, int occupancy=0);
    ~warehouse();

    // czy magazyn pracuje
    bool m_run;

    // pojemnosc i rozmiar
    int m_capacity;
    int m_occupancy;

    // ochrona synchronizacji do pojemnosci/rozmiaru
    std::mutex mutex_occupancy;
    std::mutex mutex_capacity;

    // półki z produktami
    std::vector<utils::Product> m_products_x;
    std::vector<utils::Product> m_products_y;
    std::vector<utils::Product> m_products_z;

    // ochrona polek
    std::mutex mutex_shelf_x;
    std::mutex mutex_shelf_y;
    std::mutex mutex_shelf_z;
    std::condition_variable cv_shelf_x;
    std::condition_variable cv_shelf_y;
    std::condition_variable cv_shelf_z;


    // domyslne limity na polce
    int m_max_x;
    int m_max_y;
    int m_max_z;

    // mechanizmy ipc
    key_t m_key_ipc;
    int m_sem_id;
    int m_msg_id;

    // glowna petla dzialajaca jako watek magazynu
    void working_thread();

    // pobranie produktu z kolejki i umieszczenie na polce
    void insert_into_shelf(utils::Product& package);

    // wydawanie produktow pracownikom
    int grab_x(utils::Product& container);
    int grab_y(utils::Product& container);
    int grab_z(utils::Product& container);

    // zamiana zajmowanego miejsca w magazynie
    void increase_occupancy(int amount);
    void decrease_occupancy(int amount);
    // zwiekszenie pojemnosci magazynu
    void expand(int newCapacity);

    // zwiekszenie zajetosci calego magazynu (suma kazdej półki)
    void change_occupancy(int add_value);

    // zamow produkty
    void make_order();

    // zapisanie stanu do pliku
    void save_state(const std::string& filePath) const;

    // wczytanie stanu z pliku
    void load_state(const std::string& filePath);

    // zatrzymanie magazynu (pracownika rozlawowywujacego dostawy; maszyny dalej moga pobierac produkty z magazynu)
    void stop_working(bool save);

    // obudz czekajace na produkty maszyny, zeby sprawdzily czy sa tez wylaczone
    void wake_machines();
};




#endif //WAREHOUSE_H
