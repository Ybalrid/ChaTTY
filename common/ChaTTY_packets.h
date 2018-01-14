#ifndef CHATTY_PACKETS_DEFINITION
#define CHATTY_PACKETS_DEFINITION

typedef unsigned char byte_t;

#define ChaTTY_PACKET_MAX_SIZE 1024

//Preprocessor based "strcat"
#define ChaTTY_CAT_IMPL(a, b) a##b
#define ChaTTY_CAT(a, b) ChaTTY_CAT_IMPL(a, b)

//Macro to simplify packet definition interactions
#define ChaTTY_PACKET_PREFIX_TYPE struct ChaTTY_Packet_
#define ChaTTY_PACKET_(TYPE) ChaTTY_CAT(ChaTTY_PACKET_PREFIX_TYPE, TYPE)
#define ChaTTY_PACKET_CAST(TYPE) ((ChaTTY_PACKET_(TYPE) *))
#define ChaTTY_PACKET_SERIALIZE(PTR) ((byte_t*) (PTR))
#define ChaTTY_PACKET_SIZE(TYPE) (sizeof(ChaTTY_PACKET_(TYPE)))
#define ChaTTY_PACKET_HEADER byte_t packetType;

//Configure structure packing : align to the byte
#pragma pack(push, 1)

//Define packets here


//How to add a packet type to the chat protocol:
// rules :
//  - each packet type should be defined via the ChaTTY_PACKET_() macro
//  - ecah packet type should start by ChaTTY_PACKET_HEADER alone in it's first line
//  - packet defintions here should only be added between the #pragma pack macros. It may work if you don't do that, but this would be pure luck
//  - any kind of data can be added to the packet defintion. Including structures of variables ONLY if they are defined in this file.
//  - obviously, serializing pointers is not possible, only "stack" data can be defined in there
//  - variable lengh arrays aren't possible either, buffers of data should be declared as type name[lenght] only
//  - a pointer to a packet can be casted to and from a pointer to an array of byte_t
//  - the first byte of the array should be a value defined in the ChaTTY_Packet_def enumeration
//  - as a measure of consistancy, the type name in the enumeration, and the type name in the packet definition below should match
//  - as this program is only intended to be used between linux machines running on PC-like computers, no mechanism for checking bit endianess will be used

///Packet defintions
///Theses values serve to identify the nature of a packet.
///Packets will be serialized as arrays of bytes. Testing the first one will permit to
///know how to interpret the folliwing data.
enum ChaTTY_Packet_def
{
    EMPTY = 0x00,
    NAME_TRANSPORT = 0x01,
    MESSAGE_TRANSPORT = 0x02,
    MAX_COUNT = 0xFF, // No packet identifier should be defined after this value
};


ChaTTY_PACKET_(EMPTY)
{
    ChaTTY_PACKET_HEADER
};


ChaTTY_PACKET_(NAME_TRANSPORT)
{
    ChaTTY_PACKET_HEADER

    byte_t  names[1023];  /* \0 seperated \0\0 ended */
};

ChaTTY_PACKET_(MESSAGE_TRANSPORT)
{
    ChaTTY_PACKET_HEADER

    char  nickname[32];
    char  message[991];
};

//Restore configuration
#pragma pack(pop)
#endif
