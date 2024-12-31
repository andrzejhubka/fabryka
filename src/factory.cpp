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
    std::cout<<"\n=============== Factory: inicjalizacja ==============="<<std::endl;
    // generujemy klucz ipc
    m_key_ipc = ftok("/tmp", 32);

    // probojemy podlaczyc sie do semaforow:
    m_sem_id = utils::get_semid(m_key_ipc);

    // probojemy podlaczyc sie do kolejki
    m_msg_id = utils::get_msid(m_key_ipc);

    // ladujemy magazyn z pliku
    m_magazyn.load_state("/home/andrzej/Documents/SO/fabryka/data/warehouse_state");


    // watek managera
    manager_THREAD = std::thread(&Factory::thread_manager, this);

    // utworz watki dla maszyn A i B
    worker_a_THREAD = std::thread(&Factory::thread_worker_a, this);
    worker_b_THREAD = std::thread(&Factory::thread_worker_b, this);

    // utworz watek dla magazynu
    warehouse_THREAD = std::thread (&warehouse::working_thread, &m_magazyn);

    std::cout<<"======================= SUKCES =======================\n"<<std::endl;


}

Factory::~Factory()
{
    worker_a_THREAD.join();
    worker_b_THREAD.join();
    warehouse_THREAD.join();
    manager_THREAD.join();
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
        std::cout<<"Maszyna A: Wyprodukowano produkt z x, y, z. Wazy: "<<containter_x.m_weight+containter_y.m_weight+containter_z.m_weight<<" kg.\n";
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
        m_magazyn.grab_x(containter_x);

        // pobieramy y
        m_magazyn.grab_y(containter_y);

        // pobieramy z
        m_magazyn.grab_z(containter_z);

        // gdy masz juz produkty wyprodokuj cos i napisz na konsoli
        sleep(7);
        std::cout<<"Maszyna B: Wyprodukowano produkt z x, y, z. Wazy: "<<containter_x.m_weight+containter_y.m_weight+containter_z.m_weight<<" kg.\n";
    }

}
void Factory::thread_manager()
{
    int command;
    while (true)
    {
        // czekaj az semafor polecenia zostanie zmieniony
        utils::semafor_p(m_sem_id, sem_command, 1);

        command = utils::semafor_value(m_sem_id, sem_command);

        switch (command)
        {
            case 0:
                {
                    std::cout<<"POLECENIE 1!!\n";
                    break;
                }
            case 1:
                {
                    std::cout<<"POLECENIE 2!!\n";
                    break;
                }
            case 2:
                {
                    std::cout<<"POLECENIE 3!!\n";
                    break;
                }
            case 3:
                {
                    std::cout<<"POLECENIE 4!!\n";
                    break;
                }
        }
        utils::semafor_set(m_sem_id, sem_command, 0);
    }
}





