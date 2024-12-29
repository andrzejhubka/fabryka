//
// Created by andrzej on 12/21/24.
//

#ifndef DIRECTOR_H
#define DIRECTOR_H


class director
{
    public:
        director();
        ~director();

    // klucz ipc
    key_t key_ipc;
    int semid;
    int memid;
    // zamkniecie magazynu
    void polecenie_1();

    // zamkniecie fabryki
    void polecenie_2();

    // zamkniecie magazynu i fabryki; zapisanie stanu magazynu
    void polecenie_3();

    // zamkniecie magazyniu i fabryki; brak zapisania stanu
    void polecenie_4();

};


#endif //DIRECTOR_H
