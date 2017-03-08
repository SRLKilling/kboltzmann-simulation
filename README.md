# kboltzmann-simulation
A perfect gas simulation with gravity, to experimently check the law of Boltzmann, and measure its constant.

## Requirements
The makefile has been made to work with gcc or MinGW.
The only required dependency is [SFML](http://www.sfml-dev.org/)

## How to use
You can press `space` to start the simulation.
You can (and you will have to) use `-` and `+` to lower and increase the speed of simulation.
A too fast speed may bring to unhandled collisions, so you should try to keep it low.

Frequently, the number of particule per height is computed.
You can print the measures using `2`.

Using these measurements, we can make an exponential regression, and thus, find the Boltzmann constant.
You can print the measures for K using `4`

You can always come back to the particle printing using `1`.
Using your mouse wheel, you can scroll in and out to better see evolution.
With `r`, you can reset the max counters used to colour temperatures, and density.
To fasten computing, you can choose to toggle displaying using `d`, or just toggle particles with `p`.

Here is a little video of an evolved simulation :

![Demo of the program](https://media.giphy.com/media/sGIZ7fYEFxd3a/giphy.gif)
