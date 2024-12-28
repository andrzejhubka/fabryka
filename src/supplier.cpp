#include "supplier.h"
#include <thread>
#include <sys/ipc.h>
#include "utilities.h"

// definicja semaforow
#define sem_ordered_x 0
#define sem_ordered_y 1
#define sem_ordered_z 2


int main()
{
    Supplier Supplier;
}

Supplier::Supplier()
{
    // generujemy klucz ipc
    m_key_ipc = ftok("/tmp", 32);

    // sprawdzamy czy proces dyrektor dziala:
    m_sem_id = utils::get_semid(m_key_ipc);

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
    struct X
    {
        int occupied_space = 1;
        int weight =
    };
    // wyslij produkt x
}
void Supplier::supply_y()
{

}
void Supplier::supply_z()
{

}
