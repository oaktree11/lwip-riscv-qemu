#include <stdarg.h>
#define uint64 unsigned long
#define uint32 unsigned int
#define uint16 unsigned short
#define uint8 unsigned char
#include "e1000_dev.h"
#define ETHERNET_PACKET_SIZE  1518 
//#define printf uprintf
#define TX_RING_SIZE 16
#define RX_RING_SIZE 128
#define RX_PKT_BUFF_SIZE 2048

static struct tx_desc tx_ring[TX_RING_SIZE] __attribute__((aligned(16)));
//static char *tx_mbufs[TX_RING_SIZE];
static char tx_buffs[TX_RING_SIZE][RX_PKT_BUFF_SIZE];

#define RX_RING_SIZE 128
#define RX_PKT_BUFF_SIZE 2048
static char rx_buffs[RX_RING_SIZE][RX_PKT_BUFF_SIZE];
static struct rx_desc rx_ring[RX_RING_SIZE] __attribute__((aligned(16)));
//static char *rx_buffs[RX_RING_SIZE];
panic(char * p){
putstr(p);
}
consputc(char ch){
ns16550a_putchar(ch);
}
uputc(char ch){
ns16550a_putchar(ch);
}

void *memcpy(void *dest, const void *src, int n)
{
    char *p = dest;
    while (n-- > 0) {
        *(char*)dest++ = *(char*)src++;
    }
    return p;
}


static char digits[] = "0123456789abcdef";

