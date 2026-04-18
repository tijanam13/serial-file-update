#include "../Biblioteka/adts.h"
#include "../Biblioteka/misc.h"

int main(void) {

    adts_set_update_all_action(procesuirajSlucaj3);

    adts_run_main_menu();

    return 0;
}