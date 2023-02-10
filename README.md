# Soldering_Oven
Controls the temperature of a modified toaster oven to be used for solder rework.

An Arduino Nano is used to control the PWM input of solid state relay.
Can be configured to use an OLED display and/or RGB lights to display temperature.

PID needs to be merged with temperature control from Phys_Sim branch so temperature doesnt overshoot by 10*C when heating up.
