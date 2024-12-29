#include "supplier.h"
#include <thread>
#include <sys/ipc.h>
#include "utilities.h"

// definicja semaforow


int main()
{
    Supplier Supplier;
}

Supplier::Supplier()
{
    // generujemy klucz ipc
    m_key_ipc = ftok("/tmp", 32);

    // probojemy podlaczyc sie do semaforow:
    m_sem_id = utils::get_semid(m_key_ipc);

    // probojemy podlaczyc sie do kolejki
    m_msg_id = utils::get_msid(m_key_ipc);


    // utworzenie trzech źrodel (watkow) wysylania produktow
    threads.emplace_back(&Supplier::supply_x, this);
    threads.emplace_back(&Supplier::supply_y, this);
    threads.emplace_back(&Supplier::supply_z, this);

}
Supplier::~Supplier()
{
    // zanim usuniemy dostawce, czekamy az skoncza sie wszystkie watki wysylajace produkty
    for (auto& t: threads)
    {
        if (t.joinable())
            t.join();
    }
}


void Supplier::supply_x()
{
    // generuj produkt x

    // UWAGA: SEMAFORP W ODWROTNEJ KOLEJNOSCI: POTENCJALNY DEADLOCK.
    // DOSTAWCA MOZE ZAJAC KOLEJKE, FABRYKA BEDZIE CZEKALA NA PRODUKTY I NIE DA ZAMOWIEN, A DOSTAWCA BEDZIE TEZ NA NIE CZEKAL
    utils::semafor_p(m_sem_id, sem_ordered_x, 1);
    utils::semafor_p(m_sem_id, sem_queue, 1);
    // wyslij produkt x
    utils::semafor_v(m_sem_id, sem_queue, 1);

}
void Supplier::supply_y()
{
    // generuj produkt y
    utils::semafor_p(m_sem_id, sem_ordered_y, 1);
    // wyslij produkt y
}
void Supplier::supply_z()
{
    // generuj produkt z
    utils::semafor_p(m_sem_id, sem_ordered_z, 1);

    // wyslij produkt z
}
