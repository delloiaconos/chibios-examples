# ChibiOS Examples
A set of examples made for ChibiOS/RT 21.11.x (Vettica) + HAL stable branch .
These examples have been tested on ChibiStudio 2023-02 for Windows. 

ChibiStudio version: [Sourceforge ChibStudio Project](https://sourceforge.net/projects/chibios/files/ChibiStudio%20Windows/ChibiStudio_Windows_2023-02.7z)

ChibiOS version: [ChibiOS 21.11.x](https://github.com/ChibiOS/ChibiOS/tree/stable_21.11.x)

These examples are cured by Salvatore Dello Iacono, for the NeaPolis Innovation Summer Campus's Tutors and Students.


## Examples
You can read and travel the examples as you desire, but the suggested path is the following:
** Only here listed examples have been updated and successfully tested!**

- [x] GPIOs Peripheral
- [ ] UART and SERIAL Peripheral
- [x] ADC (Analog to Digital Converter) Peripheral 
- [ ] DAC (Digital to Analog Converter) Peripheral 
- [ ] TIMER Peripheral
    - [ ] GPT (General Purpose Timer) Drivers
    - [x] ICU (Interrupt Capture Unit) Drivers
    - [x] PWM (Pulse Width Modulation) Driver
- [ ] ChibiOS Thread Examples 
- [ ] ChibiOS Shell Examples

### GPIO
- [x] GPIO01 - Onboard Green LED Toggling.
- [x] GPIO02 - Generic output PIN configuration and use.
- [x] GPIO03 - PAL LINE usage and Thread example.
- [x] GPIO04 - Reading the on-board user button.
- [x] GPIO05 - A simple way to debounce an external button.
- [x] GPIO06 - Multithreading use of GPIO with button and LED.
- [x] GPIO07 - Iterate through an array of output lines.
- [x] GPIO08 - How to use PAL Events and callbacks to measure time.

### ADC
- [x] ADC00 - ADC Base Project.
- [x] ADC01 - Simple single acquisition/channel ADC in a thread.
- [x] ADC02 - Asynchronous conversion call.
- [x] ADC03 - Asynchronous sampling and over serial display.
- [x] ADC04 - TIMER triggered ADC conversion.

### SHELL
- [x] SHELL00 - ChibiOS adapted Demo Project.
- [x] SHELL01 - Static shell thread with custom commands and configuration.
- [x] SHELL02 - Project with local custom shell.
- [ ] SHELL03 - Using the shell as a menu.

### ICU 
- [x] ICU00 - Simple example with ICU

### PWM
- [x] PWM00 - Simple exampl with PWM


## Running examples
1 - You can either download the zipped version of this repository or clone the repository into the ChibiStudio directory.

2 - Import the projects into the ChibiStudio workspace (you can also leave the "Copy project into workspace" option unchekced if downloaded correctly the repository)
