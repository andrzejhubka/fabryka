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
    key_t m_key_ipc;
    int m_semid;
    int m_memid;

    // informajca czy dyrektor pracuje
    bool m_run;

    void main_loop();
    void stop_working();

};


#endif //DIRECTOR_H
