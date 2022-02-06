/*
 * Copyright (C) 2022 abratchik
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   AuxDisplay.h
 * Author: abratchik
 *
 * Created on January 9, 2022, 6:14 PM
 */

#ifndef AUXDISPLAY_H
#define AUXDISPLAY_H

#include <dlfcn.h>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <hidapi/hidapi.h>
#include <hidlcd/hidlcd.h>
#include <libconfig.h>
#include <unistd.h>

#define CONFIG_FILE "/etc/auxdisplay.conf"
#define CONFIG_LOAD_SUCCESS     0
#define CONFIG_LOAD_FAILURE     1

#define DEFAULT_VID     0x2341
#define DEFAULT_PID     0x8036
#define DEFAULT_REFRESH 5000
#define DEFAULT_DELAY   100     //delay in miliseconds

class AuxDisplay {
public:
    AuxDisplay() {
        
        config_init(&cfg);
        
        hidlcd_init();
        
        hidlcd_send_command(handle, HID_AUXD_CLEAR);
        
        usleep(cmddelay * 1000);
    };
    
    AuxDisplay(const AuxDisplay& orig);
    
    virtual ~AuxDisplay() {
        if(handle!=NULL) 
            hidlcd_close(handle);
        delete dp;
        hidlcd_exit();
        config_destroy(&cfg);

    };
    
    bool initialize();
    
    int load_config();
    
    int show();
    
    unsigned int get_refresh() {return refresh;};
    
private:

    unsigned short vendor_id;
    unsigned short product_id;
    
    hid_device *handle = NULL;
    HIDDisplayParams* dp = NULL;
    
    config_t cfg;
    
    config_setting_t *display;
    
    int vendorid = DEFAULT_PID;
    int productid = DEFAULT_VID;
    
    int refresh = DEFAULT_REFRESH; // Daemon-specific intialization should go here
    int cmddelay = DEFAULT_DELAY;
    
    std::string exec_cmd(const std::string& cmd_string) {
        const char *cmd = cmd_string.c_str();
        FILE* pipe = popen(cmd, "r");
        if (!pipe) return "ERROR";
        char buffer[128];
        std::string result = "";
        std::string npchars = "\n\r\t";
        while(!feof(pipe)) {
            if(fgets(buffer, 128, pipe) != NULL) {
                result += buffer;
            }
        }
        pclose(pipe);
        
        for (char c: npchars) {
            result.erase(std::remove(result.begin(), result.end(), c), result.end());
        }
        return result ;
    }
};

#endif /* AUXDISPLAY_H */

