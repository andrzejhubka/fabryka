#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <iostream>
#include <sys/sem.h>
#include <vector>
#include <utilities.h>


class warehouse
{
public:
    warehouse(int capacity=0, int occupancy=0);
    ~warehouse();

    int m_capacity;
    int m_occupancy;

    // półki z produktami
    std::vector<utils::Product> m_products_x;
    std::vector<utils::Product> m_products_y;
    std::vector<utils::Product> m_products_z;

    key_t m_key_ipc;
    int m_sem_id;
    int m_msg_id;

    void save_state(const std::string& filePath) const;
    void load_state(const std::string& filePath);
    void expand(int newCapacity);
    void working_thread();

};



#endif //WAREHOUSE_H
