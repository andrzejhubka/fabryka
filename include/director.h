#include "utilities.h"
#include "warehouse.h"
#ifndef DIRECTOR_H
#define DIRECTOR_H


// ------ ustawienia ---------
#define MAX_SHARED_RAM 0.1 // procentowo, 1 to calosc
#define UNIT_SIZE 2 // wielkoscc jednej jednostki magazynowej w bajtach


// informajca czy dyrektor pracuje
void director(pid_t pid1, pid_t pid2, pid_t pid3, pid_t pid4, pid_t pid5);




#endif //DIRECTOR_H
