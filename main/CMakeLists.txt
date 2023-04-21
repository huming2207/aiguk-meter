idf_component_register(
        SRCS
            "main.cpp"
            "hal/mhz19e.cpp" "hal/mhz19e.hpp"
            "hal/bme68x/bme68x.c" "hal/bme68x/bme68x.h" "hal/bme68x/bme68x_defs.h"
            "hal/bme680.cpp" "hal/bme680.hpp"

        INCLUDE_DIRS
            "." "hal"
)
