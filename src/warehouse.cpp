#include "warehouse.h"
#include "utilities.h"
#include  <iostream>
#include <fstream>
#include <sstream>
#include <sys/sem.h>
#include <unistd.h>
#include <factory.h>

warehouse::warehouse(int capacity, int occupancy)
    : m_capacity(capacity), m_occupancy(occupancy)
{
    // generujemy klucz ipc
    m_key_ipc = ftok("/tmp", 32);

    // probojemy podlaczyc sie do semaforow:
    m_sem_id = utils::get_semid(m_key_ipc);

    // probojemy podlaczyc sie do kolejki
    m_msg_id = utils::get_msid(m_key_ipc);

    // ladujemy stan z pliku
    this->load_state("/home/andrzej/Documents/SO/fabryka/data/warehouse_state");

    // inicjujemy semafory
    utils::semafor_set(m_sem_id, sem_available_x, 0);
    utils::semafor_set(m_sem_id, sem_available_y, 0);
    utils::semafor_set(m_sem_id, sem_available_z, 0);

    std::cout << "Utworzono magazyn o pojemnosci " << m_capacity << " jednostek" << std::endl;


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
    utils::Product package = utils::Product(0,utils::X,2);

    while (true)
    {

        // pobierz to co przyszlo z kolejki
        sleep(1); // prawcownik magazynu tez potrzebuje czasu
        utils::receive_product_from_queue(m_msg_id, package, 1);
        // umiesc to na polce (nie blokuj innych polek!)

        switch (package.m_type)
        {
            case utils::X:
                {
                    mutex_shelf_x.lock();
                    m_products_x.emplace_back(package);
                    std::cout<<"Magazyn: polozono produkt X na polce\n";
                    mutex_shelf_x.unlock();
                    break;
                };
            case utils::Y:
                {
                    mutex_shelf_y.lock();
                    m_products_x.emplace_back(package);
                    std::cout<<"Magazyn: polozono produkt Y na polce\n";
                    mutex_shelf_y.unlock();
                    break;
                };
            case utils::Z:
                {
                    mutex_shelf_z.lock();
                    m_products_x.emplace_back(package);
                    std::cout<<"Magazyn: polozono produkt Z na polce\n";
                    mutex_shelf_z.unlock();
                    break;
                };
        }

    }

}



