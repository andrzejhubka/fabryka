#ifndef CONFIG_H
#define CONFIG_H

// sciezka do pliku z magazynem
#define WAREHOUSE_PATH "../../data/warehouse_data"

// plik do ktorego beda trafialy logi z pracy fabryki (zamiast do terminala)
# define REDIRECT_LOGS_TO_FILE false
#define LOG_PATH "../../data/log"

// czasy wykonywania czynnosci -> w milisekundach
#define speed_machine_a 1000//10000
#define speed_machine_b 1000//10000
#define speed_supply_x 100//1000
#define speed_supply_y 100//1000
#define speed_supply_z 100//1000

// maksymalna zajetosc pamieci ram przez magazyn w %
#define MAX_RAM 0.005

#endif //CONFIG_H
