#include <linux/if_ether.h>
#include <netinet/in.h>
#include <stdio.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>

int printFlag = 0;



char *mac_ntoa(unsigned char *d)
{
	static char str[18];
	sprintf(str,"%02x:%02x:%02x:%02x:%02x:%02x",d[0],d[1],d[2],d[3],d[4],d[5]);
	return str;
}

char *ip_ntoa2(unsigned char *d) {
	static char str[15];
	sprintf(str,"%d.%d.%d.%d",d[0],d[1],d[2],d[3]);
	return str;
}

void printEtherHeader(unsigned char *buf) { 
	struct ether_header *eth;
	eth = (struct ether_header *)buf;
	printf("-----ETHERNET-----\n");
	printf("Dst MAC addr : %17s\n",mac_ntoa(eth->ether_dhost));
	printf("Src MAC addr : %17s\n",mac_ntoa(eth->ether_shost));
	printf("Ethernet Type : 0x%04x\n",ntohs(eth->ether_type));
}

char *ip_ntoa(u_int32_t ip){
	unsigned char *d = (unsigned char *)&ip;
	static char str[15];
	sprintf(str,"%d.%d.%d.%d",d[0],d[1],d[2],d[3]);
	return str;
}

void printIPHeader(unsigned char *buf) { 
	struct iphdr *ip;
	ip =  (struct iphdr *)buf;
	printf("----IP-------\n");
	printf("version=%u\n",ip->version);
	printf("ihl=%u\n",ip->ihl);
	printf("tos=%x\n",ip->tos);
	printf("tot_len=%u\n",ip->tot_len);
	printf("id=%u\n",ip->id);
	printf("ttl=%u\n",ip->ttl);
	printf("protocol=%u\n",ip->protocol);
	printf("src addr=%s\n",ip_ntoa(ip->saddr));
	printf("dst addr=%s\n",ip_ntoa(ip->daddr));
}


void printArpHeader(unsigned char *buf) { 
	struct ether_arp *arp;
	arp = (struct ether_arp *)buf;
	printf("------ARP------\n");
	printf("hrd type=%u\n",ntohs(arp->arp_hrd));
	printf("prt type=%u\n",ntohs(arp->arp_pro));
	printf("arp_hln=%u\n",arp->arp_hln);
	printf("arp_pln=%u\n",arp->arp_pln);
	printf("arp_op=%u\n",arp->arp_op);
	printf("arp_sha=%s\n",mac_ntoa(arp->arp_sha));
	printf("arp_spa=%s\n",ip_ntoa2(arp->arp_spa));
	printf("arp_tha=%s\n",mac_ntoa(arp->arp_tha));
	printf("arp_tpa=%s\n",ip_ntoa2(arp->arp_tpa));

}
void printTcpHeader(unsigned char *buf){
	struct tcphdr *ptr;
	ptr = (struct tcphdr *)buf;
	printf("------TCP------\n");
	printf("src port = %u\n",ntohs(ptr->source));
	printf("dst port = %u\n",ntohs(ptr->dest));
}

void printIcmpHeader(unsigned char *buf){
	struct icmp *ptr;
	ptr = (struct icmp *)buf;
	printf("-------icmp-------");
	printf("%u\n",ntohs(ptr->icmp_type));
}


void analyzePacket(unsigned char *buf) { 
	unsigned char *ptr;
	struct ether_header *eth;
	printEtherHeader(buf);
	ptr = buf;
	eth = (struct ether_header*)buf;
	ptr += sizeof(struct ether_header);
	switch(ntohs(eth->ether_type)){ 
		case ETH_P_IP:
			printIPHeader(ptr);
			break;
		case ETH_P_ARP:
			printArpHeader(ptr);
			break;
		case ETH_P_IPV6:
			printf("IPv6 Packet\n");
			break;
		default:
			printf("unknown\n");
	}
	printf("\n");
}

int initRawSocket(char *dev){
	int soc,size;
	struct ifreq ifr;
	struct sockaddr_ll sa;
	soc = socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

	memset(&ifr,0,sizeof(struct ifreq));
	strncpy(ifr.ifr_name,dev,sizeof(ifr.ifr_name)-1);

	ioctl(soc,SIOCGIFINDEX,&ifr);

	sa.sll_family = PF_PACKET;
	sa.sll_protocol = htons(ETH_P_ALL);
	sa.sll_ifindex = ifr.ifr_ifindex;
	bind(soc,(struct sockaddr *)&sa,sizeof(sa));

	ioctl(soc,SIOCGIFFLAGS,&ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(soc,SIOCSIFFLAGS,&ifr);
	return soc;
}




int main() { 
	char *in_dev  ="veth2-rt";
	char *out_dev = "veth3-rt";
	int sock[2];	
	int size;
	sock[0] = initRawSocket(in_dev);
	sock[1] = initRawSocket(out_dev);

	unsigned char buf[65535];
	while(1){
		size = read(sock[0],buf,sizeof(buf));
		analyzePacket(buf);
		write(sock[1],buf,size);
	}

}
