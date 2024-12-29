#include <iostream>
#include "director.h"
#include "utilities.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    director zarzadca;
    sleep(50);
    return 0;
}

director::director()
{
    // generowaie klucza
    key_ipc = ftok("/tmp", 32);
    std::cout << "key_ipc: " << key_ipc << std::endl;
    // zainicjuj zbior semaforow
    if ((semid = utils::utworz_zbior_semaforow(key_ipc, 8)) < 0)
    {
        std::cerr << "Error in key_ipc" << std::endl;
        exit(-1);
    }

    std::cout << "semid: " << semid << std::endl;

    // stworz kolejke komunikatow miedzy fabryka a dostawca
    if ((memid= utils::utworz_kolejke(key_ipc)) < 0)
    {
        std::cerr << "Error in key_ipc" << std::endl;
        exit(-1);
    }

    std::cout << "memid: " << memid << std::endl;

}
director::~director()
{
    utils::usun_zbior_semaforow(this->semid);
    utils::usun_kolejke(this->memid);
}


void director::polecenie_1()
{
    return;
}
void director::polecenie_2()
{
    return;
}
void director::polecenie_3()
{
    return;
}
void director::polecenie_4()
{
    return;
}
