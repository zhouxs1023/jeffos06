#ifndef __NE2000_INTERFACE
#define __NE2000_INTERFACE
#ifndef __NE2000_SHARED_CODE_BASE
typedef unsigned int uint;
typedef struct snic snic;
typedef struct nic_stat nic_stat;
typedef struct buffer_header buffer_header;
typedef struct packet_data packet_data;
typedef struct packet_buffer packet_buffer;
typedef struct nic_error_stat nic_error_stat;
#endif /* NE2000_SHARED_CODE_BASE */

#define LEN_ADDR		6
#define MAX_TX			2	/* be careful with this (dual buffers) */
#define MAX_PAGESPERPACKET	6
#define TXPAGES			(MAX_TX*MAX_PAGESPERPACKET)
#define LEN_PROM		16
#define LEN_HEADER		14
#define MIN_LENGTH              60      /* minimum length for packet data */
#define MAX_LENGTH              1500    /* maximum length for packet data area */

#ifdef __CPP_BINDING
extern "C" {
#endif

/* external interface */
int nic_detect(int given);
int nic_init(snic* nic, int addr, unsigned char *prom, unsigned char *manual);
void nic_register_notify(snic *nic, void(*newnotify)(void*,packet_data*),void*);
void nic_start(snic *nic, int promiscuous);
void nic_stop(snic *nic);
void nic_isr(snic *nic);
nic_stat nic_get_stats(snic *nic);
void nic_stat_clear(nic_stat *that);
int nic_send_packet(snic *nic, packet_buffer *buffer);

/* Your implementation */
/*packet_buffer *alloc_buffer(uint count); */     /* Optional */
packet_data *alloc_buffer_data(uint size);
void free_buffer(packet_buffer *ptr);
void cleanup_buffer(packet_buffer *ptr);
void free_buffer_data(packet_data *ptr); 	/* Optional */
/* I reserve the right to use the "Option" procedures in the future */

#ifdef __CPP_BINDING
};
#endif

struct buffer_header {
	unsigned char status;
	unsigned char next;
	unsigned short count;	/* length of header and packet */
};

struct nic_error_stat {
	long frame_alignment, crc, missed_packets;
	long rx, rx_size, rx_dropped, rx_fifo, rx_overruns;
	long tx_collisions;
	long tx_aborts, tx_carrier, tx_fifo, tx_heartbeat, tx_window;
};

struct nic_stat {
	long rx_packets;
	long tx_buffered, tx_packets;
	nic_error_stat errors;
};

struct packet_buffer {
	uint page;
	uint count, len;
	packet_data *buf;	/* An array of data segments */
};

struct packet_data {		/* each protocol layer can add it's own */
	uint len;
	char *ptr;
};

struct snic {
	int iobase;	/* NULL if uninitialized */
	int pstart, pstop, wordlength, current_page;
	nic_stat stat;
	void (*notify)(void *passback, packet_data *newpacket);
	void *kore;	/* Passback pointer */
	packet_buffer *tx_packet[MAX_TX], *last_tx;
	int busy, send, sending;
};

#endif	/* __NE2000_INTERFACE */
