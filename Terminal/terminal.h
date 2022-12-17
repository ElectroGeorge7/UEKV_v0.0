/**
 * @author Katukiya G.
 *
 * @file terminal.h
 *
 * @brief UART and USB terminals control.
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

void terminal_init(void);

void uartconsole_printf(const char* fmt, ...);
#define uartprintf(...) uartconsole_printf(__VA_ARGS__)

void usbconsole_printf(const char* fmt, ...);
#define usbprintf(...) usbconsole_printf(__VA_ARGS__)


#endif /* TERMINAL_H_ */
