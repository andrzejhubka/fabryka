//
// Created by andrzej on 12/21/24.
//

#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <iostream>


class warehouse
{
    public:
        warehouse(int capacity, int occupancy);
        ~warehouse();

    int m_capacity;
    int m_occupancy;
    int m_X,m_Y,m_Z; // licznik produktow

    void save_state(std::string filePath);
    void load_state(const std::string& filePath);
    void expand(int newCapacity);

};



#endif //WAREHOUSE_H
