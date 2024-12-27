//
// Created by andrzej on 12/21/24.
//

#ifndef SUPPLIER_H
#define SUPPLIER_H

#include <thread>
#include <vector>


class Supplier
{
    public:
        Supplier();
        ~Supplier();

        void supply_x();
        void supply_y();
        void supply_z();

    private:
        std::vector<std::thread> threads;
};



#endif //SUPPLIER_H
