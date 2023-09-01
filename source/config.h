#pragma once

#include <string>

class TConfigData {
    public :
        struct {
            float lat;
            float lon;
        } location;

        std::string serialPort;
};

class TConfig {
    public :

        TConfig();
        
        bool load(std::string name);

        TConfigData data;

    private :

};

extern TConfig config;