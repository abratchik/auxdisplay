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
 * File:   AuxDisplay.cpp
 * Author: abratchik
 * 
 * Created on January 9, 2022, 6:14 PM
 */

#include <string>
#include "AuxDisplay.h"


bool AuxDisplay::initialize() {
    
    // TODO: make VID/PID configurable
    handle = hidlcd_open(vendorid, productid, NULL);
    
    if(handle == NULL) 
        return false;
    
    if(dp ==  NULL)
        dp = hidlcd_get_display_params(handle);
    
    return true;
}

int AuxDisplay::show() {
            
    int res = hidlcd_set_cursor(handle, 0, 0);
    
//    std::string cmd = "sensors -u | grep --no-messages -A 3 coretemp-isa | " 
//                      "grep temp1_input | awk '{print $2}' | awk '{printf(\"%d\\n\",$1 + 0.5);}'";
    std::string cmd = "date \"+%d-%m-%y%T\"";
    std::string msg = this->exec_cmd(cmd.c_str());
    
    // std:: string msg = "Hello from daemon";
    
    return hidlcd_print(handle, dp, reinterpret_cast<const unsigned char*>(msg.c_str()));
        
}

int AuxDisplay::load_config() {
    if(!config_read_file(&cfg, CONFIG_FILE)) {
        return(CONFIG_LOAD_FAILURE);
    }
    
    return(CONFIG_LOAD_SUCCESS);
}
