#include "utilities.h"
#include "warehouse.h"
#ifndef DIRECTOR_H
#define DIRECTOR_H


// ------ ustawienia ---------
#define MAX_SHARED_RAM 0.1 // procentowo, 1 to calosc

class director
{
    public:
        director(long warehouse_capacity);
        ~director();

    // klucz ipc
    key_t m_key_ipc;
    int m_semid;
    int m_memid;
    utils::PamiecWspoldzielona m_shared;

    // informajca czy dyrektor pracuje
    bool m_run;

    void main_loop();
    void stop_working();

};


#endif //DIRECTOR_H
