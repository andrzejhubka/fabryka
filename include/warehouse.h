#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <iostream>
#include <sys/sem.h>

class warehouse
{
    public:
        warehouse(int capacity, int occupancy);
        ~warehouse();

    int m_capacity;
    int m_occupancy;
    int m_X,m_Y,m_Z; // licznik produktow
    key_t m_key_ipc;
    int m_sem_id;

    void save_state(const std::string& filePath) const;
    void load_state(const std::string& filePath);
    void expand(int newCapacity);
    void initialize();

};



#endif //WAREHOUSE_H
