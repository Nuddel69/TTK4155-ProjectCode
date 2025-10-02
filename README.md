# TTK4155 Embedded computer Project

Repo containing code and infrastructure for building the semester project in the NTNU subject TTK4155 Embedded and Industrial Computer Systems Design

## Useful notes to self

### Commands

- Picocom (use tab-completion for the serial device):
  `picocom /dev/<SERIAL_DEVICE> --baud 9600 --parity n --databits 8 --stopbits 2`

  The shortened version
  `picocom /dev/<SERIAL_DEVICE> -b 9600 -y n -d 8 -p 2`

## Issues encountered and their solution

### USART flipping bits in the RS232 line driver

We were sending the letter 'k' but recieving 'J'. Using a scope on the TX
output of the uC showed the correct databits, but after the MAX233 we were
reading something else.

**The solution**

We were setting the wrong baud rate and the synchronisation issues revealed
themselves later down the chain of components. Turns out, you should set both
UBRRH and UBRRL regardless of the size of the UBRR number.

### Half the addresses on the RAM getting overwritten

While all our addresses were passing the write test, half were failing on read.
We figured it might be caused by a single bit not flipping and thus the addresses
using the faulty bit were being overwritten. Tried using known good code, redid
the wiring a dozen times, stuck decoupling caps _everywhere_ and swapped out both
the latch and uC, all to no avail.

We probed again and noticed a single bit not latching. While all other outputs on
the latch were switching between 0V and 3.5V (nominal according to the datasheet),
the faulty one switched between 3.5v and 5v, leading to reading a constant logic
high. Strange considering this was our sixth latch so the odds of it being faulty
were astronomically small.

**The solution**

One of the breadboard holes between the uC and had a small particle stuck inside.
While the contacts for both the uC and latch were good, this particle likely
shorted two paths causing noise and undefined behaviour. Angrily stabbing the 
offending hole with a dupont wire fixed the issue.

### SPI only sending a single (if lucky) packet

Our early attempts at getting SPI up and running were met with a single packet read
on the oscilloscope, before code execution got stuck in the SPI transmission done loop.

**The solution**

As the uC supports slave mode as well, it features a SS pin. If triggered, the uC
will automatically enter slave mode, regardless of register config. In order to avoid
accidentally pulling the wrong logic on this, it can either be pulled HIGH or explicitly
defined as output. The latter allows us to keep using the pin as a output, hence
proving the better solution.

### Debug not timing correctly when using different crystal

While this lab is based around a 4.915200MHz crystal, there was an attempt at using
a 16MHz crystal as well. After adjusting the fuses to support an 8+MHz external clock
the code was acting strange. A delay was only lasting an eighth of the time passed to
the function. Setting the clock-divider fuse solved it, but caused issues for the UART
baudrate.

**The solution**

The `delay` library has a software defined macro `F_CPU` defining the clock speed. We´ve
been lucky this is compatible with our usual clock, but now it had to be defined from the
compiler. Adding `-DF_CPU=<clock>` to the compilation command solved it (see commit `#7f15e61`).

## Debugging flowchart - expanded as we encounter more problems

```mermaid
flowchart TD
    A(["Start: Bygger'n not bygger'ning"]) --> B["Visually Inspect Wiring"]
    B --> C{"Looks Good?"}
    C -->|No| D["Redo the Wiring"]
    C -->|Maybe?| D
    C -->|Think So?| D
    C -->|Probably?| D
    D --> P
    
    C -->|Yes| E["Probe Data In to MCU"]
    E --> F{"Data In as Expected?"}
    F -->|No| D
    F -->|Yes| G["Probe Data Out of MCU"]
    
    G --> H{"Data Out as Expected?"}
    H -->|No| I["Check Your Code"]
    H -->|Yes| CLK["Check Clock (Baud Rate / CLK Pin)"]
    
    CLK --> J["Check Your Wiring Again"]
    
    I --> K{"Code Looks Correct?"}
    K -->|No| L["Debug & Fix Code"]
    K -->|Yes| M["Try Known-Working Code from Internet"]
    
    J --> N["Check Breadboard Connections"]
    M --> N
    N --> O["Probe Breadboard Holes (Bad Contacts / Particles?)"]
    O --> P{"System Working?"}
    P e1@-->|No|R["Check the Datasheet Again"] e2@--> B
     e1@{ animate: true }
     e2@{ animate: true }
    P -->|Yes| Q(["Success!"])
```
