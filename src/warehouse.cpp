#include "warehouse.h"
#include "utilities.h"
#include  <iostream>
#include <fstream>
#include <sys/sem.h>
#include <unistd.h>
#include <factory.h>


warehouse::warehouse(int capacity, int occupancy)
    : m_capacity(capacity), m_occupancy(occupancy)
{
    std::cout<<"\n=============== Warehouse: inicjalizacja ==============="<<std::endl;

    m_run = true;

    // generujemy klucz ipc
    m_key_ipc = ftok("/tmp", 32);

    // probojemy podlaczyc sie do semaforow:
    m_sem_id = utils::get_semid(m_key_ipc);

    // probojemy podlaczyc sie do kolejki
    m_msg_id = utils::get_msid(m_key_ipc);

    // ladujemy stan z pliku
    this->load_state("/home/andrzej/Documents/SO/fabryka/data/warehouse_state");

    // inicjujemy semafory
    utils::semafor_set(m_sem_id, sem_ordered_x, 0);
    utils::semafor_set(m_sem_id, sem_ordered_y, 0);
    utils::semafor_set(m_sem_id, sem_ordered_z, 0);

    // domyslnie magazyn jest maly i bedzie rosl wedlug zapotrzebowania
    m_max_x = capacity / 6;
    m_max_y = capacity / 6;
    m_max_z = capacity / 6;
    std::cout << "Utworzono magazyn o pojemnosci " << m_capacity << " jednostek. Maksymalna pojemnosc X" << m_max_x << " Y" << m_max_y << " Z" << m_max_z << std::endl;

    std::cout<<"======================= SUKCES =======================\n"<<std::endl;
}



warehouse::~warehouse()
{
    // zapisujemy stan do pliku
    save_state("/home/andrzej/Documents/SO/fabryka/data/warehouse_state");
}

void warehouse::load_state(const std::string& filePath)
{
    return;
    /*std::ifstream file(filePath);

    // sprawdzenie poprawnosci otwarcia pliku
    if (!file.is_open())
    {
        std::cerr << "Nie można otworzyć pliku: " << filePath << std::endl;
        return;
    }

    //bufor na linie w pliku
    std::string line;

    // wczytujemy naglowki, ale niz z nimi nie robimy
    std::getline(file, line);

    // wczutujemy pojemnosc magazynu, ilosc produktow x, y, z

    if (std::getline(file, line))
    {
        // klasa umozliwiajaca parsowanie danych
        std::istringstream ss(line);
        char comma; // dane z magazynu zapisalem jako csv
        std::cout<<line<<std::endl;
        // Odczytujemy dane z pliku (capacity, X, Y, Z)
        if (ss >> m_capacity >> comma >> m_X >> comma >> m_Y >> comma >> m_Z)
        {
            std::cout << "Stan magazynu załadowany.";
            std::cout << "Pojemność: " << m_capacity;
            std::cout << " X: " << m_X << ", Y: " << m_Y << ", Z: " << m_Z << std::endl;
        }
        else
        {
            std::cerr << "Błąd w formacie danych w pliku: " << filePath << std::endl;
        }
    }*/
}

void warehouse::save_state(const std::string& filePath) const
{
    return;
    /*// ten tryb nadpisuje zawartosc pliku
    std::ofstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Nie mozna otworzyc pliku do zapisu: " << filePath << std::endl;
        return;
    }

    // zapis naglowkow
    file << "capacity,X,Y,Z" << std::endl;

    // zapis stanu magazunu
    file << m_capacity << ',' << m_X << ',' << m_Y << ',' << m_Z << std::endl;

    std::cout << "Zapisano stan magazynu" << std::endl;

    // zamkniecie pliku
    file.close();*/
}

void warehouse::working_thread()
{
    // lepiej inicjowac pojemnik na przychodzace produkty na stosie watku gdyby trzebabylo przerabiac program i tworzyc wiecej pracownikow magazynu
    utils::Product package = utils::Product(0, utils::X, 10);

    while (m_run)
    {
        // jesli jest cos w kolejce
        if (utils::receive_product_from_queue(m_msg_id, package, 1) == 0)
        {
            insert_into_shelf(package);
        }

        // zloz zamowienie
        make_order();
    }

    std::cout<<"\nMagazyn zakonczyl prace!"<<std::endl;

}

void warehouse::insert_into_shelf(utils::Product& package)
{
    // umiesc to na polce (nie blokuj innych polek!)
    sleep(2); // czas dzialania pracownika
    switch (package.m_type)
    {
    case utils::X:
        {
            std::lock_guard<std::mutex> lock(mutex_shelf_x);
            m_products_x.emplace_back(package);
            change_occupancy(1);
            std::cout<<"Magazyn: polozono produkt X na polce. Nowa objetosc: "<<m_occupancy<<std::endl;
            cv_shelf_x.notify_one();
            break;
        };
    case utils::Y:
        {
            std::lock_guard<std::mutex> lock(mutex_shelf_y);
            m_products_y.emplace_back(package);
            change_occupancy(2);
            std::cout<<"Magazyn: polozono produkt Y na polce. Nowa objetosc: "<<m_occupancy<<std::endl;
            cv_shelf_y.notify_one();
            break;
        };
    case utils::Z:
        {
            std::lock_guard<std::mutex> lock(mutex_shelf_z);
            m_products_z.emplace_back(package);
            change_occupancy(3);
            std::cout<<"Magazyn: polozono produkt Z na polce. Nowa objetosc: "<<m_occupancy<<std::endl;
            cv_shelf_z.notify_one();
            break;
        };
    }
}

