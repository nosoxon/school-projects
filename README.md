# DHT11 in AVR Assembly

This is a standard embedded systems project in which students were
required to design a simple thermostat controller which allows the user to
select a desired temperature with an RPG and view the current temperature
read off of a DHT11 temperature sensor. Read the full report [here](report/report.pdf).

The most interesting part of this lab for me was learning about
[sigrok](https://sigrok.org/wiki/Main_Page) and its graphical frontend
[PulseView](https://sigrok.org/wiki/PulseView).

Had I done a little searching beforehand, I would have found that `sigrok`
already had [support for the DHT11 protocol](https://www.sigrok.org/blog/new-protocol-decoder-aosong-am230x-dht11),
but I went full steam ahead and wrote [my own](dht11/pd.py) to visualize a scope capture of
the exchange. With this visualization, I was able to devise an extremely simple method for
reading data from the DHT11 (described in the report).

[Source code](main.S)

<p align=center><img src=report/breadboard.jpg width=60%></p>

## PulseView DHT11 Screenshots
#### Full exchange
<p align=center><img src=report/sigrok-pulseview.png></p>

#### Reading technique described in report
<p align=center><img src=report/reading-dht11.png></p>
