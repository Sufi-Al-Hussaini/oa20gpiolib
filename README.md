Introduction:
-------------
This package provides class to control the GPIO on Olinuxino A20.
With some arduino compatible function.
This pacakage is based on work done by Stefan Mavrodiev <support@olimex.com>
for the pyA20 python library https://pypi.python.org/pypi/pyA20

main change between the two library is the way to give gpi pin to use.
all oa20_xxxx use connector , pin value of the olimex a20 board.
ie: oa20_gpio_set_cfgpin(3,7,OUTPUT)  will put pin 7 of the connector GPIO3 in output mode.

Some arduino like function is also added to facilitate usage of arduino library on the olimex a20 board

implemented functions:

- pinMode
- digitalWrite
- digitalRead
- _delay_ms

Warning
-------
 - currently the library need to access /dev/mem only root account can do that.
 - you may bypass or conflicting with kernel driver if you don't take care.

How to build
------------

First of all you need cmake installed on your system.
go into main directory and do 

mkdir build
cd build
cmake ../
make 
make install

The gpio-test bin provided as sample will let the green led of the board blink

TO DO
-----
- Add more arduino function like
- Add the possibility to use gpio file system (more safe but slower ) or /dev/mem 

If you have any question please send me an email at lemouchon at gmail dot com

