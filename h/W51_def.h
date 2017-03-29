#ifndef	_w51_def_h_
#define	_w51_def_h_

	#define SERVER_WELCOME_MSG		"Sveiki :)\n"

	#define	W51_SCS_ON()			W51_SCS = 0
	#define	W51_SCS_OFF()			W51_SCS = 1

	#define	OP_CODE_WRITE			0xF0
	#define	OP_CODE_READ			0x0F

	#define SOCKET_COUNT			4
	#define SOCKET_TCP_SERVER		0
	#define SOCKET_UDP_SERVER		1
	#define SOCKET_TCP_SERVER_2		2
	#define RETRY_TIME_MS			300
	#define RETRY_COUNT				6
	#define	BASE_LOCAL_PORT			2000
	#define SOCKET_INIT_DELAY		300		// ms
	#define WAIT_CONNECTION_DELAY	5000	// ms
	#define UNKNOWN_DELAY			5000	// ms

	// Maximum memory size for Tx, Rx in the W5100 is 8K Bytes
	// bit 1-0 : memory size of channel #0
	// bit 3-2 : memory size of channel #1
	// bit 5-4 : memory size of channel #2
	// bit 7-6 : memory size of channel #3
	// 00 - 1k; 01 - 2k; 10 - 4k; 11 - 8k
	#define	TX_BASE_ADDRESS		0x4000
	#define	TX_MEMORY_SHARE		0x55		// 2k for each channel
	#define	TX_BUFFER_MASK		0x07FF
//	#define	TX_BUFFER_MASK		0x03FF
	#define	TX_BUFFER_SIZE		(TX_BUFFER_MASK + 1)
	#define	RX_BASE_ADDRESS		0x6000
	#define	RX_MEMORY_SHARE		0x55		// 2k for each channel
	#define	RX_BUFFER_MASK		0x07FF
	#define	RX_BUFFER_SIZE		(RX_BUFFER_MASK + 1)

	// Common Registers
	#define	MR					0x0000		// Mode Register
	#define	GAR					0x0001		// Gateway IP Address Register
	#define	SUBR				0x0005		// Subnet Mask Register
	#define	SHAR				0x0009		// Source Hardware Address Register (MacAddress)
	#define	SIPR				0x000F		// Source IP Address Register
	#define	IR					0x0015		// Interrupt Register
	#define	IMR					0x0016		// Interrupt Mask Register
	#define	RTR					0x0017		// Retry Time-value Register
	#define	RCR					0x0019		// Retry Count Register
	#define	RMSR				0x001A		// RX Memory Size Register
	#define	TMSR				0x001B		// TX Memory Size Register
	#define	PATR				0x001C		// Authentication Type in PPPoE mode
	#define	PTIMER				0x0028		// PPP Link Control Protocol Request Timer Register
	#define	PMAGIC				0x0029		// PPP Link Control Protocol Magic number Register
	#define	UIPR				0x002A		// Unreachable IP Address Register
	#define	UPORT				0x002E		// Unreachable Port Register

	// Socket Registers
	#define	CH_BASE				0x0400		// brief socket register
	#define CH_SIZE				0x0100		// size of each channel register map

	#define	Sn_MR(ch)			((ch * CH_SIZE) + (CH_BASE + 0x0000))	// Socket n Mode register
	#define	Sn_CR(ch)			((ch * CH_SIZE) + (CH_BASE + 0x0001))	// Socket n Comand register
	#define	Sn_IR(ch)			((ch * CH_SIZE) + (CH_BASE + 0x0002))	// Socket n Interrupt register
	#define	Sn_SR(ch)			((ch * CH_SIZE) + (CH_BASE + 0x0003))	// Socket n Status Register
	#define	Sn_PORT(ch)			((ch * CH_SIZE) + (CH_BASE + 0x0004))
	#define	Sn_DHAR(ch)			((ch * CH_SIZE) + (CH_BASE + 0x0006))
	#define	Sn_DIPR(ch)			((ch * CH_SIZE) + (CH_BASE + 0x000C))
	#define	Sn_DPORT(ch)		((ch * CH_SIZE) + (CH_BASE + 0x0010))
	#define	Sn_MSS(ch)			((ch * CH_SIZE) + (CH_BASE + 0x0012))
	#define	Sn_PROTO(ch)		((ch * CH_SIZE) + (CH_BASE + 0x0014))
	#define	Sn_TOS(ch)			((ch * CH_SIZE) + (CH_BASE + 0x0015))
	#define	Sn_TTL(ch)			((ch * CH_SIZE) + (CH_BASE + 0x0016))
	#define	Sn_TX_FSR(ch)		((ch * CH_SIZE) + (CH_BASE + 0x0020))
	#define	Sn_TX_RR(ch)		((ch * CH_SIZE) + (CH_BASE + 0x0022))
	#define	Sn_TX_WR(ch)		((ch * CH_SIZE) + (CH_BASE + 0x0024))
	#define	Sn_RX_RSR(ch)		((ch * CH_SIZE) + (CH_BASE + 0x0026))
	#define	Sn_RX_RD(ch)		((ch * CH_SIZE) + (CH_BASE + 0x0028))

	// MR (Common Register) Setting bits (can be or-ed)
	#define MODE_RESET			0x80	// internal register will be initialized (automatically cleared after reset)

	// Sn_MR (Socket n Mode Register) Setting bits (can be or-ed)
	#define S_MODE_CLOSE		0x00	// unused socket
	#define S_MODE_TCP			0x01	// TCP
	#define S_MODE_UDP			0x02	// UDP
	#define S_MODE_IPRAW		0x03	// IP LAYER RAW SOCK
	#define S_MODE_MACRAW		0x04	// MAC LAYER RAW SOCK
	#define S_MODE_PPPOE		0x05	// PPPoE
	#define S_MODE_ND			0x20	// No Delayed Ack(TCP) flag
	#define S_MODE_MULTI		0x80	// support multicating

	// Sn_CR (Socket n Command Register) Commands
	#define	CMD_OPEN			0x01	// initialize or open socket
	#define	CMD_LISTEN			0x02	// wait connection request in tcp mode(Server mode)
	#define	CMD_CONNECT			0x04	// send connection request in tcp mode(Client mode)
	#define	CMD_DISCON			0x08	// send closing reqeuset in tcp mode
	#define	CMD_CLOSE			0x10	// close socket
	#define	CMD_SEND			0x20	// updata txbuf pointer, send data
	#define	CMD_SEND_MAC		0x21	// send data with MAC address, so without ARP process
	#define	CMD_SEND_KEEP		0x22	// send keep alive message
	#define	CMD_RECV			0x40	// update rxbuf pointer, recv data

	// Sn_IR (Socket n Interrupt Register) notify bits (must be cleared by writing ‘1’)
	#define S_IR_CON			0x01	// established connection
	#define S_IR_DISCON			0x02	// closed socket
	#define S_IR_RECV			0x04	// receiving data
	#define S_IR_TIMEOUT		0x08	// assert timeout
	#define S_IR_SEND_OK		0x10	// complete sending

	// Sn_SR (Socket n Status Register) Status Values
	#define S_STATUS_CLOSED		0x00	// closed
	#define S_STATUS_UNKNOWN	0x01	// not defined in W5100 documentation status...
	#define SOCK_ARP_1			0x11
	#define SOCK_ARP_2			0x21
	#define SOCK_ARP_3			0x31
	#define S_STATUS_INIT 		0x13	// init state
	#define S_STATUS_LISTEN		0x14	// listen state
	#define S_STATUS_SYNSENT	0x15	// connection state
	#define S_STATUS_SYNRECV	0x16	// connection state
	#define S_STATUS_ESTABLISHED 0x17	// success to connect
	#define S_STATUS_FIN_WAIT	0x18	// closing state
	#define S_STATUS_CLOSING	0x1A	// closing state
	#define S_STATUS_TIME_WAIT	0x1B	// closing state
	#define S_STATUS_CLOSE_WAIT	0x1C	// closing state
	#define S_STATUS_LAST_ACK	0x1D	// closing state
	#define S_STATUS_UDP		0x22	// udp socket
	#define S_STATUS_IPRAW		0x32	// ip raw mode socket
	#define S_STATUS_MACRAW		0x42	// mac raw mode socket
	#define S_STATUS_PPPOE		0x5F	// pppoe socket

