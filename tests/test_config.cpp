#include "../Glacier/config.h"
#include "../Glacier/log.h"
#include <iostream>

Glacier::ConfigVar<int>::ptr g_int_value_config = Glacier::Config::Lookup("system.port", (int)8080, "system port");

Glacier::ConfigVar<float>::ptr g_float_value_config = Glacier::Config::Lookup("system.port", (float)10.2f, "system port");

int main(int argc, char** argv) {
    GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << g_int_value_config->getValue();
    GLACIER_LOG_INFO(GLACIER_LOG_ROOT()) << g_float_value_config->toString();

    return 0;
}
