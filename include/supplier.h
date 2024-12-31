#ifndef SUPPLIER_H
#define SUPPLIER_H
#include <thread>
#include <vector>
#include <sys/types.h>
#include <mutex>

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
    int m_msg_id;

    // ilosc wyprodukowanych sztuk
    int m_produced;

    // OCHRONA SYNCHRNOIZACJI
    std::mutex mutex_send;

    private:
        std::vector<std::thread> m_threads;

};



#endif //SUPPLIER_H
