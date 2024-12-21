#ifndef PSP_UDP_DPDK_HH_
#define PSP_UDP_DPDK_HH_

#include <psp/libos/io/udp_dpdk/dpdk.hh>
#include <psp/annot.h>
#include <psp/logging.hh>

#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_mbuf_pool_ops.h>
#include <rte_hexdump.h>
#include <rte_prefetch.h>

#include <arpa/inet.h>
#include <math.h>

#include <vector>

// mbufs / mempool
#define NUM_MBUFS 16383
#define MBUF_DATA_SIZE  1024 + RTE_PKTMBUF_HEADROOM
#define MBUF_CACHE_SIZE 512

// TX/RX batch size
#define MAX_TX_BURST          8
#define MAX_RX_BURST          8
#define RX_PREFETCH_STRIDE 2

// app TQ/RQ
#define INBOUND_Q_LEN 128
#define OUTBOUND_Q_LEN 128

// Offloads
//#define OFFLOAD_IP_CKSUM

// Protocol related variables
#define NET_HDR_SIZE 42 // ETH(14) + IP(20) + UDP(8)

#define IP_DEFTTL  64   /** < from RFC 1340. */
#define IP_VERSION 0x40
#define IP_HDRLEN  0x05 /** < default IP header length == five 32-bits words. */
#define IP_VHL_DEF (IP_VERSION | IP_HDRLEN)

//#define NET_DEBUG

class UdpContext {
    private: uint32_t id;

    private: const uint16_t numa_socket_id = 0;
    private: std::vector<rte_flow *> flows;

    public: const in_addr ip;
    public: uint16_t port;
    public: uint16_t port_id; /** < NIC device id */
    private: static constexpr rte_ether_addr ether_broadcast = {
        .addr_bytes = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
    };
    private: rte_ether_addr my_mac;

    private: struct rte_mempool *mbuf_pool;
    private: int init_mempool(struct rte_mempool **mempool_out,
                              const uint16_t numa_socket_id,
                              const char *name);
    public: int set_fdir();

    public: int get_mbuf(void **mbuf_out, char **payload_addr);
    public: int free_mbuf(unsigned long mbuf);

    public: int poll();
    public: int parse_packet(struct rte_mbuf *);
    public: int prepare_outbound_packet(unsigned long mbuf, struct rte_mbuf **pkt_out);
    public: int send_packets();
    public: int recv_packets();
    private: static int ip_sum(uint16_t &sum_out, const uint16_t *hdr, int hdr_len);

    public: unsigned long inbound_queue[INBOUND_Q_LEN];
    public: uint32_t pop_tail = 0;
    public: uint32_t pop_head = 0;
    public: unsigned long outbound_queue[OUTBOUND_Q_LEN];
    public: uint32_t push_tail = 0;
    public: uint32_t push_head = 0;

    public:
        UdpContext(uint32_t id,
                   struct in_addr ip, uint16_t port, uint16_t port_id,
                   struct rte_mempool **mempool,
                   std::string &mac):
            id(id), ip(ip), port(port), port_id(port_id) {
            int ret;
            // Init mempool if 0x0
            if (*mempool == NULL) {
                ret = init_mempool(mempool, numa_socket_id, "mbuf_mempool");
                if (ret != 0) {
                    PSP_PANIC("Could not initialize mbuf mempool");
                }
            }
            mbuf_pool = *mempool;
            rte_eth_macaddr_get(0, &my_mac);
            //rte_ether_unformat_addr(mac.c_str(), &my_mac);
            //rte_eth_dev_mac_addr_add(port_id, my_mac);
        }
};
#endif //PSP_UDP_DPDK_HH_
