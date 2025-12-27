#include "AP_Thermal.h"

#if AP_THERMAL_ENABLED

#include <AP_SerialManager/AP_SerialManager.h>
#include <GCS_MAVLink/GCS.h>

AP_Thermal *AP_Thermal::_singleton;

AP_Thermal::AP_Thermal()
    : _rx_buffer(AP_THERMAL_RX_BUFFER_SIZE)
{
    _singleton = this;
}

bool AP_Thermal::init(void)
{
    if (_uart != nullptr) {
        return true;
    }

    _uart = AP::serialmanager().find_serial(AP_SerialManager::SerialProtocol_Thermal, 0);
    if (_uart == nullptr) {
        return false;
    }

    return true;
}

void AP_Thermal::update(void)
{
    if (_uart == nullptr) {
        return;
    }

    handle_uart_rx();
    // switch_palette();
}

uint32_t AP_Thermal::available(void) const
{
    return _rx_buffer.available();
}

uint32_t AP_Thermal::read(uint8_t *data, uint32_t len)
{
    return _rx_buffer.read(data, len);
}

bool AP_Thermal::read_byte(uint8_t &byte)
{
    return _rx_buffer.read_byte(&byte);
}

void AP_Thermal::handle_uart_rx(void)
{
    uint8_t buffer[32];

    while (true) {
        const uint32_t available = _uart->available();
        if (available == 0) {
            break;
        }

        const uint16_t to_read = (available > sizeof(buffer)) ? sizeof(buffer) : available;
        const ssize_t count = _uart->read(buffer, to_read);
        if (count <= 0) {
            break;
        }

        _rx_buffer.write(buffer, static_cast<uint32_t>(count));
    }
}

void AP_Thermal::switch_palette(int8_t step)
{
    if (_uart == nullptr) {
        return;
    }

    palette_counter += step;
    if (palette_counter < 0)
        palette_counter = palette_num - 1;
    else if (palette_counter >= palette_num)
        palette_counter = 0;
    palette_counter = palette_counter % palette_num;
    gcs().send_text(MAV_SEVERITY_INFO, "Send palette %d", palette_counter);

    _uart->write(palettes[palette_counter], palette_size);
}

namespace AP {
AP_Thermal *thermal()
{
    return AP_Thermal::get_singleton();
}
}

#endif  // AP_THERMAL_ENABLED
