#include <iostream>
#include <iterator>
#include <cerrno>
#include <cstdio>
#include <algorithm>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <whb04bhid.hpp>
#include <hidapi.h>
#include <unistd.h>

//#define DEBUG

void doHID( hid_device *handle);

void dumpDevices(void)
{
	struct hid_device_info *devs, *cur_dev;

    std::cerr << "HIDAPI Version:" << HID_API_VERSION_STR << std::endl;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    while (cur_dev) {
        std::cerr << "Device Found" << std::endl
            << "  vendor:       0x" << std::hex << std::setfill('0') << std::setw(4) << cur_dev->vendor_id << std::endl
            << "  product:      0x" << std::hex << std::setfill('0') << std::setw(4) << cur_dev->product_id << std::endl
            << std::dec
            << "  path:         " << cur_dev->path << std::endl
            << "  serial_number:" << cur_dev->serial_number << std::endl
            << "  Manufacturer: " << cur_dev->manufacturer_string << std::endl
            << "  Product:      " << cur_dev->product_string << std::endl
            << "  Release:      " << cur_dev->release_number << std::endl
            << "  Interface:    " << cur_dev->interface_number << std::endl
            << "  Usage:        " << cur_dev->usage <<std::endl
            << "  Usage Page:   " << cur_dev->usage_page << std::endl
            << std::endl;
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
}

int main(int argc, char **argv)
{
	hid_device *handle;

    while( true )
    {
        if (hid_init())
        {
            std::cerr << "Unable to initialize HIDAPI " << std::strerror(errno) << std::endl;
        }
        else
        {
            handle = hid_open(WHB04B_VENDOR, WHB04B_PRODUCT, NULL);
            if (!handle) {
                std::cerr << "Unable to open WHB04B device "
                    << std::hex << std::setfill('0') << std::setw(4)
                    << "V:0x" << WHB04B_VENDOR 
                    << " P:0x" << WHB04B_PRODUCT
                    << " - " << std::strerror(errno) 
                    << std::endl;
            }
            else
            {
                //dumpDevices();
                // Process HID messages
                doHID(handle);
                hid_close(handle);
            }

            // Clean up
            hid_exit();
        }

        // Delay before trying again
        usleep(1000*1000);
    }
    return 0;
}

void doHID( hid_device *handle)
{
	int res;
	unsigned char buf[256];
    WHB04B pendant;

    // Make HID reads nonblocking
    hid_set_nonblocking(handle, 1);

    // Loop until read fails
    while (1) {
        res = hid_read_timeout(handle, buf, sizeof(buf), 5000);
        if( res == 0 )
        {
            continue;
        }
        if( res < 0 )
        {
            return;
        }
        pendant.handleHIDBuffer(buf,res);
    }
}

#if 0 // maybe later

#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

static void do_something(GtkWidget *widget, gpointer data) {
  //...
}

int main (int argc, char **argv) {
  gtk_init(&argc, &argv);
  GtkWidget *menu = gtk_menu_new();
  GtkWidget *menuitem = gtk_menu_item_new_with_mnemonic("_This is a menu item");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
  g_signal_connect(menuitem, "activate", G_CALLBACK(do_something), NULL);

  /* the second parameter is the icon displayed */
  AppIndicator* ind = app_indicator_new("test", "indicator-messages-new", 
                     APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
  app_indicator_set_menu(ind, GTK_MENU(menu));
  gtk_main();
}
#endif
