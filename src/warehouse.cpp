#include "warehouse.h"
#include "utilities.h"
#include  <iostream>
#include <fstream>
#include <sstream>
#include <sys/sem.h>
#include <unistd.h>

// definicja semaforow
#define sem_ordered_x 0
#define sem_ordered_y 1
#define sem_ordered_z 2

#define sem_available_x 3
#define sem_available_y 4
#define sem_available_z 5

int main()
{
    warehouse magazyn(20, 8);
}


warehouse::warehouse(int capacity, int occupancy)
    : m_capacity(capacity), m_occupancy(occupancy)
{
    // generujemy klucz ipc
    m_key_ipc = ftok("/tmp", 32);

    // sprawdzamy czy proces dyrektor dziala:
    m_sem_id = utils::get_semid(m_key_ipc);

    // ladujemy stan z pliku
    this->load_state("/home/andrzej/Documents/SO/fabryka/data/warehouse_state");

    // inicjujemy semafory
    utils::semafor_set(m_sem_id, sem_available_x, m_Y);
    utils::semafor_set(m_sem_id, sem_available_y, m_X);
    utils::semafor_set(m_sem_id, sem_available_z, m_Z);

    std::cout << "Utworzono magazyn o pojemnosci " << m_capacity << " jednostek" << std::endl;

    // zainicjuj numery semaforow

}

warehouse::~warehouse()
{
    // zapisujemy stan do pliku
    save_state("/home/andrzej/Documents/SO/fabryka/data/warehouse_state");
}

void warehouse::load_state(const std::string& filePath)
{
    std::ifstream file(filePath);

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
    }
}

void warehouse::save_state(const std::string& filePath) const
{
    // ten tryb nadpisuje zawartosc pliku
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
    file.close();
}

void warehouse::initialize()
{
	// tworzymy watek magazynu

}


