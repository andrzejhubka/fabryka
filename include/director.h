#include "utilities.h"
#include "warehouse.h"
#ifndef DIRECTOR_H
#define DIRECTOR_H


// ------ ustawienia ---------
#define MAX_SHARED_RAM 0.1 // procentowo, 1 to calosc
#define UNIT_SIZE 2; // wielkoscc jednej jednostki magazynowej w bajtach

class director
{
    public:
        director(long warehouse_capacity_units);
        ~director();

    // klucz ipc
    key_t m_key_ipc;
    int m_semid;
    utils::PamiecWspoldzielona m_shared;
    warehouse::WarehouseManager m_warehouse;

    // informajca czy dyrektor pracuje
    bool m_run;

    void main_loop();
    void stop_working();

};


#endif //DIRECTOR_H
