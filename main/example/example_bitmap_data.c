#include <rndl/color.h>
#include <rndl/graphics/bitmap.h>

// https://lvgl.io/tools/imageconverter
static const uint8_t bitmap_data[] = {
    0x4,  0xfc, 0xbc, 0x9,  0xf8, 0xb8, 0xf,  0xf4, 0xb3, 0x14, 0xf0, 0xb0, 0x1a, 0xec, 0xab, 0x1f, 0xe9, 0xa7, 0x24,
    0xe5, 0xa4, 0x2a, 0xe1, 0x9f, 0x2f, 0xde, 0x9b, 0x34, 0xda, 0x98, 0x3a, 0xd6, 0x93, 0x3f, 0xd2, 0x8f, 0x45, 0xce,
    0x8b, 0x4a, 0xcb, 0x87, 0x4f, 0xc7, 0x83, 0x55, 0xc3, 0x7f, 0x5a, 0xbf, 0x7b, 0x60, 0xbb, 0x77, 0x65, 0xb8, 0x73,
    0x6a, 0xb4, 0x6f, 0x70, 0xb0, 0x6b, 0x75, 0xac, 0x67, 0x7a, 0xa9, 0x63, 0x80, 0xa5, 0x5f, 0x85, 0xa1, 0x5b, 0x8b,
    0x9d, 0x56, 0x90, 0x99, 0x53, 0x95, 0x96, 0x4f, 0x9b, 0x92, 0x4a, 0xa0, 0x8e, 0x47, 0xa5, 0x8b, 0x43, 0xab, 0x86,
    0x3e, 0x6,  0xfa, 0xba, 0xc,  0xf6, 0xb6, 0x11, 0xf3, 0xb2, 0x17, 0xee, 0xad, 0x1c, 0xeb, 0xaa, 0x21, 0xe7, 0xa6,
    0x27, 0xe3, 0xa1, 0x2c, 0xe0, 0x9e, 0x32, 0xdb, 0x99, 0x37, 0xd8, 0x95, 0x3c, 0xd4, 0x92, 0x42, 0xd0, 0x8d, 0x47,
    0xcd, 0x89, 0x4c, 0xc9, 0x86, 0x52, 0xc5, 0x81, 0x57, 0xc1, 0x7d, 0x5d, 0xbd, 0x79, 0x62, 0xba, 0x75, 0x67, 0xb6,
    0x71, 0x6d, 0xb2, 0x6d, 0x72, 0xae, 0x69, 0x77, 0xab, 0x65, 0x7d, 0xa7, 0x61, 0x82, 0xa3, 0x5d, 0x88, 0x9f, 0x59,
    0x8d, 0x9c, 0x55, 0x92, 0x98, 0x51, 0x98, 0x94, 0x4d, 0x9d, 0x90, 0x49, 0xa3, 0x8c, 0x44, 0xa8, 0x89, 0x41, 0xad,
    0x85, 0x3d, 0x9,  0xf8, 0xb8, 0xe,  0xf5, 0xb4, 0x14, 0xf0, 0xb0, 0x19, 0xed, 0xac, 0x1e, 0xe9, 0xa8, 0x24, 0xe5,
    0xa4, 0x29, 0xe2, 0xa0, 0x2f, 0xde, 0x9b, 0x34, 0xda, 0x98, 0x39, 0xd6, 0x94, 0x3f, 0xd2, 0x8f, 0x44, 0xcf, 0x8c,
    0x4a, 0xcb, 0x87, 0x4f, 0xc7, 0x83, 0x54, 0xc4, 0x80, 0x5a, 0xbf, 0x7b, 0x5f, 0xbc, 0x77, 0x64, 0xb8, 0x74, 0x6a,
    0xb4, 0x6f, 0x6f, 0xb1, 0x6b, 0x75, 0xac, 0x67, 0x7a, 0xa9, 0x63, 0x7f, 0xa5, 0x5f, 0x85, 0xa1, 0x5b, 0x8a, 0x9e,
    0x57, 0x8f, 0x9a, 0x53, 0x95, 0x96, 0x4f, 0x9a, 0x92, 0x4b, 0xa0, 0x8e, 0x47, 0xa5, 0x8b, 0x43, 0xaa, 0x87, 0x3f,
    0xb0, 0x83, 0x3b, 0xb,  0xf7, 0xb6, 0x11, 0xf3, 0xb2, 0x16, 0xef, 0xae, 0x1c, 0xeb, 0xaa, 0x21, 0xe7, 0xa6, 0x26,
    0xe4, 0xa2, 0x2c, 0xe0, 0x9e, 0x31, 0xdc, 0x9a, 0x36, 0xd9, 0x96, 0x3c, 0xd4, 0x92, 0x41, 0xd1, 0x8e, 0x47, 0xcd,
    0x89, 0x4c, 0xc9, 0x86, 0x51, 0xc6, 0x82, 0x57, 0xc1, 0x7d, 0x5c, 0xbe, 0x7a, 0x61, 0xba, 0x76, 0x67, 0xb6, 0x71,
    0x6c, 0xb3, 0x6e, 0x72, 0xae, 0x69, 0x77, 0xab, 0x65, 0x7c, 0xa7, 0x62, 0x82, 0xa3, 0x5d, 0x87, 0xa0, 0x59, 0x8d,
    0x9c, 0x55, 0x92, 0x98, 0x51, 0x97, 0x95, 0x4d, 0x9d, 0x90, 0x49, 0xa2, 0x8d, 0x45, 0xa7, 0x89, 0x41, 0xad, 0x85,
    0x3d, 0xb2, 0x82, 0x39, 0xe,  0xf5, 0xb4, 0x13, 0xf1, 0xb0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x34, 0xda, 0x98, 0x39, 0xd6, 0x94, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x49,
    0xcb, 0x88, 0x4e, 0xc8, 0x84, 0x54, 0xc4, 0x80, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x64, 0xb8, 0x74, 0x69, 0xb5,
    0x70, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x8a, 0x9e, 0x57,
    0x8f, 0x9a, 0x53, 0x94, 0x97, 0x50, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0xa5, 0x8b, 0x43, 0xaa, 0x87, 0x3f, 0xaf,
    0x84, 0x3b, 0xb5, 0x7f, 0x37, 0x10, 0xf3, 0xb3, 0x16, 0xef, 0xae, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x26, 0xe4,
    0xa2, 0x2b, 0xe0, 0x9e, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x3b, 0xd5, 0x92, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x51, 0xc6, 0x82, 0x56, 0xc2, 0x7e, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x66, 0xb7, 0x72, 0x6c,
    0xb3, 0x6e, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x7c, 0xa7, 0x62, 0x81, 0xa4, 0x5e, 0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x91, 0x99, 0x52, 0x97, 0x95, 0x4d, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0xa7, 0x89, 0x41, 0xac, 0x86, 0x3e,
    0xb2, 0x82, 0x39, 0xb7, 0x7e, 0x35, 0x13, 0xf1, 0xb0, 0x18, 0xee, 0xad, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x28,
    0xe2, 0xa1, 0x2e, 0xde, 0x9c, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x3e, 0xd3, 0x90, 0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x53, 0xc4, 0x80, 0x59, 0xc0, 0x7c, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x69, 0xb5, 0x70,
    0x6e, 0xb1, 0x6c, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x7e, 0xa6, 0x60, 0x84, 0xa2, 0x5c, 0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x94, 0x97, 0x50, 0x99, 0x93, 0x4c, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0xa9, 0x88, 0x40, 0xaf, 0x84,
    0x3b, 0xb4, 0x80, 0x38, 0xba, 0x7c, 0x33, 0x15, 0xf0, 0xaf, 0x1b, 0xec, 0xaa, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x3b, 0xd5, 0x92, 0x40, 0xd2, 0x8f, 0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x50, 0xc6, 0x83, 0x0,  0x0,  0x0,  0x5b, 0xbf, 0x7a, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x6b, 0xb3,
    0x6e, 0x71, 0xaf, 0x6a, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x81, 0xa4, 0x5e, 0x86, 0xa0, 0x5a, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x96, 0x95, 0x4e, 0x9c, 0x91, 0x4a, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0xac, 0x86, 0x3e, 0xb1,
    0x82, 0x3a, 0xb7, 0x7e, 0x35, 0xbc, 0x7b, 0x32, 0x18, 0xee, 0xad, 0x1d, 0xea, 0xa9, 0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x2d, 0xdf, 0x9d, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x3d, 0xd4, 0x91, 0x43, 0xcf, 0x8c, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x53, 0xc4, 0x80, 0x0,  0x0,  0x0,  0x5e, 0xbd, 0x78, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x6e,
    0xb1, 0x6c, 0x73, 0xae, 0x68, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x83, 0xa3, 0x5c, 0x89, 0x9e, 0x58, 0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x99, 0x93, 0x4c, 0x9e, 0x90, 0x48, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0xae, 0x84, 0x3c,
    0xb4, 0x80, 0x38, 0xb9, 0x7d, 0x34, 0xbe, 0x79, 0x30, 0x1a, 0xec, 0xab, 0x1f, 0xe9, 0xa7, 0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x30, 0xdd, 0x9b, 0x35, 0xd9, 0x97, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x45, 0xce, 0x8b, 0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x55, 0xc3, 0x7f, 0x5b, 0xbf, 0x7a, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x70, 0xb0, 0x6b, 0x76, 0xac, 0x66, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x86, 0xa0, 0x5a, 0x8b, 0x9d, 0x56, 0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x9b, 0x92, 0x4a, 0xa1, 0x8d, 0x46, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0xb1, 0x82,
    0x3a, 0xb6, 0x7f, 0x36, 0xbc, 0x7b, 0x32, 0xc1, 0x77, 0x2e, 0x1d, 0xea, 0xa9, 0x22, 0xe7, 0xa5, 0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x32, 0xdb, 0x99, 0x37, 0xd8, 0x95, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x48, 0xcc, 0x89, 0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x58, 0xc1, 0x7d, 0x5d, 0xbd, 0x79, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x73, 0xae, 0x68, 0x78, 0xaa, 0x65, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x88, 0x9f, 0x59, 0x8e, 0x9b, 0x54,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x9e, 0x90, 0x48, 0xa3, 0x8c, 0x44, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0xb3,
    0x81, 0x38, 0xb9, 0x7d, 0x34, 0xbe, 0x79, 0x30, 0xc3, 0x76, 0x2c, 0x1f, 0xe9, 0xa7, 0x24, 0xe5, 0xa4, 0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x35, 0xd9, 0x97, 0x3a, 0xd6, 0x93, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x4a, 0xcb, 0x87,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x5a, 0xbf, 0x7b, 0x60, 0xbb, 0x77, 0x65, 0xb8, 0x73, 0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x75, 0xac, 0x67, 0x7a, 0xa9, 0x63, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x9b, 0x92, 0x4a, 0xa0, 0x8e, 0x47, 0xa6, 0x8a, 0x42, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0xc6, 0x74, 0x2a, 0x21, 0xe7, 0xa6, 0x27, 0xe3, 0xa1, 0x2c,
    0xe0, 0x9e, 0x32, 0xdb, 0x99, 0x37, 0xd8, 0x95, 0x3c, 0xd4, 0x92, 0x42, 0xd0, 0x8d, 0x47, 0xcd, 0x89, 0x4d, 0xc8,
    0x85, 0x52, 0xc5, 0x81, 0x57, 0xc1, 0x7d, 0x5d, 0xbd, 0x79, 0x62, 0xba, 0x75, 0x67, 0xb6, 0x71, 0x6d, 0xb2, 0x6d,
    0x72, 0xae, 0x69, 0x78, 0xaa, 0x65, 0x7d, 0xa7, 0x61, 0x82, 0xa3, 0x5d, 0x88, 0x9f, 0x59, 0x8d, 0x9c, 0x55, 0x92,
    0x98, 0x51, 0x98, 0x94, 0x4d, 0x9d, 0x90, 0x49, 0xa3, 0x8c, 0x44, 0xa8, 0x89, 0x41, 0xad, 0x85, 0x3d, 0xb3, 0x81,
    0x38, 0xb8, 0x7d, 0x35, 0xbd, 0x7a, 0x31, 0xc3, 0x76, 0x2c, 0xc8, 0x72, 0x29, 0x24, 0xe5, 0xa4, 0x29, 0xe2, 0xa0,
    0x2f, 0xde, 0x9b, 0x34, 0xda, 0x98, 0x39, 0xd6, 0x94, 0x3f, 0xd2, 0x8f, 0x44, 0xcf, 0x8c, 0x4a, 0xcb, 0x87, 0x4f,
    0xc7, 0x83, 0x54, 0xc4, 0x80, 0x5a, 0xbf, 0x7b, 0x5f, 0xbc, 0x77, 0x64, 0xb8, 0x74, 0x6a, 0xb4, 0x6f, 0x6f, 0xb1,
    0x6b, 0x75, 0xac, 0x67, 0x7a, 0xa9, 0x63, 0x7f, 0xa5, 0x5f, 0x85, 0xa1, 0x5b, 0x8a, 0x9e, 0x57, 0x90, 0x99, 0x53,
    0x95, 0x96, 0x4f, 0x9a, 0x92, 0x4b, 0xa0, 0x8e, 0x47, 0xa5, 0x8b, 0x43, 0xaa, 0x87, 0x3f, 0xb0, 0x83, 0x3b, 0xb5,
    0x7f, 0x37, 0xbb, 0x7b, 0x32, 0xc0, 0x78, 0x2f, 0xc5, 0x74, 0x2b, 0xcb, 0x70, 0x26, 0x26, 0xe4, 0xa2, 0x2c, 0xe0,
    0x9e, 0x31, 0xdc, 0x9a, 0x36, 0xd9, 0x96, 0x3c, 0xd4, 0x92, 0x41, 0xd1, 0x8e, 0x47, 0xcd, 0x89, 0x4c, 0xc9, 0x86,
    0x51, 0xc6, 0x82, 0x57, 0xc1, 0x7d, 0x5c, 0xbe, 0x7a, 0x62, 0xba, 0x75, 0x67, 0xb6, 0x71, 0x6c, 0xb3, 0x6e, 0x72,
    0xae, 0x69, 0x77, 0xab, 0x65, 0x7c, 0xa7, 0x62, 0x82, 0xa3, 0x5d, 0x87, 0xa0, 0x59, 0x8d, 0x9c, 0x55, 0x92, 0x98,
    0x51, 0x97, 0x95, 0x4d, 0x9d, 0x90, 0x49, 0xa2, 0x8d, 0x45, 0xa7, 0x89, 0x41, 0xad, 0x85, 0x3d, 0xb2, 0x82, 0x39,
    0xb8, 0x7d, 0x35, 0xbd, 0x7a, 0x31, 0xc2, 0x76, 0x2d, 0xc8, 0x72, 0x29, 0xcd, 0x6f, 0x25, 0x29, 0xe2, 0xa0, 0x2e,
    0xde, 0x9c, 0x34, 0xda, 0x98, 0x39, 0xd6, 0x94, 0x3e, 0xd3, 0x90, 0x44, 0xcf, 0x8c, 0x49, 0xcb, 0x88, 0x4e, 0xc8,
    0x84, 0x54, 0xc4, 0x80, 0x59, 0xc0, 0x7c, 0x5f, 0xbc, 0x77, 0x64, 0xb8, 0x74, 0x69, 0xb5, 0x70, 0x6f, 0xb1, 0x6b,
    0x74, 0xad, 0x68, 0x7a, 0xa9, 0x63, 0x7f, 0xa5, 0x5f, 0x84, 0xa2, 0x5c, 0x8a, 0x9e, 0x57, 0x8f, 0x9a, 0x53, 0x94,
    0x97, 0x50, 0x9a, 0x92, 0x4b, 0x9f, 0x8f, 0x47, 0xa5, 0x8b, 0x43, 0xaa, 0x87, 0x3f, 0xaf, 0x84, 0x3b, 0xb5, 0x7f,
    0x37, 0xba, 0x7c, 0x33, 0xbf, 0x78, 0x2f, 0xc5, 0x74, 0x2b, 0xca, 0x71, 0x27, 0xd0, 0x6c, 0x23};

const rndl_bitmap24_t bitmap_rndl = {.width = 32, .height = 16, .pixels = (rndl_color24_t *)bitmap_data};
