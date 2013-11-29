#include "FirstApp.h"
#include "Hal.h"

static FirstApp_data_t dataVal = -40;

void main() {
    Hal_init();
    FirstApp_start();
    Hal_idleLoop();
}

/* -------- SCHEMA CALLBACKS -------- */

void FirstApp_connectHandler(void) {
    Hal_connected();
}

void FirstApp_disconnectHandler(void) {
    Hal_disconnected();
}

void FirstApp_data_fetch(FirstApp_data_t* output) {
    *output = dataVal;
}

void FirstApp_data_store(FirstApp_data_t* input) {
    dataVal = *input;
}
