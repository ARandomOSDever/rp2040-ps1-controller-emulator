#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "psxSPI.pio.h"

#include "main.h"
#include "buttons.h"

const uint16_t digiConIDLo = 0x41;
const uint16_t digiConIDHi = 0x5a;

typedef enum _ConnectionState {
    DISCONNECTED,
    HANDSHAKEING,
    CONNECTED
} ConnectionState;

#define PIN_DAT 5
#define PIN_CMD 6
#define PIN_SEL 7
#define PIN_CLK 8
#define PIN_ACK 9
#ifdef RP2040ZERO
    #define PIN_DAT 9
    #define PIN_CMD PIN_DAT + 1		// must be immediately after PIN_DAT
    #define PIN_SEL PIN_CMD + 1		// must be immediately after PIN_CMD
    #define PIN_CLK PIN_SEL + 1		// must be immediately after PIN_SEL
    #define PIN_ACK 13
#endif

PIO pio = pio0;

uint smCmdReader;
uint smDatWriter;

uint offsetCmdReader;
uint offsetDatWriter;

ConnectionState connState = DISCONNECTED;
uint16_t controllerButtons;

int main() {
    char serialInput; // Input from PlayStation

    stdio_usb_init();
    printf("Starting PS1/PS2 Controller emulator\n");
    printf("Emulating a PS1 Digital Controller\n");
    printf("Controls:\n" 
           "\tWASD - D-Pad\n"
           "\tRight arrow - Circle\n"
           "\tDown arrow - Cross\n"
           "\tUp arrow - Triangle\n"
           "\tLeft arrow - Square\n"
           "\tEnter - Start\n"
           "\tBackspace - Select\n");
    printf("Note: There isn't any code to prevent pressing opposing D-Pad directions\n");


	offsetCmdReader = pio_add_program(pio, &cmd_reader_program);
	offsetDatWriter = pio_add_program(pio, &dat_writer_program);

	smCmdReader = pio_claim_unused_sm(pio, true);
	smDatWriter = pio_claim_unused_sm(pio, true);

	dat_writer_program_init(pio, smDatWriter, offsetDatWriter);
	cmd_reader_program_init(pio, smCmdReader, offsetCmdReader);

	/* Enable all SM simultaneously */
	uint32_t smMask = (1 << smCmdReader) | (1 << smDatWriter);
	pio_enable_sm_mask_in_sync(pio, smMask);

    while (true) {
        serialInput = read_byte_blocking(pio, smCmdReader);
        if (!stdio_usb_connected()) {
            handleUSBInput(&controllerButtons);
        }
        printf("%x", controllerButtons); // This remains here for debugging
    };
}

void handleUSBInput(uint16_t* buttonBits) {
    int getcharval = getchar_timeout_us(0); // This is put into an variable for the default case to be able to show the unknown key
    switch (getcharval) {
    case 0x1b: // If the input is an escape character (aka arrow keys)
        getchar(); // Ignore the 2nd byte since it's "["
        int getcharval2 = getchar(); // Same reason as above
        switch (getcharval2) {
        case 'A': // Up arrow
            buttonBits = (uint16_t *)((int)buttonBits | BTN_TRIANGLE); // Can't use |= due to buttonBits being an uint16_t
            break;
        case 'B': // Down arrow
            buttonBits = (uint16_t *)((int)buttonBits | BTN_CROSS);
            break;
        case 'C': // Right arrow
            buttonBits = (uint16_t *)((int)buttonBits | BTN_CIRCLE);
            break;
        case 'D': // Left arrow
            buttonBits = (uint16_t *)((int)buttonBits | BTN_SQUARE);
            break;
        }
        break;
    case 0x08: // Backspace
        buttonBits = (uint16_t *)((int)buttonBits | BTN_SELECT);
        break;
    case '\n': // Enter/Return
        buttonBits = (uint16_t *)((int)buttonBits | BTN_START);
        break;
    case 'W': // D-Pad
        buttonBits = (uint16_t *)((int)buttonBits | BTN_DPAD_UP);
        break;
    case 'A':
        buttonBits = (uint16_t *)((int)buttonBits | BTN_DPAD_LEFT);
        break;
    case 'S':
        buttonBits = (uint16_t *)((int)buttonBits | BTN_DPAD_DOWN);
        break;
    case 'D':
        buttonBits = (uint16_t *)((int)buttonBits | BTN_DPAD_RIGHT);
        break;
    case 'w':
        buttonBits = (uint16_t *)((int)buttonBits | BTN_DPAD_UP);
        break;
    case 'a':
        buttonBits = (uint16_t *)((int)buttonBits | BTN_DPAD_LEFT);
        break;
    case 's':
        buttonBits = (uint16_t *)((int)buttonBits | BTN_DPAD_DOWN);
        break;
    case 'd':
        buttonBits = (uint16_t *)((int)buttonBits | BTN_DPAD_RIGHT);
        break;
    case 'Q': // L1 button
        buttonBits = (uint16_t *)((int)buttonBits | BTN_L1);
        break;
    case 'q':
        buttonBits = (uint16_t *)((int)buttonBits | BTN_L1);
        break;
    case 'E': // R1 button
        buttonBits = (uint16_t *)((int)buttonBits | BTN_R1);
        break;
    case 'e':
        buttonBits = (uint16_t *)((int)buttonBits | BTN_R1);
        break;
    case '1': // L2 button
        buttonBits = (uint16_t *)((int)buttonBits | BTN_L1);
        break;
    case '3': // R2 button
        buttonBits = (uint16_t *)((int)buttonBits | BTN_R1);
        break;
    default:
        printf("Unknown key: 0x%x", getcharval);
        buttonBits = 0;
        break;
    }
}

void handleSerialReq(uint8_t data) {
    switch (connState) {
    case DISCONNECTED:
        if (data == 0x01) {
            connState = HANDSHAKEING;
            write_byte_blocking(pio, smDatWriter, digiConIDLo);
            if (read_byte_blocking(pio, smCmdReader) != 0x42) {
                connState = DISCONNECTED;
                return;
            }
            else write_byte_blocking(pio, smDatWriter, digiConIDHi);
            connState = CONNECTED;
        }
    
    case CONNECTED:
        if (data == 0) {
            write_byte_blocking(pio, smDatWriter, controllerButtons);
        }
        break;
    }
}