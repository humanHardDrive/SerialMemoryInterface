#define MAX_PACKET_LENGTH	128

#define PACKET_STX  0xAA
#define PACKET_ETX  0x55

enum
{
  WAITING_FOR_STX,
  WAITING_FOR_TYPE,
  WAITING_FOR_LEN_LSB,
  WAITING_FOR_LEN_MSB,
  PROCESS_COMMAND,
  WAITING_FOR_DATA,
  WAITING_FOR_ETX
};

enum
{
  NO_COMMAND = 0,
  VERSION_REQUEST, //From host to hardware
  CACHE_SIZE_REQUEST, //From host to hardware
  MEMORY_REQUEST, //From hardware to host
  MEMORY_WRITE, //From hardware to host
  CLOCK_ENABLE,
  CLOCK_DISABLE,
  ALL_COMMANDS
};

#pragma pack(push, 1)

typedef struct 
{
	uint8_t VersionMinor, VersionMajor;
}VERSION_REQUEST_RSP;

typedef struct
{
	uint8_t WordSize, CacheSize;
}CACHE_REQUEST_RSP;

#pragma pack(pop)
