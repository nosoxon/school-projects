# AVR Stopwatch -  Report

My first assembly program. You can find the original report in PDF format [here](report.pdf).

#### Table of Contents
- [Introduction](#1-introduction)
- [Schematic](#2-schematic)
- [Discussion](#3-discussion)
  - [Hardware Design](#31-hardware-design)
  - [Buttons and Debouncing](#32-buttons-and-debouncing)
  - [Four-digit Seven-segment Display](#33-four-digit-seven-segment-display)
- [Datasheets](#4-datasheets)
- [Source Code](main.S)

## 1. Introduction
The goal of this lab was to construct a simple stopwatch using shift registers,
7-segment displays, and two buttons.

The stopwatch must implement two modes with different timer resolutions.
In Mode I, the stopwatch should begin with "0.0" displayed. When the first
button is pressed, the display should increment every 0.1 seconds, updating the
display to "0.1", "0.2", and so on until the display reaches "9.9".
Pushing the first button while the stopwatch is counting up should stop the
counter and freeze the display. Pressing the first button while paused should
continue the timer. Once 9.9 seconds have elapsed in Mode I, the display should
flash "9.9" once every two seconds. Pressing the second button for less than a
second in any state should stop the counter and reset the stopwatch to 0.

In Mode II the stopwatch should function identically to Mode I. However,
the timer should increment in steps of 1 second instead of 0.1 seconds.
Similarly, the stopwatch should now show "00" at the start and "99" for
overflows.

Pressing the second button for more than 1 second should reset the
stopwatch to 0 and alternate between modes I and II.

<p align=center>
 <img src=images/state-diagram.png><br>
 <b>Figure 0:</b> logic state diagram
</p>


## 2. Schematic
<p align=center>
 <img src=images/schematic.png><br>
 <b>Figure 1:</b> schematic as implemented
</p>
<br>
<p align=center>
 <img src=images/implementation.jpg><br>
 <b>Figure 2:</b> physical implementation
</p>

## 3. Discussion
I used $1\ \text{k}\Omega$ resistors for the seven-segment display
in order to keep the current through each segment well beneath the target value
of $6\ \text{mA}$. I also included a standard $0.1\ \upmu\text{F}$ decoupling
capacitor to smooth ripples in the power supply.

### 3.1 Hardware Design
The final design was almost identical to that prescribed in the lab manual, with
the exception of the four-digit display. Fortunately, the pinout of the 3461AS
was nearly identical to that of the 5161AS, and only required one additional
wire for each digit.

### 3.2 Buttons and Debouncing
Each button has an associated structure in SRAM containing relevant information.
I aimed to keep the debouncing logic as simple as possible: register a change
in button state if and only if it maintains that changed state over a minimum
period of time. An unregistered change occurs when the button state in memory
differs from the button's hardware state. The final implementation requires
50 stable samples at $1\ \text{ms}$ intervals to register a change.

Also included in the button structure is a duration field, incremented
every $100\ \text{ms}$ when a button is in the pressed state. This value is used
when button B is released to determine whether or not to change the mode.

### 3.3 Four-digit Seven-segment Display
In pursuit of extra credit, I created the design with a four-digit
seven-sigment display. The 3461AS can only display one unique digit at a time,
with power controlled by one pin for each digit. One study[^1]
found that humans can detect flicker at an extreme of $500\ \text{Hz}$. With a
system clock of $16\ \text{MHz}$, a refresh rate as large as $1\ \text{kHz}$ still
affords an entire 16,000 cycles to spend per refresh. For each refresh, the
stopwatch must display each digit for a distinct but equal period of time.

The shift register supports clock speeds up to $20\ \text{MHz}$; the final
implementation requires approximately $8\ \upmu\text{s}$ (lines 574-634) to
shift one byte into the shift register. No display digit is powered while the shift
register is loaded. In order to maximize the duty cycle of each digit, it is
ideal for each digit to be powered substantially longer than it is not during
its 25% of each refresh. Lines 635-648 below produce a delay of approximately
$48\ \upmu\text{s}$, resulting in a final duty cycle of
$$ 25\% \cdot \frac{48}{8+48}\approx 21.4\% $$
for each digit. Unsurprisingly, reducing the duty cycle dims the digits.
Each refresh in the final implementation takes approximately
$230\ \upmu\text{s}$ (lines 576-611), which corresponds to a refresh rate of
$4.35\ \text{kHz}$. Indeed, no flicker is detectable with the naked eye.

To control power to the display digits, one I/O pin is assigned to each of
the four digits. While this method is simple and convenient, it would also be
feasible to use another shift register in lieu of I/O pins to control power to
each digit. While this would increase I/O pin availability, it would increase
hardware cost and increase code complexity marginally.

[^1]: https://www.nature.com/articles/srep07861

See source comments in Appendix A for more local and detailed discussion.

## 4. Datasheets
- [Microchip ATmega328P](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf) &ndash; 8-bit RISC microcontroller (32KB flash, 2KB SRAM)
- [TI SN74HC595](https://www.ti.com/lit/gpn/sn74hc595) &ndash; 8-bit shift register (~25MHz @ 5V)
- [XLITX 3461AS](http://www.xlitx.com/datasheet/3461AS.pdf) &ndash; 4x7-segment common-cathode display
- [Arduino UNO R3 (schematic)](https://www.arduino.cc/en/uploads/Main/Arduino_Uno_Rev3-schematic.pdf) &ndash; ATmega328P development board and programmer

## [Appendix A: Source Code Listing](main.S)
