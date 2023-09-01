#include "config.h"

#include "Ini/SimpleIni.h"

TConfig config;


//
//
//
TConfig::TConfig()
{

}


//
//
//
bool TConfig::load(std::string name)
{
    CSimpleIniA ini;


    if(ini.LoadFile(name.c_str()) != SI_OK) {
        return false;
    }

    //
    data.location.lat = ini.GetDoubleValue("location", "lat", 0.0);
    data.location.lon = ini.GetDoubleValue("location", "lon", 0.0);

    //
    data.serialPort = ini.GetValue("Wan", "port", "");
    if(data.serialPort == "") {
        return false;
    }

    return true;
}

