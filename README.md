# NeaPolis Innvoation Summer Campus Examples
Examples made for ChibiOS/RT + HAL during the NeaPolis Innovation Summer Campus 2022.

These examples have been tested on ChibiStudio 2022-01 for Windows. 
You can download this version of the development environment at
[OSDN ChibStudio Project](https://osdn.net/projects/chibios/downloads/70767/ChibiStudio_Windows_2022-01.7z/)

ChibiOS version:[ChibiOS 21.11.x](https://github.com/ChibiOS/ChibiOS/tree/stable_21.11.x)

These examples were made by Student Tutors and NeaPolis Innovation Summer Campus collaborators.

## Running examples
1 - You can either download the zipped version of this repository or clone the repository into the ChibiStudio directory.

2 - Import the projects into the ChibiStudio workspace (you can also leave the "Copy project into workspace" option unchekced if downloaded correctly the repository)


## Examples
You can read and travel the examples as you desire, but the suggested path is the following:
** Only here listed examples have been updated and tested!**

- [x] GPIOs Examples 
- [x] ChibiOS Thread Examples 
- [ ] UART and SERIAL
- [x] Analog to Digital Converter Examples (ADC)
- [ ] ChibiOS Shell Examples

### GPIO
- GPIO00 - Onboard Green LED Toggling.
- GPIO01 - Generic output PIN configuration and use.
- GPIO02 - PAL Line usage.
- GPIO03 - Reading the on-board user button.
- GPIO04 - A simple way to debounce an external button.
- GPIO05 - Multithreading use of GPIO with button and LED.
- GPIO06 - Iterate through an array of output lines.
- GPIO07 - How to use PAL Events and callbacks to measure time.

### ADC
- ADC00 - ADC Base Project.
- ADC01 - Simple project with single acquisition from a single channel.
- ADC02 - Asynchronous conversion call.
- ADC03 - Asynchronous sampling and over serial display.
- ADC04 - TIMER triggered ADC conversion.

### SHELL
- SHELL00 - ChibiOS adapted Demo Project.
- SHELL01 - Static shell thread with custom commands and configuration.
- SHELL02 - Project with local custom shell.
- SHELL03 - Using the shell as a menu.

### ICU 
- ICU00 - Simple example with ICU

### PWM
- PWM00 - Simple exampl with PWM