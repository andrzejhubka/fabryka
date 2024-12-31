#include "factory.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <utilities.h>
#include <warehouse.h>

int main()
{
    Factory fabryka;
    return 0;
}

Factory::Factory()
:   m_magazyn(warehouse(20, 0))
{
    // generujemy klucz ipc
    m_key_ipc = ftok("/tmp", 32);

    // probojemy podlaczyc sie do semaforow:
    m_sem_id = utils::get_semid(m_key_ipc);

    // probojemy podlaczyc sie do kolejki
    m_msg_id = utils::get_msid(m_key_ipc);

    // ladujemy magazyn z pliku
    m_magazyn.load_state("/home/andrzej/Documents/SO/fabryka/data/warehouse_state");


    // utworz watki dla maszyn A i B
    worker_a_THREAD = std::thread(&Factory::thread_worker_a, this);
    //std::thread worker_b_THREAD(&Factory::thread_worker_b, this);

    // utworz watek dla magazynu
    warehouse_THREAD = std::thread (&warehouse::working_thread, &m_magazyn);


}

Factory::~Factory()
{
    worker_a_THREAD.join();
    //worker_b_THREAD.join();
    warehouse_THREAD.join();
}


void Factory::thread_worker_a()
{
    // gdy pobierzemy produkty przechowywujemy je tutaj
    utils::Product containter_x = utils::Product(0, utils::X, 10 );
    utils::Product containter_y = utils::Product(0, utils::Y, 10 );
    utils::Product containter_z = utils::Product(0, utils::Z, 10);

    while (true)
    {
        // pobieramy X
        m_magazyn.grab_x(containter_x);


        // pobieramy y
        m_magazyn.grab_y(containter_y);

        // pobieramy z
        m_magazyn.grab_z(containter_z);


        // gdy masz juz produkty wyprodokuj cos i napisz na konsoli
        sleep(4);
        std::cout<<"Maszyna A. Wyprodukowano produkt z x, y, z. Wazy: \n";//<<containter_x.m_weight+containter_y.m_weight+containter_z.m_weight<<" kg.\n";
    }

}
void Factory::thread_worker_b()
{
    // gdy pobierzemy produkty przechowywujemy je tutaj
    utils::Product containter_x = utils::Product(0, utils::X, 10 );
    utils::Product containter_y = utils::Product(0, utils::Y, 10 );
    utils::Product containter_z = utils::Product(0, utils::Z, 10);

    while (true)
    {
        // pobieramy X
        utils::semafor_p(m_sem_id, sem_available_x, 1);
        // pobierz cos z magazynu
        this->m_magazyn.grab_x(containter_x);


        // pobieramy y
        utils::semafor_p(m_sem_id, sem_available_y, 1);

        // pobierz cos z magazynu
        this->m_magazyn.grab_y(containter_y);


        // pobieramy z
        utils::semafor_p(m_sem_id, sem_available_z, 1);

        // pobierz cos z magazynu
        this->m_magazyn.grab_z(containter_z);


        // gdy masz juz produkty wyprodokuj cos i napisz na konsoli
        sleep(8);
        std::cout<<"Maszyna B. Wyprodukowano produkt z x, y, z. Wazy: "<<containter_x.m_weight+containter_y.m_weight+containter_z.m_weight<<" kg.\n";
    }
}




