#include "supplier.h"
#include <thread>
#include <sys/ipc.h>
#include "utilities.h"
#include <mutex>
#include <unistd.h>

// definicja semaforow

std::mutex mutex_send;

int main()
{
    Supplier Supplier;
}

Supplier::Supplier()
{
    std::cout<<"\n=============== Supplier: inicjalizacja ==============="<<std::endl;
    // generujemy klucz ipc
    m_key_ipc = ftok("/tmp", 32);

    // probojemy podlaczyc sie do semaforow:
    m_sem_id = utils::get_semid(m_key_ipc);

    // probojemy podlaczyc sie do kolejki
    m_msg_id = utils::get_msid(m_key_ipc);

    // domyslnie producent nic nie wyprodukowal
    m_produced = 0;

    // utworzenie trzech źrodel (watkow) wysylania produktow
    m_threads.emplace_back(&Supplier::supply_x, this);
    m_threads.emplace_back(&Supplier::supply_y, this);
    m_threads.emplace_back(&Supplier::supply_z, this);
    std::cout<<"======================= SUKCES =======================\n"<<std::endl;

}
Supplier::~Supplier()
{
    // zanim usuniemy dostawce, czekamy az skoncza sie wszystkie watki wysylajace produkty
    for (auto& t: m_threads)
    {
        if (t.joinable())
            t.join();
    }
}


void Supplier::supply_x()
{
    // generuj produkt x
    utils::Product towar(0, utils::X, 0);
    int id = 0;
    int waga = 10;

    while (true)
    {
        // UWAGA: SEMAFORP W ODWROTNEJ KOLEJNOSCI: POTENCJALNY DEADLOCK.
        // DOSTAWCA MOZE ZAJAC KOLEJKE, FABRYKA BEDZIE CZEKALA NA PRODUKTY I NIE DA ZAMOWIEN, A DOSTAWCA BEDZIE TEZ NA NIE CZEKAL
        utils::semafor_p(m_sem_id, sem_ordered_x, 1);

        sleep(7); // trwa produkcja

        // generuj produkt
        waga = utils::random_number(1, 20);
        towar.set_id(0);
        towar.set_weight(waga);

        // wyslij produkt
        mutex_send.lock();
        utils::send_product_to_queue(m_msg_id, towar, 1);
        towar.describe();



        fflush(stdout);
        mutex_send.unlock();
    }
}
void Supplier::supply_y()
{
    // generuj produkt y
    utils::Product towar(0, utils::Y, 0);
    int id = 0;
    int waga = 10;

    while (true)
    {
        // UWAGA: SEMAFORP W ODWROTNEJ KOLEJNOSCI: POTENCJALNY DEADLOCK.
        // DOSTAWCA MOZE ZAJAC KOLEJKE, FABRYKA BEDZIE CZEKALA NA PRODUKTY I NIE DA ZAMOWIEN, A DOSTAWCA BEDZIE TEZ NA NIE CZEKAL
        utils::semafor_p(m_sem_id, sem_ordered_y, 1);

        sleep(5); // trwa produkcja

        // generuj produkt
        waga = utils::random_number(1, 20);
        towar.set_id(0);
        towar.set_weight(waga);

        // wyslij produkt
        mutex_send.lock();
        utils::send_product_to_queue(m_msg_id, towar, 1);
        towar.describe();
        fflush(stdout);
        mutex_send.unlock();
    }
}
void Supplier::supply_z()
{
    // generuj produkt y
    utils::Product towar(0, utils::Z, 0);
    int id = 0;
    int waga = 10;

    while (true)
    {
        // UWAGA: SEMAFORP W ODWROTNEJ KOLEJNOSCI: POTENCJALNY DEADLOCK.
        // DOSTAWCA MOZE ZAJAC KOLEJKE, FABRYKA BEDZIE CZEKALA NA PRODUKTY I NIE DA ZAMOWIEN, A DOSTAWCA BEDZIE TEZ NA NIE CZEKAL
        utils::semafor_p(m_sem_id, sem_ordered_z, 1);
        //utils::semafor_p(m_sem_id, sem_queue, 1);

        sleep(8); // trwa produkcja

        // generuj produkt
        waga = utils::random_number(1, 20);
        towar.set_id(0);
        towar.set_weight(waga);

        // wyslij produkt
        mutex_send.lock();
        utils::send_product_to_queue(m_msg_id, towar, 1);
        towar.describe();
        fflush(stdout);
        mutex_send.unlock();
    }
}
