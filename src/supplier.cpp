//
// Created by andrzej on 12/21/24.
//

#include "supplier.h"

#include <iostream>
#include <thread>

int main()
{
    Supplier Supplier;
}


Supplier::Supplier()
{
    // infromacja o pojawieniu sie procesu dostawcy
    printf("Tworzenie procesu dostawcy\n");

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

}
void Supplier::supply_y()
{

}
void Supplier::supply_z()
{

}
