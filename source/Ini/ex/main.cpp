#include <stdio.h>

#include "SimpleIni.h"


void test_read()
{
    CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error rc = ini.LoadFile("example2.ini");
	if (rc < 0) { /* handle error */ };
	//ASSERT_EQ(rc, SI_OK);

	const char* pv;
	float gps_lat = ini.GetDoubleValue("gps", "lat", 0.0);
	float gps_lon = ini.GetDoubleValue("gps", "lon", 0.0);
    printf("%f %f\r\n", gps_lat, gps_lon);

    ini.SetDoubleValue("gps", "lat", 2*gps_lat);

    int clients_count = ini.GetLongValue("clients", "count", 0);
    printf("clients %d\r\n", clients_count);
	//ASSERT_STREQ(pv, "value");

	ini.SetValue("section", "key", "newvalue");

	pv = ini.GetValue("section", "key", "default");
	//ASSERT_STREQ(pv, "newvalue");

    
	rc = ini.SaveFile("example3.ini");
}

void test_write() 
{
    CSimpleIniA ini;


	ini.SetValue("honden", "key", "newvalue");

  	// save the data to a string
	std::string data;
	auto rc = ini.Save(data);
	if (rc < 0) { /* handle error */ };
	//ASSERT_EQ(rc, SI_OK);

	// save the data back to the file
	rc = ini.SaveFile("example2.ini");
	if (rc < 0) { /* handle error */ };
	//ASSERT_EQ(rc, SI_OK);
}

int main(int argc, char **argv) {
    printf("init test\r\n");

    test_read();

    return 0;
}