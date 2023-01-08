/**
 * @author Katukiya G.
 *
 * @file terminal.c
 *
 * @brief UART and USB terminals control.
 */

#include "terminal.h"

#include <string.h>

#include "uart_terminal.h"
#include "usb_device.h"

static char __aligned(32) uartprintbuf[256 + 3] = {'\0'};
static char __aligned(32) usbprintbuf[256 + 3] = {'\0'};

/// @brief Init uart terminal
/// @note Usb terminal is inited by USB stack init
void terminal_init(void){
	uart_terminal_init();
}

/// @brief printf() in uart console
void uartconsole_printf(const char* fmt, ...){
    va_list args;
    size_t size = 0;
    va_start(args, fmt);
    int ret = vsnprintf(uartprintbuf, sizeof(uartprintbuf) - 3, fmt, args);
    va_end(args);
    if(ret < 0){
        return;
    }
    size += ret;
    uartprintbuf[size] = '\n';
    uartprintbuf[size+1] = '\r';
    uartprintbuf[size + 2] = '\0';

    uart_terminal_print(uartprintbuf);
}

/// @brief printf() in usb console
void usbconsole_printf(const char* fmt, ...){
    va_list args;
    size_t size = 0;
    va_start(args, fmt);
    int ret = vsnprintf(usbprintbuf, sizeof(usbprintbuf) - 3, fmt, args);
    va_end(args);
    if(ret < 0){
        return;
    }
    size += ret;
    usbprintbuf[size] = '\n';
    usbprintbuf[size+1] = '\r';
    usbprintbuf[size + 2] = '\0';

    CDC_Transmit_FS(usbprintbuf, strlen(usbprintbuf));

    HAL_Delay(200);
}
