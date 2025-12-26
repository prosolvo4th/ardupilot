#pragma once

#include "AP_Thermal_config.h"

#if AP_THERMAL_ENABLED

#include <AP_HAL/AP_HAL.h>
#include <AP_HAL/utility/RingBuffer.h>

class AP_Thermal {
public:
    AP_Thermal();

    CLASS_NO_COPY(AP_Thermal);

    static AP_Thermal *get_singleton(void) { return _singleton; }

    bool init(void);
    void update(void);

    uint32_t available(void) const;
    uint32_t read(uint8_t *data, uint32_t len);
    bool read_byte(uint8_t &byte);

private:
    static AP_Thermal *_singleton;

    AP_HAL::UARTDriver *_uart = nullptr;
    ByteBuffer _rx_buffer;
    uint32_t _last_tx_ms = 0;

    void handle_uart_rx(void);
    void send_dummy_byte(void);
};

namespace AP {
    AP_Thermal *thermal();
};

#endif  // AP_THERMAL_ENABLED