static void
printint(int xx, int base, int sign)
{
  char buf[16];
  int i;
  unsigned int x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  int i;
  consputc('0');
  consputc('x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the console. only understands %d, %x, %p, %s.
void
printf(char *fmt, ...)
{
  va_list ap;
  int i, c;
  char *s;

  

  if (fmt == 0)
    panic("null fmt");

  va_start(ap, fmt);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(va_arg(ap, int), 10, 1);
      break;
    case 'x':
      printint(va_arg(ap, int), 16, 1);
      break;
    case 'p':
      printptr(va_arg(ap, uint64));
      break;
    case 's':
      if((s = va_arg(ap, char*)) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  
}

#ifdef fff
char *tab = "0123456789ABCDEF";

int uputs(char *s)
{
  while(*s){
    uputc(*s++);
    if (*s=='\n')
      uputc('\r');
  }
   
 }
int urpx(int x)
{
  char c;
  if (x){
     c = tab[x % 16];
     urpx(x / 16);
  }
  uputc(c);
}

int uprints( char *s)
{
  while(*s)
    uputc(*s++);
}

int uprintx(int x)
{
  uprints("0x");
  if (x==0)
    uputc('0');
  else
    urpx(x);
  uputc(' ');
}

int urpu( int x)
{
  char c;
  if (x){
     c = tab[x % 10];
     urpu(x / 10);
  }
  uputc(c);
}

int uprintu( int x)
{
  if (x==0)
    uputc('0');
  else
    urpu(x);
  uputc(' ');
}

int uprinti(int x)
{
  if (x<0){
    uputc('-');
    x = -x;
  }
  uprintu(x);
}

int ufprintf(char *fmt,...)
{
  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt + 1;
va_list ap;
  while(*cp){
    if (*cp != '%'){
      uputc(*cp);
      if (*cp=='\n')
	uputc('\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c': uputc((char)*ip);      break;
    case 's': uprints((char *)*ip);  break;
    case 'd': uprinti(*ip);           break;
    case 'u': uprintu(*ip);           break;
    case 'x': uprintx(*ip);  break;
    }
    cp++; ip++;
  }
}

int uprintf(char *fmt, ...)
{
  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt + 1;

    while(*cp){
    if (*cp != '%'){
      uputc(*cp);
      if (*cp=='\n')
	uputc('\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c': uputc((char)*ip);      break;
    case 's': uprints((char *)*ip);   break;
    case 'd': uprinti(*ip);           break;
    case 'u': uprintu(*ip);           break;
    case 'x': uprintx(*ip);  break;
    }
    cp++; ip++;
  }
}
#endif
// remember where the e1000's registers live.
static volatile uint32 *regs;

//struct spinlock e1000_lock;
//struct spinlock e1000_lock_recv;

// called by pci_init().
// xregs is the memory address at which the
// e1000's registers are mapped.
void
e1000_init(uint32 *xregs)
{
  int i;

  
  regs = xregs;

  // Reset the device
  regs[E1000_IMS] = 0; // disable interrupts
  regs[E1000_CTL] |= E1000_CTL_RST;
  regs[E1000_IMS] = 0; // redisable interrupts
  __sync_synchronize();

  // [E1000 14.5] Transmit initialization
  //memset(tx_ring, 0, sizeof(tx_ring));
  for (i = 0; i < TX_RING_SIZE; i++) {
    tx_ring[i].status = E1000_TXD_STAT_DD;
    tx_ring[i].addr = tx_buffs[i];
//    tx_mbufs[i] = 0;
  }
  regs[E1000_TDBAL] = (uint64) tx_ring;
  regs[E1000_TDBAH] = 0;
  if(sizeof(tx_ring) % 128 != 0)
    panic("e1000");
  regs[E1000_TDLEN] = sizeof(tx_ring);
  regs[E1000_TDH] = regs[E1000_TDT] = 0;
  
  // [E1000 14.4] Receive initialization
  //memset(rx_ring, 0, sizeof(rx_ring));
  for (i = 0; i < RX_RING_SIZE; i++) {
    //rx_buffs[i] = 0;//mbufalloc(0);
    
    rx_ring[i].addr = rx_buffs[i];  //(uint64) rx_buffs[i]->head;
  }
  regs[E1000_RDBAL] = (uint64) rx_ring;
  if(sizeof(rx_ring) % 128 != 0)
    panic("e1000");
  regs[E1000_RDH] = 0;
  regs[E1000_RDT] = RX_RING_SIZE - 1;
  regs[E1000_RDLEN] = sizeof(rx_ring);

  // filter by qemu's MAC address, 52:54:00:12:34:56
  regs[E1000_RA] = 0x12005452;
  regs[E1000_RA+1] = 0x5634 | (1<<31);
  // multicast table
  for (int i = 0; i < 4096/32; i++)
    regs[E1000_MTA + i] = 0;
putstr("hello2\n");
  // transmitter control bits.
  regs[E1000_TCTL] = E1000_TCTL_EN |  // enable
    E1000_TCTL_PSP |                  // pad short packets
    (0x10 << E1000_TCTL_CT_SHIFT) |   // collision stuff
    (0x40 << E1000_TCTL_COLD_SHIFT);
  regs[E1000_TIPG] = 10 | (8<<10) | (6<<20); // inter-pkt gap

  // receiver control bits.
  regs[E1000_RCTL] = E1000_RCTL_EN | // enable receiver
    E1000_RCTL_BAM |                 // enable broadcast
    E1000_RCTL_SZ_2048 |             // 2048-byte rx buffers
    E1000_RCTL_SECRC;                // strip CRC
  
  // ask e1000 for receive interrupts.
 // regs[E1000_RDTR] = 0; // interrupt after every received packet (no timer)
 // regs[E1000_RADV] = 0; // interrupt after every packet (no timer)
  //regs[E1000_IMS] = (1 << 7); // RXDW -- Receiver Descriptor Write Back
}

int
e1000_transmit(char * buf,int len)
{
 /*uint32_t tail = E1000_REG(E1000_TDT);

    if (len > TX_PKT_BUFF_SIZE)
        return -E_PKT_TOO_LONG;	

    if ((tx_queue[tail].cmd & E1000_TXD_CMD_RS) 
        && !(tx_queue[tail].sta & E1000_TXD_STA_DD))
        return -E_TX_FULL;

    memcpy(tx_buffs[tail], data, len);
    tx_queue[tail].length = len;
    tx_queue[tail].cmd |= E1000_TXD_CMD_RS | E1000_TXD_CMD_EOP;
    tx_queue[tail].sta &= ~E1000_TXD_STA_DD;

    E1000_REG(E1000_TDT) = (tail + 1) % NTXDESC;
   
    return 0;

*/	uint32 tail = regs[E1000_TDT];
printf("e1000_trans len =%d tail =%d\n",len,tail);
	    if (tx_ring[tail].cmd & E1000_TXD_CMD_RS){
	        if (!(tx_ring[tail].status & E1000_TXD_STAT_DD )){
	           return -1;
	        }
	        
	    }
	    printf("do memcpy in e1000_trans\n");
 	    memcpy(tx_buffs[tail], buf, len);
 	    tx_ring[tail].length =len;
	    tx_ring[tail].cmd |= E1000_TXD_CMD_RS | E1000_TXD_CMD_EOP;
	    tx_ring[tail].status &= ~E1000_TXD_STAT_DD;
	    //memcpy(tx_mbufs[tail],buf,len);
	    regs[E1000_TDT] = (tail + 1) % TX_RING_SIZE;

	    return 0;
  //
  // Your code here.
  //
  // the mbuf contains an ethernet frame; program it into
  // the TX descriptor ring so that the e1000 sends it. Stash
  // a pointer so that it can be freed after sending.
  //
  //return -1;
}


struct ethernet_h{
        //	unsigned char preamble[7];
        //	unsigned char delimiter;

    unsigned char destAddress[6];
	unsigned char srcAddress[6];
        // if value < 1500(max allowed frame size); specifies length - ver802.2
        // else value > 1536; specifies which protocol is encapsulated in the payload - Ethernet II framing
    unsigned char etherType[2];
};

struct ip_h
{
    /*need these to compute packet lengths*/
    unsigned char v_ihl; //internet header length
    unsigned char service; //Type of service - used to define the way routers handle the datagram
    unsigned char total_len[2]; //16 bits, max packet size - 2^16 - 65,536

    unsigned char identification[2]; //Used along with src address to uniquely id a datagram
    unsigned char offset[2]; // 00000xxx {Reserved = 0, Don't Fragment, Fragment} 00000000
    unsigned char ttl; //no. of hops
    unsigned char protocol; //http://bit.ly/c0xBMt list of ip protocols
    unsigned char checksum[2];
    unsigned char srcAddress[4];
    unsigned char destAddress[4];
};


struct tcp_h{
	unsigned char src_port[2];
	unsigned char dest_port[2];
	unsigned char seq_num[4];
	unsigned char ack[4];
	unsigned char offset_res_flag[2];
	unsigned char window_size[2];
	unsigned char check_sum[2];
	unsigned char urgent[2];
	unsigned char option[4];		//this char just indicates the first 4 bytes of the optional section. We me need to have a
};

struct tls_h{
	unsigned char type;
	unsigned char version[2];
	unsigned char length[2];
};


void parse(char *packet, int len){


           int  j,sz;
    printf("len =%d\n",len);
    /*Header Structs*/
    struct ethernet_h * ethernet;
    struct ip_h * ip;
    struct tcp_h * tcp;
        /*ethernet header memory map*/
        ethernet = (struct ethernet_h *)(packet);
        printf("\nMAC src:\t");
        for(j=0;j<6;j++)
        {
            printf("%x:", ethernet->srcAddress[j]);
        }

        printf("\nMAC dest:\t");
        for(j=0;j<6;j++)
        {
            printf("%x:", ethernet->destAddress[j]);
        }
        /*cacluate start of IP header and map to struct*/
        ip = (struct ip_h *) (packet + sizeof(struct ethernet_h));

        printf("\b\nIP src: \t");
        for (j=0;j<4;j++)
        {
            printf(" %d ", ip->srcAddress[j]);
        }
        printf("\nIP dest: \t");
        for (j=0;j<4;j++)
        {
            printf(" %d ", ip->destAddress[j]);
        }
        printf("\n");

 	// print src and dest port number
	tcp = (struct tcp_h *) (packet + sizeof(struct ethernet_h) + sizeof(struct ip_h)); //calulate tcp header and map to struct
	printf("source port- ");
	unsigned short src_port = *((unsigned short*)tcp->src_port);
	src_port = src_port>>8 | src_port<<8;
	printf("%d",src_port);

	printf("\n");
	printf("destination port- ");
	unsigned short dest_port = *((unsigned short*)tcp->dest_port);
	dest_port = dest_port>>8 | dest_port<<8;
	printf("%d",dest_port);
	printf("\n");
	sz = sizeof(struct ethernet_h) + sizeof(struct ip_h)+sizeof(struct tcp_h);
	char *p = (char *)(packet + sz);
	for (j=sz;j<len;j++){
		printf("%d ",*p++);
	}

#ifdef more
	//calulate tls header and map to struct. This calculation  checks for the first tls message if any. It checks only for TLSv1 (using 0x0301)
	int size = header.len;
	if( size >= sizeof(struct ethernet_h) + sizeof(struct ip_h) + sizeof(struct tcp_h)+ sizeof(struct tls_h)){ 	//check if header has enough bytes for tls
		tls = (struct tls_h *) (packet + sizeof(struct ethernet_h) + sizeof(struct ip_h) + sizeof(struct tcp_h));
		unsigned char version_upper = *((unsigned char*)tls->version);
		unsigned char version_lower = *((unsigned char*)tls->version+1);
		if (version_upper == 0x03 && version_lower == 0x01){
			cprintf("TLS 1.0: Yes\n");
		}
		else{
			cprintf("TLS 1.0: No\n");
		}
		version_upper = 0;	//clearing values
		version_lower = 0;	//clearing values
	}
	else{
		cprintf("TLS 1.0: No\n");
	}
	cprintf("\n");
    }
#endif
}
 int
e1000_recv(char *buf)
{
  //
  // Your code here.
  //
  // Check for packets that have arrived from the e1000
  // Create and deliver an mbuf for each packet (using net_rx()).
  //
	//while(1){
	 uint32 tail = regs[E1000_RDT];
	    uint32 next = (tail + 1) % RX_RING_SIZE;
	    int len=0;
//printf("e1000_recv\n");
	    if (!(rx_ring[next].status & E1000_RXD_STAT_DD))
	        return 0;
 len = rx_ring[next].length;
	    printf("next %d\n",next);
 printf("len %d\n",len);
printf("22 %d\n",22);
printf("22 %x\n",22);
printf("=");
//uputc(tab[1 % 10]);
//uputc(tab[2 % 10]);
//uputc(tab[7 % 10]);
printf("=\n");
printf("=");
//urpu( 54);
printf("=\n");
printf("=");
//uprintu( 654);
printf("=\n");
printf("=");
//uprintf("%d", 4654);
printf("=\n");
printf("=");
//uprintf("dd%d", 32323);
printf("=\n");


	    parse((char *)rx_ring[next].addr,len);
            memcpy(buf,rx_buffs[next],len);
	    //printf("call net_rx %p",rx_buffs[next]);
	    //rx_buffs[next]->len = len;
	    //net_rx(rx_buffs[next]);
	    //rx_buffs[next] = mbufalloc(0);
	   // if (!rx_buffs[next])
	   //    panic("e1000");
	    //rx_ring[next].addr =  rx_buffs[next];
	    regs[E1000_RDT] = next;
	    rx_ring[next].status &= ~E1000_RXD_STAT_DD;
	//}
	    return len;
}

void
e1000_intr(void)
{
	printf("raise interrupts\n");
	  regs[E1000_ICR];
  //e1000_recv();
  // tell the e1000 we've seen this interrupt;
  // without this the e1000 won't raise any
  // further interrupts.

}


