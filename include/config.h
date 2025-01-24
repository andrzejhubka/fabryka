#ifndef CONFIG_H
#define CONFIG_H

// sciezka do pliku z magazynem
#define WAREHOUSE_PATH "warehouse_data"

// rodzaje bledow & wynikow
#define IPC_RESULT_ERROR -1

// zwracane przez warehouse
#define MACHINE_RECIEVED_PRODUCT 0
#define WAREHOUSE_SUCCESFUL_INSERT 0
#define WAREHOUSE_CLOSED 1

// definicja semaforow
#define sem_dostepne_x 0
#define sem_dostepne_y 1
#define sem_dostepne_z 2

#define sem_wolne_miejsca_x 3
#define sem_wolne_miejsca_y 4
#define sem_wolne_miejsca_z 5

#define sem_shelf_x 6
#define sem_shelf_y 7
#define sem_shelf_z 8

#define sem_wareohuse_working 9

// rodzaje komend dyrektora
#define COMMAND_STOP_WAREHOUSE 1
#define COMMAND_STOP_FACTORY 2
#define COMMAND_STOP_WAREHOUSE_FACTORY_AND_SAVE 3
#define COMMAND_STOP_WAREHOUSE_FACTORY_NO_SAVE 4

// czasy wykonywania czynnosci -> w milisekundach
#define speed_machine_a 1000//10000
#define speed_machine_b 1000//10000
#define speed_supply_x 1000//1000
#define speed_supply_y 1000//1000
#define speed_supply_z 1000//1000






#endif //CONFIG_H
