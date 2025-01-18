#include "supplier.h"
#include <thread>
#include <sys/ipc.h>
#include "utilities.h"
#include <mutex>
#include <unistd.h>

bool supplier_x_run{true};
bool supplier_y_run{true};
bool supplier_z_run{true};

#define test_sended_x_count 10000
#define test_sended_y_count 10000
#define test_sended_z_count 10000


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

    // api magazynu
    m_warehouse = warehouse::WarehouseManager(m_key_ipc, m_sem_id);

    // domyslnie producent nic nie wyprodukowal
    m_produced = 0;


    // watki
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
    sleep(6);
}


void Supplier::supply_x()
{
    // generuj produkt x
    utils::ProductX towar(0);

    for (int i = 0; i < test_sended_x_count; i++)
   // while (supplier_x_run)
    {
        usleep(speed_supply_x*100); // trwa produkcja
        // generuj produkt
        towar.m_weight = utils::random_number(1, 20);;
        // wyslij produkt
        switch(m_warehouse.insert_x(&towar))
        {
        case WAREHOUSE_CLOSED:
            {
                i = test_sended_x_count+1;
                supplier_x_run = false;
                break;
            }
        case WAREHOUSE_SUCCESFUL_INSERT:
            {
                std::cout<<"Supplier X: dostarczono produkt X"<<std::endl;
                break;
            }
        default:
            {
                i = test_sended_x_count+1;
                supplier_x_run = false;
                break;
            }
        }
    }
    std::cout<<"Supplier X: koniec pracy"<<std::endl;
}
void Supplier::supply_y()
{
    // generuj produkt x
    utils::ProductY towar(0);
    int id = 0;
    int waga = 10;

    for (int i = 0; i < test_sended_y_count; i++)
    //while (supplier_y_run)
    {
        usleep(speed_supply_y*100); // trwa produkcja
        // generuj produkt
        towar.m_weight = utils::random_number(1, 20);
        switch(m_warehouse.insert_y(&towar))
        {
        case WAREHOUSE_CLOSED:
            {
                i = test_sended_y_count+1;
                supplier_y_run = false;
                break;
            }
        case WAREHOUSE_SUCCESFUL_INSERT:
            {
                std::cout<<"Supplier Y: dostarczono produkt y"<<std::endl;
                break;
            }
        default:
            {
                i = test_sended_y_count+1;
                supplier_y_run = false;
                break;
            }
        }
    }
    std::cout<<"Supplier y: koniec pracy"<<std::endl;
}
void Supplier::supply_z()
{
    // generuj produkt x
    utils::ProductZ towar(0, 0);
    int id = 0;
    int waga = 10;

    for (int i = 0; i < test_sended_z_count; i++)
    //while (supplier_z_run)
    {
        usleep(speed_supply_z*100); // trwa produkcja
        // generuj produkt
        towar.m_weight = utils::random_number(1, 20);
        towar.m_weight = utils::random_number(1, 20);
        switch(m_warehouse.insert_z(&towar))
        {
            case WAREHOUSE_CLOSED:
            {
                i = test_sended_z_count+1;
                supplier_z_run = false;
                break;
            }
            case WAREHOUSE_SUCCESFUL_INSERT:
            {
                std::cout<<"Supplier Z: dostarczono produkt Z"<<std::endl;
                    break;
            }
            default:
            {
                i = test_sended_z_count+1;
                supplier_z_run = false;
                break;
            }
        }
    }
    std::cout<<"Supplier z: koniec pracy"<<std::endl;
}

//!!!!!!!!!!!!!!! koniec tego procesu wywala semafory?
