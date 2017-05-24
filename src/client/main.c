/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#include <time.h>
#include "network_client.h"
#include "client_ui.h"

int main(int argc, char *argv[]) {
    if (argc < 2)
        ZF_LOGF_STR("Need the hostname/address of the server");
    else {
        init_client(argv[1]);
        init_gui();
    }
}