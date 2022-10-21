# PNG-Color-Modifier


Changes the desired color of the png using C  


**This is a personnal** project meant to improve in C and to understand how png worked (using only wikipedia and other png-related ressources)  


There are a lot of thing to optimise in this code but it is not the point


***

## Compilation & Execution

`gcc -o main duplicate.c crc32.c main.c`

`./main <path>`


I might add a Makefile later


## How it works (basically)

The program take a PNG picture has an input (must be of color type 3, [Palette](https://en.wikipedia.org/wiki/Portable_Network_Graphics#Critical_chunks))  

Asks user for which color to change and then modifies the PLTE chunck of the PNG (basically replacing the bytes of the pre exising color with the new one)  

Since it is a Palette, all pixel associated with this color will be replaced (in the example below you can see that even the mouth changed color)

 ***


## Example

Input Image / Modified


<img src="./images/touhou.png">
<img src="./images/touhouModified.png">




