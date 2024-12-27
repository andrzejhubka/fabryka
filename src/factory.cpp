//
// Created by andrzej on 12/21/24.
//

#include "factory.h"
#include <iostream>
#include <thread>
#include <unistd.h>

int worker_a();
int worker_b();

int main()
{
    std::thread worker_a_THREAD(worker_a);
    std::thread worker_b_THREAD(worker_b);

    worker_a_THREAD.join();
    worker_b_THREAD.join();
    return 0;
}


int worker_a()
{
    while (true)
    {
        // pobierz z magazynu produkt x, y, z
        // wyprodukuj

    }

    return 0;
}
int worker_b()
{
    while (true)
    {
        // pobierz z magazynu produkt x, y, z
        // wyprodukuj

    }
}