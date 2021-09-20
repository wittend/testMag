# testMag

This software is intended to run on a Raspberry Pi 3/4 using Raspberry Pi OS.  It has not been tested in any other context.

This is a program intended to assist in testing the Personal Space Weather Station (PSWS) magnetometer support boards with or without the PNI RM3100 geomagnetic sensor module.  It is written in simple C. It is intended for use on boards such as the Raspberry Pi 3/4. It uses Linux OS resources to open, read, write, and close the device through the appropriate I2C bus.  It does not support SPI.  It assumes that the I2C kernel drivers are installed, and that device names such as /dev/i2c-1 can be listed using ls.  (The exact numbers vary depending on the device used).

The current pre-release code is 0.0.1 (alpha1)

Just clone this project into your home directory on the Raspberry Pi or board with similar bus using:

    git clone https://github.com/wittend/testMag.git

Then do:

    $ cd testMag
    $ make
    $ cd eepromutils
    $ make
    $ cd ..


and if all goes well type:

    $ sudo ./testMag
    (most flavors of Linux will require sudo)


and now you should see some results!

## Example on Raspberry Pi 3/4:

    dave@raspi-3: ~/projects/testMag $ ./testMag


## Example output using -h or -? option:

    $ /rm3100-testMag$ ./testMag -h

    ./testMag Version = 0.0.01

    Parameters:

       -B <reg mask>          :  Do built in self test (BIST).         [ Not implemented ]
       -b <bus as integer>    :  I2C bus number as integer.
       -M <addr as integer>   :  Magnetometer address.                 [ default 20 hex ]
       -P                     :  Show Parameters.
       -V                     :  Display software version and exit.
       -h or -?               :  Display this help.


