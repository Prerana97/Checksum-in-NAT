# Checksum in NAT

## Course Code: CO300

## Assignment #5

### Overview:
Static and Dynamic NAT have been implemented in ns-3.27. This repository contains the formatted NAT code in ns-3.27, improvements to the test suite and example codes for Static and Dynamic NAT.

### NAT Example:

An example program for Static NAT has been provided in

scratch/ipv4-static-nat-example.cc

and should be executed as

./waf --run ipv4-static-nat-example

An example for Dynamic NAT has been provided in

scratch/ipv4-dynamic-nat-example.cc

and should be executed as

./waf --run ipv4-dynamic-nat-example

The test suite can be executed by

./test.py --suite=ipv4-nat

### References:
[1] Google Summer of Code (GSoC) ns-3 project on NAT (Link: https://www.nsnam.org/wiki/GSOC2012NetworkAddressTranslation)

[2] http://www.isi.edu/nsnam/ns/

[3] http://www.nsnam.org/
