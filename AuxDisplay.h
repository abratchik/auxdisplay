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
#include <libconfig.h>

#define CONFIG_FILE "/etc/auxdisplay.conf"
#define CONFIG_LOAD_SUCCESS     0
#define CONFIG_LOAD_FAILURE     1

#define DEFAULT_VID     0x2341
#define DEFAULT_PID     0x8036
#define DEFAULT_REFRESH 5

typedef struct
{
  u_int8_t rows;     // 0x35
  u_int8_t cols;     // 0x36
  u_int8_t chrw;     // 0x3d char width 
  u_int8_t chrh;     // 0x3e char height
  u_int8_t flags;    // 0x21|0x22|0x29|5 unused
} HIDDisplayParams;

extern "C" {

    typedef hid_device *  (*hidlcd_open_t)(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number);
    typedef int (*hidlcd_init_t)(void);
    typedef void (*hidlcd_close_t)(hid_device *dev);
    typedef int (*hidlcd_exit_t)(void);
    typedef int (*hidlcd_set_cursor_t)(hid_device *dev, u_int8_t row, u_int8_t col);
    typedef HIDDisplayParams* (*hidlcd_get_display_params_t)(hid_device *dev);
    typedef int (*hidlcd_print_t)(hid_device *dev, HIDDisplayParams *display_params, const unsigned char *string);
    typedef int (*hidlcd_send_command)(hid_device *dev, u_int8_t command);
}

class AuxDisplay {
public:
    AuxDisplay() {
        lib = dlopen("libhidlcd.so", RTLD_LAZY);
        hidlcd_open = (hidlcd_open_t)dlsym(lib, "hidlcd_open");
        hidlcd_init = (hidlcd_init_t)dlsym(lib, "hidlcd_init");
        hidlcd_close = (hidlcd_close_t)dlsym(lib, "hidlcd_close");
        hidlcd_exit = (hidlcd_exit_t)dlsym(lib, "hidlcd_exit");
        hidlcd_get_display_params = (hidlcd_get_display_params_t)dlsym(lib, "hidlcd_get_display_params");
        hidlcd_set_cursor = (hidlcd_set_cursor_t)dlsym(lib, "hidlcd_set_cursor");
        hidlcd_print = (hidlcd_print_t)dlsym(lib, "hidlcd_print");
        
        config_init(&cfg);
        
        hidlcd_init();
    };
    
    AuxDisplay(const AuxDisplay& orig);
    
    virtual ~AuxDisplay() {
        if(handle!=NULL) 
            hidlcd_close(handle);
        delete dp;
        hidlcd_exit();
        config_destroy(&cfg);
        dlclose(lib);
    };
    
    bool initialize();
    
    int load_config();
    
    int show();
    
    unsigned int get_refresh() {return refresh;};
    
private:
    void* lib;
    unsigned short vendor_id;
    unsigned short product_id;
    
    hidlcd_open_t hidlcd_open;
    hidlcd_init_t hidlcd_init;
    hidlcd_close_t hidlcd_close;
    hidlcd_exit_t hidlcd_exit;
    hidlcd_get_display_params_t hidlcd_get_display_params;
    hidlcd_set_cursor_t hidlcd_set_cursor ;
    hidlcd_print_t hidlcd_print;
    
    hid_device *handle = NULL;
    HIDDisplayParams* dp = NULL;
    
    config_t cfg;
    
    config_setting_t *display;
    
    int vendorid = DEFAULT_PID;
    int productid = DEFAULT_VID;
    
    int refresh = DEFAULT_REFRESH; // Daemon-specific intialization should go here
    
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

