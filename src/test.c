/*******************************************************
 Windows HID simplification

 Alan Ott
 Signal 11 Software

 8/22/2009

 Copyright 2009

 This contents of this file may be used by anyone
 for any reason without any conditions and may be
 used as a starting point for your own applications
 which use HIDAPI.
********************************************************/

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#include <hidapi.h>

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	int res;
	unsigned char buf[256];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	struct hid_device_info *devs, *cur_dev;

	printf("hidapi test/example tool. Compiled with hidapi version %s, runtime version %s.\n", HID_API_VERSION_STR, hid_version_str());
	if (hid_version()->major == HID_API_VERSION_MAJOR && hid_version()->minor == HID_API_VERSION_MINOR && hid_version()->patch == HID_API_VERSION_PATCH) {
		printf("Compile-time version matches runtime version of hidapi.\n\n");
	}
	else {
		printf("Compile-time version is different than runtime version of hidapi.\n]n");
	}

	if (hid_init())
		return -1;

	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {
        if( cur_dev->vendor_id == 0x10ce )
        {
		printf("Device Found\n  vendor:%04hx product:%04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
		printf("\n");
        }
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	// Set up the command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;





    /*
  type: 10ce eb93
  path: /dev/hidraw5
  serial_number: 
  Manufacturer: KTURT.LTD
  Product:      (null)
  Release:      0
  Interface:    0
  Usage (page): 0x1 (0xff00)
  */
	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	//handle = hid_open(0x4d8, 0x3f, NULL);
	handle = hid_open(0x10ce, 0xeb93, NULL);
	if (!handle) {
		printf("unable to open device by ID\n");
	}

#if 0
    handle = hid_open_path("/dev/hidraw5");
	if (!handle) {
		printf("unable to open device by path\n");
	}
 		return 1;
#endif

	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read manufacturer string\n");
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	printf("\n");

#if 0
	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read indexed string 1\n");
	printf("Indexed String 1: %ls\n", wstr);
#endif

	// Read the Serial Number String
    char buf2[1024];
    buf2[0] = 1;
    size_t len;
	res = hid_get_feature_report(handle,buf2,len);
	if (res < 0)
		printf("Unable to read feature report\n");
	printf("\n");
	// Set the hid_read() function to be non-blocking.
	//hid_set_nonblocking(handle, 1);

	// Try to read from the device. There should be no
	// data here, but execution should not block.
	res = hid_read(handle, buf, 17);

#if 0
    for( int i = 0; i <= 0xFF; i++)
    {
        // Send a Feature Report to the device
        buf[0] = 0x00;
        buf[1] = 0x00;
        buf[2] = 0x00;
        buf[3] = 0x00;
        buf[4] = i;
        res = hid_send_feature_report(handle, buf, 5);
        printf("FR: 0x%02x\n",i);
        if (res < 0) {
            printf("Unable to send a feature report.\n");
        }
    }
#endif

	memset(buf,0,sizeof(buf));

	// Read a Feature Report from the device
	buf[0] = 0x2;
	res = hid_get_feature_report(handle, buf, sizeof(buf));
	if (res < 0) {
		printf("Unable to get a feature report.\n");
		printf("%ls", hid_error(handle));
	}
	else {
		// Print out the returned buffer.
		printf("Feature Report\n   ");
		for (i = 0; i < res; i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	}

	memset(buf,0,sizeof(buf));

	// Toggle LED (cmd 0x80). The first byte is the report number (0x1).
	buf[0] = 0x1;
	buf[1] = 0x80;
	res = hid_write(handle, buf, 17);
	if (res < 0) {
		printf("Unable to write()\n");
		printf("Error: %ls\n", hid_error(handle));
	}


	// Request state (cmd 0x81). The first byte is the report number (0x1).
	buf[0] = 0x1;
	buf[1] = 0x81;
	hid_write(handle, buf, 17);
	if (res < 0)
		printf("Unable to write() (2)\n");

	// Read requested state. hid_read() has been set to be
	// non-blocking by the call to hid_set_nonblocking() above.
	// This loop demonstrates the non-blocking nature of hid_read().
	res = 0;
	while (1) {
		res = hid_read(handle, buf, sizeof(buf));
#if 0
		if (res == 0)
			printf("waiting...\n");
		if (res < 0)
			printf("Unable to read()\n");
		#ifdef _WIN32
		Sleep(500);
		#else
		usleep(500*1000);
		#endif
#endif

        printState(buf);
    switch(buf[0])

	printf("Data read:\n   ");
	// Print out the returned buffer.
	for (i = 0; i < res; i++)
		printf("%02hhx ", buf[i]);
	printf("\n");
	}

	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

#ifdef _WIN32
	system("pause");
#endif

	return 0;
}

#define RESET 0x01
#define STOP 0x02
#define START_PAUSE 0x03
#define FEED_UP 0x04
#define FEED_DOWN 0x05
#define SPINDLE_UP 0x06
#define SPINDLE_DOWN 0x07
#define M_HOME 0x08
#define SAFE_Z 0x09
#define W_HOME 0x0a
#define S_ON_OFF 0x0b
#define FN 0x0c
#define PROBE_Z 0x0d
#define CONTINUOUS 0x0e
#define STEP 0x0f

#define MACRO_1 0x04
#define MACRO_2 0x05
#define MACRO_3 0x06
#define MACRO_4 0x07
#define MACRO_5 0x08
#define MACRO_6 0x09
#define MACRO_7 0x0a
#define MACRO_8 0x0b
#define MACRO_9 0x0d
#define MACRO_10 0x10

#define AXIS_OFF 0x06
#define AXIS_X 0x11
#define AXIS_Y 0x12
#define AXIS_Z 0x13
#define AXIS_A 0x14

#define STEP_0_001 0x0d
#define STEP_0_01 0x0e
#define STEP_0_1 0x0f
#define STEP_1 0x10
#define STEP_60 0x1a
#define STEP_100 0x1b
#define STEP_LOAD 0x9b

const char *axisName(char axis)
{
    switch(axis)
    {
        case AXIS_OFF: return "AXIS_OFF";
        case AXIS_X: return "AXIS_X";
        case AXIS_Y: return "AXIS_Y";
        case AXIS_Z: return "AXIS_Z";
        case AXIS_A: return "AXIS_A";
    }
    return "NO AXIS";
}

const char *stepName(unsigned char step)
{
    switch(step)
    {
        case STEP_0_001: return "STEP_0_001";
        case STEP_0_01: return "STEP_0_01";
        case STEP_0_1: return "STEP_0_1";
        case STEP_1: return "STEP_1";
        case STEP_60: return "STEP_60";
        case STEP_100: return "STEP_100";
        case STEP_LOAD: return "STEP_LOAD";
    }
    return "NO STEP";
}

const char *keyName(char key, char mod)
{
    if( mod == FN )
    {
        switch(key)
        {
            case MACRO_1: return "MACRO_1";
            case MACRO_2: return "MACRO_2";
            case MACRO_3: return "MACRO_3";
            case MACRO_4: return "MACRO_4";
            case MACRO_5: return "MACRO_5";
            case MACRO_6: return "MACRO_6";
            case MACRO_7: return "MACRO_7";
            case MACRO_8: return "MACRO_8";
            case MACRO_9: return "MACRO_9";
            case MACRO_10: return "MACRO_10";
        }
    }
    else {
        switch(key)
        {
            case RESET: return "RESET" ;
            case STOP: return "STOP" ;
            case START_PAUSE: return "START_PAUSE" ;
            case FEED_UP: return "FEED_UP" ;
            case FEED_DOWN: return "FEED_DOWN" ;
            case SPINDLE_UP: return "SPINDLE_UP" ;
            case SPINDLE_DOWN: return "SPINDLE_DOWN" ;
            case M_HOME: return "M_HOME" ;
            case SAFE_Z: return "SAFE_Z" ;
            case W_HOME: return "W_HOME" ;
            case S_ON_OFF: return "S_ON_OFF" ;
            case FN: return "FN" ;
            case PROBE_Z: return "PROBE_Z" ;
            case CONTINUOUS: return "CONTINUOUS" ;
            case STEP: return "STEP" ;
            case MACRO_10: return "MACRO_10" ;
        }
    }
    return "NO KEY";
}

void printState(char buf [])
{
    char key = buf[2];
    char mod = buf[3];
    if(mod)
    {
        mod=buf[2];
        key=buf[3];
    }
    printf("AXIS:(%02x)%s   STEP:(%02x)%s   KEY:%s\n",  
            buf[5], axisName(buf[5]), 
            (unsigned char)buf[4], stepName(buf[4]), 
            keyName(key,mod));
}
