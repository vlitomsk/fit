#include <iostream>
#include <libusb-1.0/libusb.h>
#include <cstdlib>
#include <string>
#include <sstream>

using namespace std;

void processdev(ostream& out, libusb_device *dev);

int main(int argc, char **argv) {
  libusb_device **devs;
  libusb_context *ctx = NULL;
  int r;
  ssize_t cnt;
  ssize_t i;
  r = libusb_init(&ctx);
  if (r < 0) {
    cerr << "libusb init failed" << endl;
    throw 1;
  }

  libusb_set_debug(ctx, 3);
  cnt = libusb_get_device_list(ctx, &devs);
  if (cnt < 0) {
    cerr << "get device list failed" << endl;
    throw 1;
  }

  ostream& out = cout;
  out << "Total: " << cnt << " devices" << endl;
  for (i = 0; i < cnt; ++i) {
      out << "- - - - - - - -" << endl;
    processdev(out, devs[i]);
  }

  libusb_free_device_list(devs, 1);
  libusb_exit(NULL);

  return EXIT_SUCCESS;
}

string getClassString(uint8_t bclass) {
  switch (bclass) {
    case 0x00 : return "No class";
    case 0x01 : return "Audio device";
    case 0x02 : return "Network adapter";
    case 0x03 : return "HID";
    case 0x05 : return "Physical device";
    case 0x06 : return "Image";
    case 0x07 : return "Printer";
    case 0x08 : return "Storage device";
    case 0x09 : return "Hub";
    case 0x0b : return "Smart-card";
    case 0x0d : return "Content security device";
    case 0x0e : return "Video device";
    case 0x0f : return "Personal medical device";
    case 0x10 : return "Audio/video device";
    case 0xdc : return "Diagnostic device";
    case 0xe0 : return "Wireless controller";
    case 0xef : return "Some device";
    case 0xfe : return "Specific device";
  }
  return "Unsupported class";
}

#define CTRL_IN (LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE)
#define HID_GET_IDLE 0x02

void processdev(ostream& out, libusb_device *dev) {
  libusb_device_descriptor desc;
  libusb_config_descriptor *config;
  libusb_device_handle *handle;
  unsigned char sn[256];

  int r = libusb_get_device_descriptor(dev, &desc);
  if (r < 0) {
    cerr << "can't get device descriptor" << endl;
    return;
  }

  libusb_get_config_descriptor(dev, 0, &config);
  out << "Vendor ID:  [0x" << hex << desc.idVendor << "]" << endl;
  out << "Product ID: [0x" << desc.idProduct << "]" << dec << endl;
  r = libusb_open(dev, &handle);
  if (r < 0) {
    cerr << "can't open device" << endl;
    return;
  }
  r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, sn, sizeof(sn));
  if (r >= 0) 
    out << "Device SN: [" << sn << "]" << endl;
 
  if (desc.bDeviceClass) 
    out << "Device class : [" << getClassString(desc.bDeviceClass) << "]" << endl;

  for (size_t i = 0; i < config->bNumInterfaces; ++i) {
    const libusb_interface& inter = config->interface[i];
    for (size_t j = 0; j < inter.num_altsetting; ++j) {
      const libusb_interface_descriptor& interdesc = inter.altsetting[j];
      if (!desc.bDeviceClass) 
        out << "Interface device class : [" << getClassString(interdesc.bInterfaceClass) << "]" << endl;
      for (size_t k = 0; k < interdesc.bNumEndpoints; ++k) {
        const libusb_endpoint_descriptor& epdesc = interdesc.endpoint[k];
      }
    }
  }

  if (desc.idVendor == 0x458) {
    r = libusb_detach_kernel_driver(handle, 0);
    if (r >= 0)
      r = libusb_set_configuration(handle, 1);
    if (r >= 0)
      r = libusb_claim_interface(handle, 0);

    unsigned char buffer[2];

    if (r >= 0)
      r = libusb_control_transfer(handle, CTRL_IN, HID_GET_IDLE, 0, 0, buffer, 2, 5000);
    out << (r >= 0 ? "Transfer OK" : "Transfer failed") << endl;
    libusb_release_interface(handle, 0);
    libusb_attach_kernel_driver(handle, 0);
  } else
    out << "No device -- no transfer" << endl;

  libusb_close(handle);

  libusb_free_config_descriptor(config);
}