int warehouse::grab_x(utils::Product& container)
{
    // Blokujemy mutex używając std::unique_lock
    std::unique_lock<std::mutex> lock(mutex_shelf_x);

    // maszyna moze pobrac produkt. Zaczyna czekac. Trzeba sprawdzic czy sygnal dotyczy konca pracy czy pojawienia sie prudktu
    cv_shelf_x.wait(lock, [this] { return !m_products_x.empty() || !machine_a_run;});

    if (!machine_a_run)
    {
        std::cout << "Magazyn: maszyna przeznaczona do wylaczenia. Odmowiono wydania produktu z\n";
        return -1;
    }

    container = m_products_x.back();
    m_products_x.pop_back();
    change_occupancy(-1);
    std::cout << "Magazyn: wydano produkt x.\n";
    return 1;
}
int warehouse::grab_y(utils::Product& container)
{
    // Blokujemy mutex używając std::unique_lock
    std::unique_lock<std::mutex> lock(mutex_shelf_y);

    // maszyna moze pobrac produkt. Zaczyna czekac. Trzeba sprawdzic czy sygnal dotyczy konca pracy czy pojawienia sie prudktu
    cv_shelf_y.wait(lock, [this] { return !m_products_y.empty() || !machine_a_run;});

    if (!machine_a_run)
    {
        std::cout<<"Magazyn: maszyna przeznaczona do wylaczenia. Odmowiono wydania produktu y\n";
        return -1;
    }

    container = m_products_y.back();
    m_products_y.pop_back();
    change_occupancy(-1);
    return 1;
}
int warehouse::grab_z(utils::Product& container)
{
    // Blokujemy mutex używając std::unique_lock
    std::unique_lock<std::mutex> lock(mutex_shelf_z);

    // maszyna moze pobrac produkt. Zaczyna czekac. Trzeba sprawdzic czy sygnal dotyczy konca pracy czy pojawienia sie prudktu
    cv_shelf_z.wait(lock, [this] { return !m_products_z.empty() || !machine_a_run;});

    if (!machine_a_run)
    {
        std::cout<<"Magazyn: maszyna przeznaczona do wylaczenia. Odmowiono wydania produktu z\n";
        return -1;
    }

    container = m_products_z.back();
    m_products_z.pop_back();
    change_occupancy(-1);
    std::cout << "Magazyn: wydano produkt z.\n";
    return 1;
}


void warehouse::change_occupancy(int add_value)
{
    std::lock_guard<std::mutex> lock(mutex_occupancy);
    m_occupancy+=add_value;
}

void warehouse::make_order()
{
    int new_order_x, new_order_y, new_order_z;
    {
        std::lock_guard<std::mutex> lock(mutex_shelf_x);
        sleep(1);
        new_order_x = m_max_x -  utils::semafor_value(m_sem_id, sem_ordered_x) - m_products_x.size();

        if (new_order_x > 0)
        {
            std::cout<<"Magazyn: Zamawiam x: "<<new_order_x<<std::endl;
            utils::semafor_v(m_sem_id, sem_ordered_x, new_order_x);
        }

    }
    {
        std::lock_guard<std::mutex> lock(mutex_shelf_y);
        sleep(1);

        new_order_y =  m_max_y-  utils::semafor_value(m_sem_id, sem_ordered_y) - m_products_y.size();

        if (new_order_y > 0)
        {
            std::cout<<"Magazyn: zamawiam y: "<<new_order_y<<std::endl;
            utils::semafor_v(m_sem_id, sem_ordered_y, new_order_y+1);
        }
    }
    {
        std::lock_guard<std::mutex> lock(mutex_shelf_z);
        sleep(1);

        new_order_z = m_max_z -  utils::semafor_value(m_sem_id, sem_ordered_z) - m_products_z.size();

        if (new_order_z > 0)
        {
            std::cout<<"Magazyn: zamawiam z: "<<new_order_z<<std::endl;
            utils::semafor_v(m_sem_id, sem_ordered_z, new_order_z+1);
        }
    }
}

void warehouse::stop_working(bool save)
{
    // wycofaj wszystkie dostawy
    utils::semafor_set(m_sem_id, sem_ordered_x, 0);
    utils::semafor_set(m_sem_id, sem_ordered_y, 0);
    utils::semafor_set(m_sem_id, sem_ordered_z, 0);


    //
}

void warehouse::wake_machines()
{
    cv_shelf_x.notify_all();
    cv_shelf_y.notify_all();
    cv_shelf_z.notify_all();
    std::cout<<"Magazyn: wybudzono maszyny. Sprawdza czy nie maja sie wyslaczyc\n";
}