enum COMMAND_STATE
{
	CMD_STATE_IDLE = 0,
	CMD_STATE_PENDING,
	CMD_STATE_EXECUTING,
	CMD_STATE_RETRY,
	CMD_STATE_RETRY_DELAY,
	CMD_STATE_ERROR
};

typedef enum
{
	SOCKET_NOT_ALLOCATED = 0,	// 0
	SOCKET_CLOSED,				// 1
	SOCKET_INIT,				// 2
	SOCKET_MUST_CONNECT,		// 3
	SOCKET_WAIT_CONNECTION,		// 4
	SOCKET_CONNECTED,			// 5
	SOCKET_LISTEN,				// 6
	SOCKET_DATA_RECEIVED,		// 7
	SOCKET_CLOSING,				// 8
	SOCKET_ABORTING,			// 9
	SOCKET_ERROR,				// 10
	SOCKET_REPORT_ERROR,		// 11
	SOCKET_FATAL_ERROR			// 12
} SOCKET_STATE;

typedef union{
	U32	u32;
	U8	u8[4];
} IP;

typedef struct {
	IP				ip;
	U16				port;
	U16				delayStartMs;
	U16				activityTimeoutMs;
	U16				dataInReceiveBuffer;
	U8				autoReconnect;
	U8				protocol;
	U8				retryCount;
	SOCKET_STATE	state;
	U8				status;
} W5100_SOCKET;

typedef	struct	{
	IP				ip;
	U16				port;
	U16				len;
} UDP_HEADER;
/*
typedef struct {
	SOCKET_STATE state;
	uint16 dataInReceiveBuffer;
	uint16 retryCount;
	uint32 delayStartTicks;
	uint32 activityTimeoutMs;
	uint32 ip;
	uint16 port;
	bool autoReconnect;
	bool allocated;
} W5100_CONTROL;
*/
#endif  // _w51_def_h_

