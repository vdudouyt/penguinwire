#define CONTROL_CMD			0x00
#define COMM_CMD			0x01
#define MODE_CMD			0x02

#define COMM_ERROR_ESCAPE		0x0601
#define COMM_SET_DURATION		0x0012
#define COMM_BIT_IO			0x0020
#define COMM_PULSE			0x0030
#define COMM_1_WIRE_RESET		0x0042
#define COMM_BYTE_IO			0x0052
#define COMM_MATCH_ACCESS		0x0064
#define COMM_BLOCK_IO			0x0074
#define COMM_READ_STRAIGHT		0x0080
#define COMM_DO_RELEASE			0x6092
#define COMM_SET_PATH			0x00A2
#define COMM_WRITE_SRAM_PAGE		0x00B2
#define COMM_WRITE_EPROM		0x00C4
#define COMM_READ_CRC_PROT_PAGE		0x00D4
#define COMM_READ_REDIRECT_PAGE_CRC	0x21E4
#define COMM_SEARCH_ACCESS		0x00F4

/* Status flags */
#define ST_SPUA				0x01  /* Strong Pull-up is active */
#define ST_PRGA				0x02  /* 12V programming pulse is being generated */
#define ST_12VP				0x04  /* external 12V programming voltage is present */
#define ST_PMOD				0x08  /* DS2490 powered from USB and external sources */
#define ST_HALT				0x10  /* DS2490 is currently halted */
#define ST_IDLE				0x20  /* DS2490 is currently idle */
#define ST_EPOF				0x80
/* Status transfer size, 16 bytes status, 16 byte result flags */
#define ST_SIZE				0x20
