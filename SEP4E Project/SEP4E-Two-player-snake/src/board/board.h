/*! @file board.h
@brief Board drivers for the Game Console Board.

@author iha

@defgroup  board_drivers Drivers for the Game Console Main Board.
@{
@brief A driver selection for the Game Console board.

@defgroup board_init Board driver initialization.
@brief How to initialize the Board drivers.

@defgroup board_public_function Public Board Functions
@brief Public Board functions.

Here you you will find the functions you will need.

@defgroup board_public Public Declarations, variables etc.
@brief Declarations, variables etc. that are private for the Board Driver implementation.

@defgroup board_return Board Driver Return codes
@brief Codes returned from Board Driver functions.
@}
*/
#ifndef BOARD_H_
#define BOARD_H_

typedef void (*display_callback_ptr_t)(void);

//-------------------------------------------------
/**
@ingroup board_init
@brief Initialize the Game Console board drivers.

@note Must be called before any other board functions are used.
*/
void init_board(void);

//-------------------------------------------------
/**
@ingroup board_public_function
@brief Initialize and start a timer (~0.764 ms) that calls a call back function.

The call back function must have the following signature
@code
void f(void);
@endcode

@param[in] cb function pointer to the call back function to be called.
*/
void init_display_timer(display_callback_ptr_t cb);

//-------------------------------------------------
/**
@ingroup board_init
@brief Initialize the serial communication port.

Every time a byte is received from the com port, it will be put into the specified queue.

@param[in] x_rx_queue handle to a FreeRTOS queue.
*/
void init_com(QueueHandle_t x_rx_queue);

//-------------------------------------------------
/**
@ingroup board_public_function
@brief Send a byte array to COM port.

@note The COM module must be initialized before sending @ref init_com

@param[in] bytes pointer to byte array.
@param[in] len number of bytes to send.
*/
uint8_t com_send_bytes(uint8_t *bytes, uint8_t len);



uint8_t com_send_byte(uint8_t byte);
#endif /* BOARD_H_ */