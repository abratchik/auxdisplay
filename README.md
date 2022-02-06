# Auxiliary LCD Display Service

The goal of this project is to create a configurable cross-platform system service, which can print 
the textual information on the auxiliary HID-compliant LCD display connected through USB port. 

## Supported Operating Systems
* Linux
* Mac OSX (TBD)
* Windows (TBD)

## Tested on Operating Systems
* Ubuntu 18.04 LTS

## Architecture of the solution
The overall architecture of the solution is represented on the image below:

![Alt text](https://github.com/abratchik/HIDAuxiliaryDisplay/blob/master/img/HIDAuxiliaryDisplayArchitecture.png)

The solution is consisted of 2 parts - the HID-compliant LCD display and the host. The host part 
contains the auxdisplay service (this project), the HIDLCD driver and the HIDAPI library.


## Installation
Currently, only Linux hosts are supported so the installation procedure below describes the steps to be performed 
on Linux.

Before executing the steps below, please ensure the developer tools are installed on your host. If your operating 
system is Ubuntu, you can run `sudo apt update` & `sudo apt install build-essential` in the bash terminal. 

1. Install [HIDAPI library](https://github.com/libusb/hidapi). Just follow the instructions in HIDAPI Readme.
2. Install [HIDLCD library](https://github.com/abratchik/hidlcd). Follow the instructions in HIDLCD Readme.
3. Clone this repository to a folder on your host machine.
4. Navigate to the folder and execute `make` and `sudo make install`

In case there was no errors during the make, the auxdisplay service can be enabled in your system using following commands:

```
sudo systemctl enable auxdisplay.service
sudo systemctl start auxdisplay.service
```

If everything is installed and configured properly, you will see the date/time and CPU temp on the screen. You can
modify the information displayed on the screen by editing the auxdisplay.conf and re-starting the auxdisplay service.


## Usage
Configuration of the auxdisplay service is simple and requires editing of the cofiguration file. Location of 
the file varies depending on the host operation system:

* Linux: /etc/auxdisplay.conf
* Mac OSX: TBD
* Windows: TBD

Example of the configuration file is below:

```
# Auxiliary Display Configuration file

# VID and PID of the Aux Display
vendorid = 0x2341;
productid = 0x8036;

# Refresh rate in milliseconds
refresh = 1000;

# delay between execution of display output sections, in milliseconds
cmddelay = 10;

# Display output
# The purpose of this section is to configure, which information is going to be printed on the display
# Each  element of the 'display' collection ('output' section) will be executed sequentially. Execution will 
# be triggered with the interval specified by the 'refresh' parameter. The delay between output section 
# execution is specified by the 'cmddelay' parameter and is designated for aligning the slow speed of USB LCD
# and the host computer. 
# 
# Parameters of the display output section explained below:   
# 
# output - can be a plain text or a shell command, depending on the 'type' parameter. This parameter is 
#          estimated as a C/C++ string so all special characters in it have to be escaped.
# type   - an integer parameter specifying how the 'output' will be interpreted. Possible options:
#   0    - the 'output' line will be printed on the display as-is
#   1    - the 'output' line is intrerpreted as a system shell command. The result of the output will
#          be printed on the display.   
# posx   - X-coordinate of the LCD display position where the cursor shoudl be moved before the output.
# posy   - Y-coordinate of the LCD display position where the cursor shoudl be moved before the output.
#          If the value of X or Y coordinate is negative, the cursor will not be moved. This means that 
#          the output will be printed after the last output.  
         
display = 
(
    { output = "date \"+%d%m%y %T\"";
      type = 1;
      posx = 0;
      posy = 0; },
#    { output = "date \"+%T\"";
#      type = 1;
#      posx = 0;
#      posy = 0; },
    { output = "        ";
      type = 0;
      posx = -1;
      posy = -1; },
    { output = "CPU Temp. ";
      type = 0;
      posx = 0;
      posy = 1; },
    { output = "sensors -u | grep --no-messages -A 3 coretemp-isa | grep temp1_input | awk '{print $2}' | awk '{printf(\"%d\\n\",$1 + 0.5);}'";
      type = 1;
      posx = -1;
      posy = -1; }
); 
```

There is no know limitation on shell commands, which can be used with this service but it is recommended to use
single-line output commands because cr/lf symbols are not interpreted properly.  

## License

[GPL v.3](http://www.gnu.org/copyleft/gpl.html)











