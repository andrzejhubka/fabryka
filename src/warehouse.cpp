//
// Created by andrzej on 12/21/24.
//

#include "warehouse.h"
#include  <iostream>
#include <fstream>
#include <sstream>

int main()
{
    warehouse magazyn(20, 8);
    std::cout << "Hello World!" << std::endl;
}


warehouse::warehouse(int capacity, int occupancy)
    : m_capacity(capacity), m_occupancy(occupancy)
{
    //this->load_state("/home/andrzej/Documents/SO/fabryka/data/warehouse_state");
    m_X=0;
    m_Y=0;
    m_Z=0;
    std::cout << "Utworzono magazyn o pojemnosci " << m_capacity << " jednostek" << std::endl;
}
warehouse::~warehouse()
{
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
void warehouse::save_state(std::string filePath)
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


