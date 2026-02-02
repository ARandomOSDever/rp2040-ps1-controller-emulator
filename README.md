# PS1 controller emulator
This is an emulator for the PS1 digital controller using the RP2040 in USB CDC ACM mode, based on the PicoMemcard POC controller simulator example
You can also use a PicoMemcard board (as the PS1 memory card and controller share the same bus), however if you use a RP2040-Zero you will need to edit CMakeLists.txt. See [Notes](#Notes) for more info. Make sure you desolder the 3.3V line while connecting USB to your PC
connected to serial)
## Usage
- Compile the program and flash it to your Pico (there isn't a prebuilt UF2 yet)
- Connect the Pico to your PC with a USB data cable
- Use a terminal emulator (such as ```screen```) to connect to the serial port
  - For example: ```screen /dev/ttyACM0```
- Use your keyboard to press buttons on the emulated controller
  - Keybinds are below
  - If your terminal emulator cannot send escape sequences, use the characters from the parenthesis
## Keybinds
- WASD -- D-Pad
- Q -- L1
- E --  R1
- 1 -- L2
- 3 -- R2
- Enter/Return (0x0A) -- Start
- Backspace (0x08) -- Select
- Arrow keys ("\x1b[y", where y is one of the characters in the parenthesis)
  - Right ("C") -- Circle
  - Down ("B") -- Cross
  - Up ("A") -- Triangle
  - Left ("D") -- Square 
## Notes
- This uses the PicoMemcard pinout for the Pi Pico (DAT = 5, CMD = 6, SEL = 7, CLK = 8 and ACK = 9), if you have a RP2040-Zero you will have to uncomment line 49 of CMakeLists.txt
- This hasn't been tested on real hardware due to me accidentally shorting the 5V line to GND when checking the voltage of a 7805 from a Famiclone which resulted into the PS601 and PS606 fuses to open
