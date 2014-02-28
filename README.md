# Routes Drawer #

Just a simple drawer of a graph of routes stored in a binary file.

## Example ##

![Rostov-on-Don](http://rexim.me/images/rostov-on-don.png)

## Dependencies ##

* cairo 1.12.2+
* GCC 4.7.2+

## Compilation ##

    $ mkdir build/
    $ cd build/
    $ cmake ..
    $ make

## Usage ##

    $ ./build/routes-drawer <binary file> <map.png>
