#ifndef SUPPLIER_H
#define SUPPLIER_H
#include <thread>
#include <vector>
#include <sys/types.h>
#include "warehouse.h"
#include <mutex>

// praca dostawcow
extern bool supplier_x_run;
extern bool supplier_y_run;
extern bool supplier_z_run;


class Supplier
{
    public:
        Supplier();
        ~Supplier();

        void supply_x();
        void supply_y();
        void supply_z();

    // dane do ipc
    key_t m_key_ipc;
    int m_sem_id;

    // api do magazynu
    warehouse::WarehouseManager m_warehouse;

    // ilosc wyprodukowanych sztuk
    int m_produced;
    std::thread supplier_x_THREAD;
    std::thread supplier_y_THREAD;
    std::thread supplier_z_THREAD;

};



#endif //SUPPLIER_H
