#ifndef FACTORY_H
#define FACTORY_H

#include <sys/types.h>
#include <iostream>

class warehouse
{
public:
    warehouse(int capacity=0, int occupancy=0);
    ~warehouse();

    int m_capacity;
    int m_occupancy;
    int m_X,m_Y,m_Z; // licznik produktow
    key_t m_key_ipc;
    int m_sem_id;

    void save_state(const std::string& filePath) const;
    void load_state(const std::string& filePath);
    void expand(int newCapacity);

};

class Factory {
  public:
    Factory();
    ~Factory();

    private:

    // dane do ipc
    key_t m_key_ipc;
    int m_sem_id;

    // zarzadzanie magazynem
    warehouse m_magazyn;

    // watki maszyn
    int thread_worker_a();
    int thread_worker_b();

};


#endif //FACTORY_H
