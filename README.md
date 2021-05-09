# Snake
This game can be run in a simulator for the DE1-SoC ARMv7 board. It was programmed in the simulator and hasn't been
tested on the actual board due to covid, but it should work fine. 
It uses C and is one file with a header to access hardware addresses for a few peripherals.
If you would like to run it, visit https://cpulator.01xz.net/?sys=arm-de1soc and load in the .axf file,
then you should be good to go and see the output on the Pixel buffer. Controls are with the push buttons.

To note: There is flickering on the screen, that's because I'm redrawing the entire buffer every clock pulse.
This can be fixed using double buffering, but for a prototype school project that wasn't really necessary,
and I haven't looked into the registers that the board uses to modify double buffering, or perhaps it would
have to be implemented from scratch.
That's about it for this one.