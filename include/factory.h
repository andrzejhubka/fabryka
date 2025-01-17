#ifndef FACTORY_H
#define FACTORY_H
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <utilities.h>
#include <warehouse.h>
#include <mutex>
#include <thread>
#include <atomic>

// praca maszyn
extern bool machine_a_run;
extern bool machine_b_run;


class Factory {
  public:
    Factory();
    ~Factory();

    private:

    // dane do ipc
    key_t m_key_ipc;
    int m_sem_id;

    // zarzadzanie magazynem
    warehouse::WarehouseManager m_magazyn;

    // watki
    std::thread worker_a_THREAD;
    std::thread worker_b_THREAD;
    std::thread warehouse_THREAD;
    std::thread manager_THREAD;

    // funckje maszyn
    void thread_worker_a();
    void thread_worker_b();

    // manager czekajacy na polecenia od kierownika
    void thread_manager();

    // zakonczenie pracy tylko fabryki; maszyny a i b;
    void stop_workring();

};


#endif //FACTORY_H
