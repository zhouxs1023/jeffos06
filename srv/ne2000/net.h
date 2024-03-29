#pragma pack(2)

//typedef unsigned char uint8;
//typedef unsigned short uint16;
//typedef unsigned int uint32;

typedef struct 
{
    uint8  dst[6];
    uint8  src[6];    
    uint16 type;    
} net_ether;

typedef struct 
{
    uint8  ip_hdr_len:4;
    uint8  ip_version:4;
    uint8  ip_tos;
    uint16 ip_len;
    
    uint16 ip_id;
    uint16 ip_off;
    
    uint8  ip_ttl;
    uint8  ip_proto;    
    uint16 ip_chk;
    
    uint32 ip_src;
    uint32 ip_dst;        
} net_ip;

typedef struct 
{
    uint16 udp_src;
    uint16 udp_dst;

    uint16 udp_len;
    uint16 udp_chk;    
} net_udp;

typedef struct
{
    uint8  icmp_type;
    uint8  icmp_code;
    uint16 icmp_chk;
    union {
        struct {
            uint16 id;
            uint16 seq;
	    uint8 data[128];
        } ping;
    } data; 
} net_icmp;

#define ICMP_PING_REQ 8
#define ICMP_PING_REP 0

typedef struct
{
    uint16 arp_hard_type;
    uint16 arp_prot_type;    
    uint8  arp_hard_size;
    uint8  arp_prot_size;
    uint16 arp_op;    
    uint8  arp_enet_sender[6];
    uint32 arp_ip_sender;
    uint8  arp_enet_target[6];
    uint32 arp_ip_target;    
} net_arp;

typedef struct 
{
    net_ether ether;
    net_ip ip;
    unsigned char data[0];
} ip_packet;

typedef struct {
    net_ether ether;
    net_ip ip;
    net_udp udp;
    unsigned char data[0];
} udp_packet;

typedef struct {
    net_ether ether;
    net_ip ip;
    net_icmp icmp;
    unsigned char data[0];
} icmp_packet;

#define ARP_OP_REQUEST  1
#define ARP_OP_REPLY    2

/* yeah, ugly as shit */
#define ntohs(n) ( (((n) & 0xFF00) >> 8) | (((n) & 0x00FF) << 8) )
#define htons(n) ( (((n) & 0xFF00) >> 8) | (((n) & 0x00FF) << 8) )
#define ntohl(n) ( (((n) & 0xFF000000) >> 24) | (((n) & 0x00FF0000) >> 8) | (((n) & 0x0000FF00) << 8) | (((n) & 0x000000FF) << 24) )
#define htonl(n) ( (((n) & 0xFF000000) >> 24) | (((n) & 0x00FF0000) >> 8) | (((n) & 0x0000FF00) << 8) | (((n) & 0x000000FF) << 24) )

/*
typedef struct 
{
    uint32 ip;    
    uint16 flags;
    uint8  hw[6];   
} net_arp_table;

typedef struct 
{
    uint32 dest;
    uint32 gw;
    uint32 mask;
    uint16 flags;
    net_iface *iface;    
} net_route_table;


typedef struct 
{
    char name[8];
    uint32 addr;
    uint32 bcast;
    uint32 mask;
    uint16 flags;
    uint16 mtu;
    uint32 metric;
    
    uint32 stat_rx_packets;
    uint32 stat_rx_errors;
    uint32 stat_rx_dropped;
    uint32 stat_rx_overruns;

    uint32 stat_tx_packets;
    uint32 stat_tx_errors;
    uint32 stat_tx_dropped;
    uint32 stat_tx_overruns;

    void (*send)(void);
    void (*init)(void);    
    void (*add_proto)(net_proto *proto, uint16 id);
    
} net_iface;

#define IF_UP         0x0001
#define IF_BROADCAST  0x0002
#define IF_PROMISC    0x0004
#define IF_LOOPBACK   0x0008
#define IF_MULTICAST  0x0010
#define IF_RUNNING    0x0020
*/
