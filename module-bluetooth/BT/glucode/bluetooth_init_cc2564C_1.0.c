// init script created from
// - initscripts-TIInit_6.12.26_v1.0.bts
// - AKA TIInit_6.12.26.bts
// - initscripts-TIInit_6.12.26_ble_add-on_v1.0.bts
#include <stdint.h>
#include "btstack_chipset_cc256x.h"


const uint16_t cc256x_init_script_lmp_subversion = 0x9a1a;

uint16_t btstack_chipset_cc256x_lmp_subversion(void){
    return cc256x_init_script_lmp_subversion;
}

#if defined(__GNUC__) && defined(__MSP430X__) && (__MSP430X__ > 0)
__attribute__((section (".fartext")))
#endif
#ifdef __AVR__
__attribute__((__progmem__))
#endif
const uint8_t cc256x_init_script[] = {

    // #--------------------------------------------------------------------------------
    // # Description : Orca C ROM Initialization Script
    // #
    // # Compatibility: Orca, 12.0.26 ROM
    // #
    // # Last Updated: 29-Sep-2016  14:08:47.69
    // #
    // # Version     : 12_26.12
    // #
    // #
    // #
    // #
    // # Notes       : Use this script on Orca C, 12.0.26 ROM device only (FW v12.0.26)
    // #--------------------------------------------------------------------------------
    // 
    // #################################################################
    // ## START of CC256x Add-On
    // #################################################################
    // 
    // ## Change UART baudrate
    // 
    // #################################################################
    // ## END of CC256x Add-On
    // #################################################################
    // 
    0x01, 0x37, 0xfe, 0x02, 0x0c, 0x1a, 

    // 
    // 
    0x01, 0x05, 0xff, 0xff, 0xd0, 0x65, 0x08, 0x00, 0xfa, 0x0c, 0x1a, 0x09, 0x0c, 0x01, 0x6a, 
    0xc8, 0x7b, 0x00, 0x02, 0x89, 0x7b, 0x01, 0x43, 0x09, 0x48, 0x51, 0x30, 0x02, 0x88, 0x06, 
    0x48, 0x91, 0x42, 0x03, 0xd1, 0x04, 0x49, 0x09, 0x78, 0x01, 0x29, 0x01, 0xd0, 0x5d, 0x30, 
    0xf7, 0x46, 0xff, 0x30, 0xd8, 0x30, 0xf7, 0x46, 0x76, 0x24, 0x08, 0x00, 0xbd, 0x28, 0x02, 
    0x00, 0x69, 0x53, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb5, 0x00, 0x69, 
    0xff, 0x21, 0x02, 0x31, 0x09, 0x5c, 0x09, 0x29, 0x05, 0xd1, 0x01, 0x21, 0x00, 0x22, 0x8e, 
    0x46, 0xcb, 0x4b, 0xfe, 0x44, 0x18, 0x47, 0x00, 0xbd, 0xca, 0x4a, 0x11, 0x88, 0x01, 0x20, 
    0x40, 0x03, 0x08, 0x43, 0x10, 0x80, 0xf7, 0x46, 0x30, 0xb5, 0x00, 0x69, 0xf4, 0x21, 0x08, 
    0x5c, 0x01, 0x28, 0x16, 0xd1, 0xc5, 0x48, 0x00, 0x78, 0x03, 0x28, 0x12, 0xd0, 0x00, 0x25, 
    0x28, 0x1c, 0xc3, 0x49, 0x01, 0x24, 0xa6, 0x46, 0xc2, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x00, 
    0x28, 0x08, 0xd0, 0xc1, 0x49, 0xc1, 0x4a, 0xbb, 0x32, 0x20, 0x20, 0x2b, 0x1c, 0xa6, 0x46, 
    0xc0, 0x4c, 0xfe, 0x44, 0x20, 0x47, 0x30, 0xbd, 0x70, 0xb5, 0x85, 0x69, 0x00, 0x7d, 0x80, 
    0x1f, 0x11, 0xd0, 0x47, 0x38, 0x2e, 0xd1, 0xa9, 0x79, 0x28, 0x20, 0x48, 0x43, 0xba, 0x4a, 
    0x10, 0x18, 0x23, 0x22, 0x12, 0x5c, 0x01, 0x2a, 0x25, 0xd1, 0x80, 0x7b, 0x00, 0x28, 0x22, 
    0xd0, 0xb6, 0x4a, 0x00, 0x20, 0x50, 0x54, 0x70, 0xbd, 0x01, 0x24, 0xa6, 0x46, 0xef, 0x48, 
    0xfe, 0x44, 0x00, 0x47, 

    0x01, 0x05, 0xff, 0xff, 0xca, 0x66, 0x08, 0x00, 0xfa, 0x01, 0x28, 0x05, 0xd0, 0xa6, 0x46, 
    0xec, 0x48, 0xfe, 0x44, 0x00, 0x47, 0x04, 0x28, 0x11, 0xd1, 0xe8, 0x78, 0x00, 0x28, 0x0e, 
    0xd1, 0x0e, 0x26, 0x31, 0x1c, 0xe9, 0x4d, 0x28, 0x1c, 0x14, 0x38, 0xa6, 0x46, 0xe6, 0x4a, 
    0xfe, 0x44, 0x10, 0x47, 0x28, 0x1c, 0x31, 0x1c, 0xa6, 0x46, 0xe3, 0x4a, 0xfe, 0x44, 0x10, 
    0x47, 0x70, 0xbd, 0x70, 0xb5, 0x01, 0x1c, 0x88, 0x69, 0x89, 0x8a, 0xe1, 0x4a, 0x89, 0x1a, 
    0x1c, 0xd0, 0x1c, 0x39, 0x20, 0xd1, 0xc5, 0x7a, 0x01, 0x21, 0x0c, 0x1c, 0x8e, 0x46, 0xde, 
    0x4a, 0xfe, 0x44, 0x10, 0x47, 0x06, 0x1c, 0x10, 0x20, 0xa6, 0x46, 0xdc, 0x49, 0xfe, 0x44, 
    0x08, 0x47, 0x00, 0x2d, 0x11, 0xd0, 0x02, 0x2e, 0x0f, 0xd0, 0xd9, 0x49, 0xda, 0x4a, 0xd9, 
    0x32, 0x20, 0x20, 0x00, 0x23, 0xa6, 0x46, 0x92, 0x4c, 0xfe, 0x44, 0x20, 0x47, 0x70, 0xbd, 
    0xc0, 0x7a, 0x00, 0x28, 0x02, 0xd1, 0x00, 0x20, 0xd4, 0x49, 0x08, 0x70, 0x70, 0xbd, 0x00, 
    0xb5, 0x00, 0x69, 0xff, 0x21, 0x04, 0x31, 0x09, 0x5c, 0x06, 0x29, 0x06, 0xd1, 0xff, 0x21, 
    0x05, 0x31, 0x0a, 0x5c, 0x2c, 0x2a, 0x01, 0xd1, 0x2d, 0x22, 0x0a, 0x54, 0xff, 0x21, 0x05, 
    0x31, 0x09, 0x5c, 0x34, 0x29, 0x11, 0xd1, 0xff, 0x21, 0x0b, 0x31, 0x09, 0x5c, 0x91, 0x29, 
    0x0c, 0xd1, 0xf1, 0x21, 0x09, 0x5c, 0x60, 0x22, 0x4a, 0x43, 0xc6, 0x4b, 0x00, 0x21, 0x99, 
    0x54, 0x06, 0x21, 0x01, 0x22, 0x96, 0x46, 0xc4, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x00, 0xbd, 
    0xf0, 0xb5, 0x06, 0x1c, 0xf7, 0x69, 0x08, 0x20, 0xc0, 0x19, 0x01, 0x24, 0xa6, 0x46, 0xbf, 
    0x49, 0xfe, 0x44, 0x08, 0x47, 0xc1, 0x7b, 0x09, 0x02, 0x80, 0x7b, 0x08, 0x43, 0x05, 0x04, 
    0x2d, 0x0c, 0x02, 0x2d, 

    0x01, 0x05, 0xff, 0xff, 0xc4, 0x67, 0x08, 0x00, 0xfa, 0x12, 0xd0, 0xbb, 0x48, 0x00, 0x88, 
    0xa8, 0x42, 0x0e, 0xd1, 0xba, 0x48, 0x00, 0x78, 0x01, 0x28, 0x0a, 0xd1, 0xb9, 0x48, 0x82, 
    0x8f, 0x81, 0x6b, 0x08, 0x1c, 0x10, 0x43, 0x04, 0xd0, 0x38, 0x1c, 0xa6, 0x46, 0xb6, 0x4b, 
    0xfe, 0x44, 0x18, 0x47, 0x35, 0x60, 0xb5, 0x48, 0x24, 0x30, 0x30, 0x62, 0xf0, 0xbd, 0xf0, 
    0xb5, 0x85, 0xb0, 0x04, 0x90, 0x87, 0x69, 0x81, 0x8a, 0xb1, 0x48, 0x08, 0x1a, 0x74, 0xd0, 
    0xb1, 0x49, 0x40, 0x1a, 0x28, 0xd0, 0xb0, 0x49, 0x40, 0x1a, 0x1d, 0xd1, 0xb0, 0x48, 0x05, 
    0x1c, 0x84, 0x3d, 0x00, 0x78, 0x02, 0x28, 0x17, 0xd1, 0x01, 0x24, 0xa6, 0x46, 0xad, 0x48, 
    0xfe, 0x44, 0x00, 0x47, 0x00, 0x28, 0x10, 0xd1, 0xa6, 0x46, 0xab, 0x48, 0xfe, 0x44, 0x00, 
    0x47, 0x00, 0x28, 0x0a, 0xd1, 0xa6, 0x46, 0xa9, 0x48, 0xfe, 0x44, 0x00, 0x47, 0x00, 0x28, 
    0x04, 0xd1, 0x28, 0x78, 0x00, 0x28, 0x01, 0xd1, 0xa8, 0x78, 0x00, 0x28, 0x00, 0xd0, 0x8b, 
    0xe0, 0xa6, 0x20, 0xa6, 0x46, 0xa3, 0x49, 0xfe, 0x44, 0x08, 0x47, 0x85, 0xe0, 0x95, 0x48, 
    0x51, 0x38, 0x00, 0x78, 0x2a, 0x28, 0x7c, 0xd1, 0x0e, 0x20, 0xc6, 0x19, 0x44, 0x20, 0x0c, 
    0x21, 0x1a, 0x22, 0x01, 0x24, 0xa6, 0x46, 0x9c, 0x4b, 0xfe, 0x44, 0x18, 0x47, 0x9b, 0x48, 
    0x81, 0x78, 0xc2, 0x78, 0x9b, 0x48, 0xa6, 0x46, 0x9b, 0x4b, 0xfe, 0x44, 0x18, 0x47, 0x06, 
    0x20, 0xb8, 0x80, 0x08, 0x25, 0x35, 0x70, 0x01, 0x36, 0x30, 0x1c, 0x00, 0x21, 0xa6, 0x46, 
    0x96, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x06, 0x1c, 0x35, 0x70, 0x01, 0x36, 0x30, 0x1c, 0x0d, 
    0x21, 0xa6, 0x46, 0x92, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x06, 0x1c, 0xa6, 0x46, 0x91, 0x48, 
    0xfe, 0x44, 0x00, 0x47, 

    0x01, 0x05, 0xff, 0xff, 0xbe, 0x68, 0x08, 0x00, 0xfa, 0x81, 0x02, 0x8a, 0x48, 0xc0, 0x78, 
    0x40, 0x06, 0x40, 0x0e, 0x08, 0x43, 0x8c, 0x49, 0x01, 0x43, 0x09, 0x04, 0x09, 0x0c, 0x30, 
    0x1c, 0xa6, 0x46, 0x88, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x7d, 0x80, 0x38, 0x1c, 0xff, 0x21, 
    0x02, 0x31, 0x00, 0x22, 0xa6, 0x46, 0x86, 0x4b, 0xfe, 0x44, 0x18, 0x47, 0x38, 0xe0, 0x38, 
    0x1c, 0x00, 0x21, 0x6a, 0x46, 0x01, 0x24, 0xa6, 0x46, 0x84, 0x4b, 0xfe, 0x44, 0x18, 0x47, 
    0x02, 0xa8, 0x00, 0x21, 0x06, 0x22, 0xa6, 0x46, 0x81, 0x4b, 0xfe, 0x44, 0x18, 0x47, 0x00, 
    0x25, 0x02, 0xe0, 0x68, 0x1c, 0x05, 0x04, 0x2d, 0x0c, 0x66, 0x48, 0x4f, 0x38, 0x00, 0x78, 
    0x85, 0x42, 0x23, 0xda, 0x11, 0x20, 0x40, 0x01, 0x68, 0x43, 0x7a, 0x49, 0x0e, 0x18, 0x10, 
    0x20, 0x80, 0x19, 0x69, 0x46, 0xa6, 0x46, 0x78, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x00, 0x28, 
    0xe9, 0xd1, 0x68, 0x46, 0x02, 0xa9, 0xa6, 0x46, 0x74, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x00, 
    0x28, 0xe1, 0xd0, 0xb0, 0x78, 0x01, 0x28, 0xde, 0xd0, 0x38, 0x1c, 0x5c, 0x49, 0x3a, 0x22, 
    0xa6, 0x46, 0x6f, 0x4b, 0xfe, 0x44, 0x18, 0x47, 0x69, 0x49, 0xe2, 0x31, 0x04, 0x98, 0x01, 
    0x62, 0x05, 0xb0, 0xf0, 0xbd, 0xc0, 0x46, 0xdd, 0x9d, 0x00, 0x00, 0x3e, 0xa6, 0x1b, 0x00, 
    0x18, 0x32, 0x08, 0x00, 0xce, 0x04, 0x00, 0x00, 0x23, 0xb9, 0x02, 0x00, 0xf1, 0x6b, 0x08, 
    0x00, 0xfd, 0x79, 0x00, 0x00, 0x35, 0x6b, 0x08, 0x00, 0xd4, 0x1d, 0x08, 0x00, 0x94, 0x54, 
    0x08, 0x00, 0xfe, 0xb5, 0x00, 0x90, 0x81, 0x69, 0x01, 0x91, 0x4c, 0x78, 0xc6, 0x69, 0x25, 
    0x20, 0x20, 0x1a, 0x5d, 0x49, 0x32, 0xd0, 0x01, 0x38, 0x68, 0xd1, 0x05, 0x23, 0x03, 0x20, 
    0x1a, 0x1c, 0x01, 0x25, 

    0x01, 0x05, 0xff, 0xff, 0xb8, 0x69, 0x08, 0x00, 0xfa, 0xae, 0x46, 0x5a, 0x4f, 0xfe, 0x44, 
    0x38, 0x47, 0x04, 0x20, 0xb0, 0x80, 0x0c, 0x20, 0x87, 0x19, 0x61, 0x00, 0x01, 0x98, 0x00, 
    0x78, 0x08, 0x43, 0x38, 0x70, 0x08, 0x20, 0x78, 0x70, 0x02, 0x37, 0x38, 0x1c, 0x0d, 0x21, 
    0xae, 0x46, 0x46, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x07, 0x1c, 0xae, 0x46, 0x45, 0x48, 0xfe, 
    0x44, 0x00, 0x47, 0x82, 0x02, 0x4d, 0x48, 0x00, 0x78, 0x40, 0x06, 0x41, 0x0e, 0x11, 0x43, 
    0x40, 0x48, 0x08, 0x43, 0x01, 0x04, 0x09, 0x0c, 0x38, 0x1c, 0xae, 0x46, 0x3c, 0x4a, 0xfe, 
    0x44, 0x10, 0x47, 0x03, 0x20, 0x02, 0x90, 0x2e, 0xe0, 0x03, 0x20, 0x02, 0x90, 0x04, 0x22, 
    0x13, 0x1c, 0x01, 0x25, 0xae, 0x46, 0x41, 0x4f, 0xfe, 0x44, 0x38, 0x47, 0x04, 0x20, 0xb0, 
    0x80, 0x0c, 0x20, 0x87, 0x19, 0x61, 0x00, 0x01, 0x98, 0x00, 0x78, 0x08, 0x43, 0x38, 0x70, 
    0x08, 0x20, 0x78, 0x70, 0x02, 0x37, 0x38, 0x1c, 0x0d, 0x21, 0xae, 0x46, 0x2d, 0x4a, 0xfe, 
    0x44, 0x10, 0x47, 0x07, 0x1c, 0xae, 0x46, 0x2c, 0x48, 0xfe, 0x44, 0x00, 0x47, 0x82, 0x02, 
    0x34, 0x48, 0x00, 0x78, 0x40, 0x06, 0x41, 0x0e, 0x11, 0x43, 0x27, 0x48, 0x08, 0x43, 0x01, 
    0x04, 0x09, 0x0c, 0x38, 0x1c, 0xae, 0x46, 0x23, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x06, 0x20, 
    0x70, 0x80, 0x02, 0x98, 0x30, 0x80, 0x2d, 0x48, 0x2c, 0x49, 0x09, 0x18, 0x00, 0x98, 0x01, 
    0x62, 0xfe, 0xbd, 0x8f, 0x8d, 0x01, 0x00, 0xfd, 0x06, 0x05, 0x00, 0x14, 0x05, 0x1a, 0x00, 
    0xa2, 0xfd, 0x00, 0x00, 0x65, 0x2d, 0x00, 0x00, 0x7d, 0xca, 0x03, 0x00, 0x31, 0x6c, 0x08, 
    0x00, 0x79, 0x47, 0x00, 0x00, 0x78, 0x24, 0x08, 0x00, 0x76, 0xa0, 0x1b, 0x00, 0xc5, 0x8e, 
    0x00, 0x00, 0x5b, 0x19, 

    0x01, 0x05, 0xff, 0xff, 0xb2, 0x6a, 0x08, 0x00, 0xfa, 0x04, 0x00, 0xba, 0x53, 0x08, 0x00, 
    0x90, 0xa1, 0x1b, 0x00, 0xa8, 0x59, 0x08, 0x00, 0x25, 0x6f, 0x04, 0x00, 0x79, 0x6c, 0x04, 
    0x00, 0x05, 0x04, 0x00, 0x00, 0xfc, 0x0b, 0x00, 0x00, 0x1d, 0x10, 0x00, 0x00, 0x45, 0x10, 
    0x08, 0x00, 0xc1, 0x72, 0x03, 0x00, 0x1b, 0x5f, 0x03, 0x00, 0x53, 0x38, 0x02, 0x00, 0x21, 
    0xf0, 0x04, 0x00, 0xdb, 0x8e, 0x04, 0x00, 0xfc, 0x53, 0x08, 0x00, 0xc6, 0x02, 0x00, 0x00, 
    0x8d, 0x8f, 0x04, 0x00, 0xf9, 0x2d, 0x00, 0x00, 0x00, 0x82, 0xff, 0xff, 0xa9, 0x57, 0x05, 
    0x00, 0xed, 0x49, 0x02, 0x00, 0x25, 0x00, 0x00, 0x00, 0x99, 0x2d, 0x00, 0x00, 0xe9, 0x63, 
    0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x49, 0x8f, 0x03, 0x00, 0x85, 0x48, 0x02, 0x00, 0xc5, 
    0x05, 0x00, 0x00, 0x31, 0x90, 0x04, 0x00, 0xff, 0x53, 0x08, 0x00, 0x4e, 0x05, 0x00, 0x00, 
    0xb5, 0xcc, 0x00, 0x00, 0x40, 0x1e, 0x80, 0x00, 0x8c, 0x4b, 0x19, 0x50, 0x8a, 0x49, 0x0a, 
    0x50, 0xf7, 0x46, 0xf0, 0xb5, 0x01, 0x1c, 0x88, 0x69, 0x82, 0x88, 0x53, 0x04, 0x5b, 0x0c, 
    0x88, 0x4e, 0x88, 0x4f, 0x89, 0x4d, 0x03, 0xd0, 0x60, 0x2b, 0x01, 0xdc, 0xb2, 0x42, 0x14, 
    0xd0, 0x82, 0x88, 0x01, 0x23, 0x1b, 0x03, 0x54, 0x04, 0x64, 0x0f, 0x24, 0x03, 0x9c, 0x42, 
    0x08, 0xdb, 0x3b, 0x1c, 0x01, 0x33, 0x54, 0x04, 0x24, 0x0d, 0xe4, 0x00, 0x9c, 0x42, 0x01, 
    0xda, 0xba, 0x42, 0x03, 0xd0, 0xff, 0x20, 0x88, 0x60, 0xe8, 0x1d, 0xf0, 0xbd, 0x01, 0x24, 
    0xa6, 0x46, 0x7b, 0x49, 0xfe, 0x44, 0x08, 0x47, 0x38, 0x1c, 0xa6, 0x46, 0x7a, 0x49, 0xfe, 
    0x44, 0x08, 0x47, 0x30, 0x1c, 0xa6, 0x46, 0x77, 0x49, 0xfe, 0x44, 0x08, 0x47, 0xa6, 0x46, 
    0x76, 0x48, 0xfe, 0x44, 

    0x01, 0x05, 0xff, 0xff, 0xac, 0x6b, 0x08, 0x00, 0xfa, 0x00, 0x47, 0x38, 0x1c, 0xa6, 0x46, 
    0x73, 0x49, 0xfe, 0x44, 0x08, 0x47, 0xe8, 0x48, 0x01, 0x68, 0x30, 0x1c, 0xa6, 0x46, 0x71, 
    0x4a, 0xfe, 0x44, 0x10, 0x47, 0x71, 0x48, 0x40, 0x19, 0xf0, 0xbd, 0x01, 0x1c, 0x0a, 0x7d, 
    0x6f, 0x48, 0x00, 0x2a, 0x02, 0xd0, 0xc9, 0x68, 0x01, 0x29, 0x01, 0xd0, 0x4f, 0x30, 0xf7, 
    0x46, 0x31, 0x30, 0xf7, 0x46, 0x41, 0x68, 0x02, 0x39, 0x41, 0x60, 0xe9, 0x48, 0x3f, 0x30, 
    0xf7, 0x46, 0x1c, 0xb5, 0x41, 0x68, 0xe7, 0x4c, 0x00, 0x29, 0x17, 0xd0, 0x41, 0x69, 0xb0, 
    0x20, 0x40, 0x18, 0xb8, 0x31, 0x6a, 0x46, 0x01, 0x23, 0x9e, 0x46, 0xe9, 0x4b, 0xfe, 0x44, 
    0x18, 0x47, 0x00, 0x99, 0xe8, 0x48, 0xe4, 0x38, 0x41, 0x43, 0x68, 0x46, 0x80, 0x88, 0x41, 
    0x18, 0xff, 0x20, 0xae, 0x30, 0x81, 0x42, 0x02, 0xd9, 0x20, 0x1c, 0xbf, 0x30, 0x1c, 0xbd, 
    0x20, 0x1c, 0xdf, 0x30, 0x1c, 0xbd, 0x10, 0xb5, 0x04, 0x1c, 0xc8, 0x68, 0x0a, 0x21, 0x01, 
    0x22, 0x96, 0x46, 0xde, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x00, 0x28, 0x00, 0xd1, 0x8c, 0x20, 
    0xa0, 0x60, 0xdc, 0x48, 0xdb, 0x30, 0x10, 0xbd, 0xdc, 0x49, 0x04, 0x39, 0x09, 0x78, 0x00, 
    0x29, 0x01, 0xd1, 0x00, 0x21, 0x41, 0x60, 0x41, 0x68, 0x42, 0x69, 0x51, 0x1a, 0x41, 0x60, 
    0xd6, 0x48, 0x55, 0x30, 0xf7, 0x46, 0xd6, 0x49, 0x09, 0x78, 0x00, 0x29, 0x04, 0xd1, 0x01, 
    0x7d, 0xd3, 0x48, 0x02, 0x30, 0xff, 0x22, 0x42, 0x54, 0xd3, 0x48, 0x4f, 0x30, 0xf7, 0x46, 
    0x01, 0x1c, 0x8a, 0x69, 0x4b, 0x68, 0xd1, 0x48, 0x9a, 0x42, 0x01, 0xd9, 0x3b, 0x30, 0xf7, 
    0x46, 0xca, 0x60, 0x79, 0x30, 0xf7, 0x46, 0xcf, 0x48, 0xcd, 0x49, 0x08, 0x80, 0xce, 0x48, 
    0xff, 0x30, 0xde, 0x30, 

    0x01, 0x05, 0xff, 0xff, 0xa6, 0x6c, 0x08, 0x00, 0xfa, 0xf7, 0x46, 0xc2, 0x69, 0xff, 0x21, 
    0x11, 0x31, 0x8b, 0x5c, 0xcb, 0x49, 0x5b, 0x08, 0x08, 0xd3, 0xff, 0x23, 0x02, 0x33, 0x9a, 
    0x5c, 0x02, 0x2a, 0x03, 0xd0, 0x01, 0x2a, 0x01, 0xd0, 0x03, 0x2a, 0x02, 0xd1, 0x08, 0x1c, 
    0x5b, 0x30, 0xf7, 0x46, 0xff, 0x22, 0x42, 0x60, 0x08, 0x1c, 0x39, 0x30, 0xf7, 0x46, 0x00, 
    0xb5, 0x40, 0x68, 0x01, 0x21, 0x8e, 0x46, 0xc0, 0x49, 0xfe, 0x44, 0x08, 0x47, 0xc0, 0x48, 
    0x57, 0x30, 0x00, 0xbd, 0x02, 0x8a, 0x01, 0x79, 0x0a, 0x29, 0x00, 0xdb, 0x0a, 0x21, 0xbd, 
    0x48, 0x8a, 0x42, 0x01, 0xdd, 0x5f, 0x30, 0xf7, 0x46, 0x5b, 0x30, 0xf7, 0x46, 0xf0, 0xb5, 
    0x01, 0x24, 0xa6, 0x46, 0xb9, 0x48, 0xfe, 0x44, 0x00, 0x47, 0x01, 0x28, 0x05, 0xd0, 0xa6, 
    0x46, 0xb6, 0x48, 0xfe, 0x44, 0x00, 0x47, 0x04, 0x28, 0x19, 0xd1, 0x02, 0x26, 0xb4, 0x4d, 
    0x28, 0x79, 0x00, 0x28, 0x11, 0xd0, 0xe8, 0x7a, 0x06, 0x28, 0x0e, 0xd1, 0x0e, 0x20, 0x01, 
    0x1c, 0xb1, 0x4f, 0x38, 0x1c, 0x14, 0x38, 0xa6, 0x46, 0xae, 0x4a, 0xfe, 0x44, 0x10, 0x47, 
    0x38, 0x1c, 0x0e, 0x21, 0xa6, 0x46, 0xab, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x70, 0x35, 0x01, 
    0x3e, 0xe7, 0xd1, 0xa6, 0x46, 0xaa, 0x48, 0xfe, 0x44, 0x00, 0x47, 0xa9, 0x49, 0x0b, 0x48, 
    0x54, 0x30, 0x40, 0x18, 0xf0, 0xbd, 0xc0, 0x46, 0x04, 0xf3, 0x1a, 0x00, 0x80, 0x7b, 0x08, 
    0x00, 0x03, 0x80, 0x00, 0x00, 0x17, 0x10, 0x00, 0x00, 0xc9, 0xfb, 0x04, 0x00, 0xbb, 0x15, 
    0x04, 0x00, 0x7d, 0xca, 0x03, 0x00, 0x05, 0x43, 0x02, 0x00, 0x0b, 0xc9, 0x03, 0x00, 0xab, 
    0x02, 0x00, 0x00, 0xb1, 0x33, 0x02, 0x00, 0xf0, 0xb5, 0x8d, 0xb0, 0x01, 0x90, 0x81, 0x69, 
    0x02, 0x91, 0x01, 0x7d, 

    0x01, 0x05, 0xff, 0xff, 0xa0, 0x6d, 0x08, 0x00, 0xfa, 0x03, 0x91, 0x42, 0x68, 0x00, 0x92, 
    0x80, 0x8b, 0x40, 0x00, 0x04, 0x90, 0x6b, 0x48, 0x83, 0x30, 0x00, 0x78, 0x40, 0x00, 0x05, 
    0x90, 0x00, 0x29, 0x01, 0xd1, 0x00, 0x20, 0xe1, 0xe0, 0x04, 0x98, 0x02, 0x04, 0x12, 0x0c, 
    0x06, 0x92, 0x02, 0x98, 0x03, 0x99, 0x6b, 0x46, 0x01, 0x24, 0xa6, 0x46, 0x8e, 0x4d, 0xfe, 
    0x44, 0x28, 0x47, 0x07, 0x90, 0x00, 0x9e, 0x00, 0x20, 0x08, 0x90, 0x7e, 0x48, 0x09, 0x90, 
    0x0a, 0x90, 0x7f, 0xe0, 0x09, 0x98, 0x00, 0x28, 0x2b, 0xd0, 0x0b, 0x98, 0x09, 0x90, 0x28, 
    0xe0, 0x05, 0x98, 0x87, 0x42, 0x25, 0xdb, 0x90, 0x79, 0x03, 0x28, 0x01, 0xd0, 0x01, 0x28, 
    0x20, 0xd1, 0x79, 0x19, 0x05, 0x98, 0x08, 0x1a, 0x07, 0x99, 0xa6, 0x46, 0x80, 0x4a, 0xfe, 
    0x44, 0x10, 0x47, 0x07, 0x1c, 0x28, 0x1c, 0x07, 0x99, 0xa6, 0x46, 0x7d, 0x4a, 0xfe, 0x44, 
    0x10, 0x47, 0x01, 0x04, 0x09, 0x0c, 0x3a, 0x04, 0x12, 0x0c, 0x00, 0x20, 0xa6, 0x46, 0x79, 
    0x4b, 0xfe, 0x44, 0x18, 0x47, 0x00, 0x28, 0x04, 0xd1, 0x0a, 0x98, 0x00, 0x28, 0x03, 0xd0, 
    0x0a, 0x97, 0x01, 0xe0, 0x00, 0x20, 0x0a, 0x90, 0xb6, 0x68, 0x00, 0x2e, 0x03, 0xd0, 0x30, 
    0x88, 0x07, 0x99, 0x88, 0x42, 0x49, 0xdb, 0x08, 0x98, 0x00, 0x28, 0x46, 0xd1, 0x5f, 0x49, 
    0x09, 0x98, 0x88, 0x42, 0x42, 0xd1, 0x0a, 0x98, 0x88, 0x42, 0x3f, 0xd1, 0x03, 0x98, 0x00, 
    0x28, 0x2c, 0xd0, 0x02, 0x9d, 0x03, 0x98, 0x0c, 0x90, 0x00, 0x27, 0x28, 0x88, 0x0b, 0x90, 
    0x04, 0x99, 0xa6, 0x46, 0x64, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x00, 0x28, 0x07, 0xd0, 0x0b, 
    0x99, 0x04, 0x98, 0xa6, 0x46, 0x60, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0x00, 0x28, 0x11, 0xd1, 
    0x08, 0x98, 0xb8, 0x42, 

    0x01, 0x05, 0xff, 0xff, 0x9a, 0x6e, 0x08, 0x00, 0xfa, 0x09, 0xd0, 0x08, 0x98, 0xc1, 0x00, 
    0x02, 0x98, 0x40, 0x18, 0x29, 0x1c, 0x08, 0x22, 0xa6, 0x46, 0x5b, 0x4b, 0xfe, 0x44, 0x18, 
    0x47, 0x08, 0x98, 0x40, 0x1c, 0x00, 0x06, 0x00, 0x0e, 0x08, 0x90, 0x08, 0x35, 0x01, 0x37, 
    0x0c, 0x98, 0x01, 0x38, 0x0c, 0x90, 0xd6, 0xd1, 0x08, 0x98, 0x00, 0x28, 0x0c, 0xd0, 0x02, 
    0x98, 0x08, 0x99, 0x06, 0x9a, 0x6b, 0x46, 0xa6, 0x46, 0x4d, 0x4d, 0xfe, 0x44, 0x28, 0x47, 
    0x07, 0x90, 0x00, 0x9e, 0x3d, 0x48, 0x09, 0x90, 0x0a, 0x90, 0x00, 0x2e, 0x3a, 0xd0, 0x35, 
    0x88, 0x07, 0x98, 0x85, 0x42, 0x36, 0xda, 0xb0, 0x68, 0x00, 0x28, 0x05, 0xd0, 0x00, 0x88, 
    0x07, 0x99, 0x88, 0x42, 0x01, 0xdc, 0x47, 0x1b, 0x04, 0xe0, 0x07, 0x98, 0x40, 0x1b, 0x00, 
    0x99, 0x09, 0x88, 0x0f, 0x18, 0x72, 0x68, 0x91, 0x88, 0x05, 0x98, 0x40, 0x18, 0x87, 0x42, 
    0x00, 0xda, 0x6a, 0xe7, 0x48, 0x19, 0x07, 0x99, 0xa6, 0x46, 0x3b, 0x4a, 0xfe, 0x44, 0x10, 
    0x47, 0x0b, 0x90, 0x79, 0x19, 0x05, 0x98, 0x08, 0x1a, 0x07, 0x99, 0xa6, 0x46, 0x36, 0x4a, 
    0xfe, 0x44, 0x10, 0x47, 0x02, 0x04, 0x12, 0x0c, 0x0b, 0x98, 0x01, 0x04, 0x09, 0x0c, 0x00, 
    0x20, 0xa6, 0x46, 0x32, 0x4b, 0xfe, 0x44, 0x18, 0x47, 0x00, 0x28, 0x00, 0xd1, 0x48, 0xe7, 
    0x00, 0x20, 0x09, 0x90, 0x06, 0xe0, 0xc0, 0x46, 0xe0, 0x31, 0x08, 0x00, 0x1d, 0x48, 0x09, 
    0x99, 0x81, 0x42, 0x02, 0xd0, 0x09, 0x98, 0x0a, 0x90, 0x02, 0xe0, 0x0a, 0x99, 0x81, 0x42, 
    0x01, 0xd0, 0x0a, 0x98, 0x04, 0xe0, 0x06, 0x98, 0xa6, 0x46, 0x27, 0x49, 0xfe, 0x44, 0x08, 
    0x47, 0x01, 0x99, 0x48, 0x60, 0x20, 0x48, 0xff, 0x30, 0x10, 0x30, 0x0d, 0xb0, 0xf0, 0xbd, 
    0xb7, 0x4b, 0x04, 0x00, 

    0x01, 0x05, 0xff, 0xf3, 0x94, 0x6f, 0x08, 0x00, 0xee, 0xfd, 0x79, 0x00, 0x00, 0x22, 0x49, 
    0x09, 0x78, 0x2a, 0x29, 0x01, 0xd1, 0x08, 0x21, 0x00, 0xe0, 0x06, 0x21, 0x41, 0x60, 0x1d, 
    0x48, 0x43, 0x30, 0xf7, 0x46, 0xc0, 0x46, 0x4f, 0x81, 0x03, 0x00, 0xc6, 0x05, 0x00, 0x00, 
    0x65, 0x2d, 0x00, 0x00, 0x79, 0x47, 0x00, 0x00, 0x0d, 0x13, 0x02, 0x00, 0x76, 0x24, 0x08, 
    0x00, 0xe0, 0xa0, 0x1b, 0x00, 0x89, 0x28, 0x05, 0x00, 0x3d, 0x39, 0x02, 0x00, 0x60, 0x5b, 
    0x08, 0x00, 0xff, 0xff, 0x00, 0x00, 0xd9, 0xaa, 0x00, 0x00, 0xd5, 0x75, 0x00, 0x00, 0x23, 
    0x01, 0x05, 0x00, 0xb3, 0x09, 0x02, 0x00, 0xf9, 0x97, 0x00, 0x00, 0x8f, 0x8d, 0x01, 0x00, 
    0xa4, 0x13, 0x08, 0x00, 0xe5, 0x06, 0x05, 0x00, 0x14, 0x05, 0x1a, 0x00, 0x77, 0xc5, 0x01, 
    0x00, 0xb9, 0xc5, 0x01, 0x00, 0x6d, 0x95, 0x00, 0x00, 0xa1, 0x95, 0x01, 0x00, 0x95, 0x49, 
    0x05, 0x00, 0x21, 0x96, 0x01, 0x00, 0x5d, 0x5f, 0x05, 0x00, 0x4d, 0x96, 0x01, 0x00, 0x7b, 
    0x5e, 0x02, 0x00, 0x69, 0x53, 0x08, 0x00, 0xff, 0xb5, 0x68, 0x46, 0xff, 0xf7, 0xb9, 0xfb, 
    0xff, 0xbd, 0xff, 0xb5, 0x68, 0x46, 0xff, 0xf7, 0xde, 0xfb, 0xff, 0xbd, 0xff, 0xb5, 0x68, 
    0x46, 0xff, 0xf7, 0xaa, 0xfc, 0xff, 0xbd, 0xff, 0xb5, 0x68, 0x46, 0xff, 0xf7, 0x84, 0xfb, 
    0xff, 0xbd, 0xff, 0xb5, 0x68, 0x46, 0xff, 0xf7, 0xf8, 0xfa, 0xff, 0xbd, 0xff, 0xb5, 0x68, 
    0x46, 0xff, 0xf7, 0xe5, 0xfa, 0xff, 0xbd, 0xff, 0xb5, 0x68, 0x46, 0xff, 0xf7, 0xf5, 0xfa, 
    0xff, 0xbd, 0xff, 0xb5, 0x68, 0x46, 0xff, 0xf7, 0x45, 0xfb, 0xff, 0xbd, 0xff, 0xb5, 0x68, 
    0x46, 0xff, 0xf7, 0x09, 0xfb, 0xff, 0xbd, 

    0x01, 0x05, 0xff, 0x8d, 0x78, 0x7b, 0x08, 0x00, 0x88, 0x00, 0xb5, 0xf8, 0xf0, 0xa7, 0xfa, 
    0x00, 0xbd, 0x43, 0x6b, 0x08, 0x00, 0xcd, 0x6b, 0x08, 0x00, 0xd5, 0x65, 0x08, 0x00, 0xe5, 
    0x6b, 0x08, 0x00, 0x51, 0x6c, 0x08, 0x00, 0x6d, 0x6c, 0x08, 0x00, 0x85, 0x6c, 0x08, 0x00, 
    0x9b, 0x6c, 0x08, 0x00, 0xa9, 0x6c, 0x08, 0x00, 0xd9, 0x6c, 0x08, 0x00, 0xed, 0x6c, 0x08, 
    0x00, 0x05, 0x6d, 0x08, 0x00, 0x95, 0x6d, 0x08, 0x00, 0x99, 0x6f, 0x08, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x6c, 0x08, 0x00, 

    0x01, 0x05, 0xff, 0x85, 0x04, 0xf3, 0x1a, 0x00, 0x80, 0xce, 0xfb, 0x04, 0x00, 0xde, 0x33, 
    0x02, 0x00, 0x14, 0x29, 0x02, 0x00, 0xf4, 0x4b, 0x04, 0x00, 0x60, 0x13, 0x02, 0x00, 0xd6, 
    0x28, 0x05, 0x00, 0x74, 0x39, 0x02, 0x00, 0x88, 0xac, 0x00, 0x00, 0x0c, 0x76, 0x00, 0x00, 
    0x06, 0x0a, 0x02, 0x00, 0x50, 0x98, 0x00, 0x00, 0xb4, 0xc6, 0x01, 0x00, 0x7a, 0x95, 0x00, 
    0x00, 0xb8, 0x5e, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x52, 0x48, 
    0x00, 0x00, 

    0x01, 0x05, 0xff, 0xff, 0x00, 0x00, 0x18, 0x00, 0xfa, 0x70, 0xb5, 0x25, 0x4d, 0xae, 0x7f, 
    0x01, 0x24, 0xa6, 0x46, 0x21, 0x48, 0xfe, 0x44, 0x00, 0x47, 0xb0, 0x42, 0xf8, 0xd1, 0x03, 
    0x20, 0x17, 0x21, 0x89, 0x01, 0xa6, 0x46, 0x1d, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0xad, 0x7f, 
    0xa6, 0x46, 0x1a, 0x48, 0xfe, 0x44, 0x00, 0x47, 0xa8, 0x42, 0xf9, 0xd1, 0xfe, 0xe7, 0x10, 
    0xb5, 0x19, 0x49, 0x08, 0x1f, 0x19, 0x4a, 0x10, 0x60, 0x1c, 0x48, 0x02, 0x1c, 0x71, 0x3a, 
    0x93, 0x24, 0x01, 0x23, 0xa3, 0x54, 0x16, 0x4b, 0x0b, 0x60, 0x02, 0x23, 0x13, 0x71, 0x15, 
    0x4b, 0x4b, 0x60, 0x03, 0x23, 0x53, 0x71, 0x14, 0x4b, 0x8b, 0x60, 0x04, 0x23, 0x03, 0x70, 
    0x14, 0x4b, 0xcb, 0x60, 0x05, 0x23, 0x83, 0x73, 0x13, 0x4b, 0x0b, 0x61, 0x06, 0x23, 0x03, 
    0x73, 0x12, 0x4b, 0x4b, 0x61, 0x07, 0x23, 0x43, 0x71, 0x11, 0x4b, 0x8b, 0x61, 0x08, 0x23, 
    0x33, 0x38, 0x03, 0x70, 0x10, 0x48, 0xc8, 0x61, 0x09, 0x20, 0xd0, 0x74, 0x0f, 0x48, 0x08, 
    0x62, 0x10, 0xbd, 0x25, 0x86, 0x04, 0x00, 0x1b, 0x90, 0x04, 0x00, 0x6c, 0x52, 0x08, 0x00, 
    0x08, 0x66, 0x08, 0x00, 0x20, 0x55, 0x08, 0x00, 0x5b, 0x70, 0x08, 0x00, 0x6f, 0x70, 0x08, 
    0x00, 0x79, 0x70, 0x08, 0x00, 0x95, 0x55, 0x08, 0x00, 0x47, 0x70, 0x08, 0x00, 0x51, 0x70, 
    0x08, 0x00, 0x65, 0x70, 0x08, 0x00, 0x29, 0x70, 0x08, 0x00, 0x33, 0x70, 0x08, 0x00, 0x3d, 
    0x70, 0x08, 0x00, 0xf0, 0xb5, 0x0c, 0x22, 0x22, 0x4e, 0x32, 0x70, 0x1a, 0x23, 0x73, 0x70, 
    0x09, 0x20, 0xb0, 0x70, 0xf2, 0x70, 0x03, 0x20, 0x20, 0x4d, 0x29, 0x1c, 0x01, 0x39, 0x01, 
    0x24, 0xa6, 0x46, 0x1d, 0x4f, 0xfe, 0x44, 0x38, 0x47, 0xb2, 0x78, 0xf3, 0x78, 0x03, 0x20, 
    0x29, 0x1c, 0xa6, 0x46, 

    0x01, 0x05, 0xff, 0x8b, 0xfa, 0x00, 0x18, 0x00, 0x86, 0x19, 0x4e, 0xfe, 0x44, 0x30, 0x47, 
    0x03, 0x20, 0x29, 0x1c, 0x01, 0x31, 0xa6, 0x46, 0x17, 0x4a, 0xfe, 0x44, 0x10, 0x47, 0xa6, 
    0x46, 0x16, 0x48, 0xfe, 0x44, 0x00, 0x47, 0x16, 0x4b, 0x00, 0x21, 0x08, 0x1c, 0x1a, 0x68, 
    0x00, 0x2a, 0x04, 0xd0, 0x02, 0x07, 0x15, 0x0f, 0x22, 0x1c, 0xaa, 0x40, 0x11, 0x43, 0x02, 
    0x07, 0x12, 0x0f, 0x0f, 0x2a, 0x05, 0xd1, 0xc5, 0x08, 0x06, 0x22, 0x2a, 0x40, 0x0e, 0x4d, 
    0xa9, 0x52, 0x00, 0x21, 0x04, 0x33, 0x01, 0x30, 0x20, 0x28, 0xe9, 0xd3, 0x0c, 0x48, 0x01, 
    0x1c, 0x50, 0x31, 0x0c, 0x70, 0x0a, 0x21, 0x09, 0x4a, 0x11, 0x70, 0x2a, 0x21, 0x01, 0x70, 
    0xf0, 0xbd, 0xfc, 0x53, 0x08, 0x00, 0x31, 0x90, 0x04, 0x00, 0xc6, 0x05, 0x00, 0x00, 0x1b, 
    0x90, 0x04, 0x00, 0x33, 0x00, 0x18, 0x00, 0x80, 0x7b, 0x08, 0x00, 0x84, 0xf3, 0x1a, 0x00, 
    0x6d, 0x22, 0x08, 0x00, 0x69, 0x53, 0x08, 0x00, 

    0x01, 0x83, 0xff, 0x14, 0x79, 0x7b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

    // 
    // 
    0x01, 0x0c, 0xfd, 0x09, 0x01, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x64, 0x00, 

    0x01, 0x09, 0xfd, 0x08, 0x58, 0x60, 0x1a, 0x00, 0x00, 0x10, 0x00, 0x10, 

    0x01, 0x09, 0xfd, 0x08, 0x10, 0x60, 0x1a, 0x00, 0x10, 0x00, 0x10, 0x00, 

    0x01, 0x1c, 0xfd, 0x14, 0xff, 0x88, 0x13, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0x00, 0xfa, 0x00, 0xff, 0xff, 0x00, 

    // 
    // 
    // ##--------------------------------------------------------------------------------
    // ## Description:   ORCA_C Commercial PHY FW Initialization Script
    // ##--------------------------------------------------------------------------------
    0x01, 0x76, 0xfd, 0x31, 0x01, 0x21, 0x54, 0x00, 0x00, 0x61, 0x57, 0x00, 0x00, 0x14, 0x05, 
    0x0a, 0x05, 0x00, 0x07, 0x06, 0x0a, 0x04, 0x05, 0x08, 0x09, 0x0b, 0x0c, 0x0d, 0x0e, 0x10, 
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 
    0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 

    // BTstack: added HCI_VS_SET_POWER_VECTOR(GFSK) 0xFD82 template
    0x01, 0x82, 0xfd, 0x14, 0x00, 0x9c, 0x18, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xdc,
    0xe6, 0xf0, 0xfa, 0x04, 0x0e, 0x18, 0xff, 0x00, 0x00,

    // BTstack: added HCI_VS_SET_POWER_VECTOR(EDR2) 0xFD82 template
    0x01, 0x82, 0xfd, 0x14, 0x01, 0x9c, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xd8, 
    0xe2, 0xec, 0xf6, 0x00, 0x0a, 0x14, 0xff, 0x00, 0x00,

    // BTstack: added HCI_VS_SET_POWER_VECTOR(EDR3) 0xFD82 for EDR3 template
    0x01, 0x82, 0xfd, 0x14, 0x02, 0x9c, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xd8,
    0xe2, 0xec, 0xf6, 0x00, 0x0a, 0x14, 0xff, 0x00, 0x00,

    // BTstack: added HCI_VS_SET_CLASS2_SINGLE_POWER 0xFD87 template
    0x01, 0x87, 0xfd, 0x03, 0x0d, 0x0d, 0x0d,

    0x01, 0x80, 0xfd, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 

    0x01, 0x80, 0xfd, 0x06, 0x3c, 0xf0, 0x5f, 0x00, 0x00, 0x00, 

    // 
    // 
    // 
    0x01, 0x38, 0xfe, 0x00, 

    // 
    // #################################################################
    // ## START of CC2564 Adds-On
    // #################################################################
    // 
    // ## Enable fast clock XTAL support
    0x01, 0x1c, 0xfd, 0x14, 0x01, 0x88, 0x13, 0x00, 0x00, 0xd0, 0x07, 0x00, 0x00, 0xff, 0xff, 
    0x04, 0xff, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00, 

    // 
    // ## Enable eHCILL
    0x01, 0x2b, 0xfd, 0x05, 0x10, 0x00, 0x50, 0x00, 0x96, 

    // 
    0x01, 0x0c, 0xfd, 0x09, 0x01, 0x01, 0x00, 0xff, 0xff, 0xff, 0xff, 0x64, 0x00, 

    // 
    // #################################################################
    // ## END of CC2564 Adds-On
    // #################################################################
    0x01, 0x5b, 0xfd, 0x02, 0x01, 0x01, 

    // 
    0x01, 0xdd, 0xfd, 0x01, 0x01, 

};

const uint32_t cc256x_init_script_size = 3825;

