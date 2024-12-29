#include "factory.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <utilities.h>

int main()
{
    Factory fabryka;
    return 0;
}

Factory::Factory()
{
    // generujemy klucz ipc
    m_key_ipc = ftok("/tmp", 32);

    // sprawdzamy czy proces dyrektor dziala:
    m_sem_id = utils::get_semid(m_key_ipc);

    // utworz magazyn
    m_magazyn = warehouse(0, 0);
    m_magazyn.load_state("/home/andrzej/Documents/SO/fabryka/data/warehouse_state");
    m_magazyn.initialize();

    // utworz watki dla maszyn A i B
    std::thread worker_a_THREAD(thread_worker_a);
    std::thread worker_b_THREAD(thread_worker_b);

    worker_a_THREAD.join();
    worker_b_THREAD.join();
}

int Factory::thread_worker_a()
{
    while (true)
    {
        utils::semafor_p(this->m_sem_id, 7, 1);
        // pobierz cos z magazynu
        utils::semafor_v(this->m_sem_id, 7, 1);
        // wyprodokuj
        std::cout<<"Wyprodukowalem produkt: ";
        sleep(1);
    }

}
int Factory::thread_worker_b()
{
    while (true)
    {
        utils::semafor_p(this->m_sem_id, 7, 1);
        // x =
        // y =
        // z =
        utils::semafor_v(this->m_sem_id, 7, 1);
        // wyprodokuj
        std::cout<<"Wyprodukowalem produkt: ";
        sleep(1);
    }
}


