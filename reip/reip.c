/*
   Poll ethernet driver to process received frames.
   The driver will pass them up to the lwIP stack using the input function
   you specified in netif_add.
   Periodically invoke the lwIP timer processing functions (etharp_tmr(),
   ip_reass_tmr(), tcp_tmr(), etc.
   the exact list depends on which protocols you have enabled).
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stddef.h>
#include "libBareMetal.h"

#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/ip_frag.h"
#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/stats.h"
#include "netif/etharp.h"
#include "renetif.h"
#define uint8_t unsigned char
#define uint32_t unsigned int
#define uint64 unsigned long
#include "riscv.h"
#define RE_IPADDR_BASE "10.0.2.15"
#define RE_IPADDR_NETMASK "255.255.255.0"
#define RE_IPADDR_GATEWAY "10.0.2.2"
#define uint8_t unsigned char
#define uint32_t unsigned int
unsigned long time_since;
// assembly code in kernelvec.S for machine-mode timer interrupt.
extern void timervec();

int
strncmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return (0);
	do {
		if (*s1 != *s2++)
			return (*(unsigned char *)s1 - *(unsigned char *)--s2);
		if (*s1++ == 0)
			break;
	} while (--n != 0);
	return (0);
}
int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s2 && *s1 == *s2) { s1++; s2++; }
    return *s1 - *s2;
}
size_t strlen(const char* s)
{
    const char *p = s;
    for (;; s++) {
        if (!*s) break;
    }
    return s - p;
}
char *strchr(const char *s, int c)
{
    char *p = (char *)s;
    while (*p) {
       if (*p == c) return p;
       p++;
    }
    return (char*)0;
}
int memcmp(const void *s1, const void *s2, size_t n)
{
    while (n-- > 0) {
        char c1 = *(const char*)s1++;
        char c2 = *(const char*)s2++;
        if (c1 != c2) {
            return c1 - c2;
        }
    }
    return 0;
}

puts(const char *s){
printf(s);
return 1;
}
void *memset(void *p, int val, size_t size){

for (int i=0;i<size;i++)
    ((char *)p)[i] = val;
return p;
}

enum {
    HART0_CLOCK_FREQ           = 0x00010000,
    UART0_CLOCK_FREQ           = 0x00011000,
    UART0_BAUD_RATE            = 0x00011100,
    NS16550A_UART0_CTRL_ADDR   = 0x00030000,
    RISCV_HTIF_BASE_ADDR       = 0x00050000,
    SIFIVE_CLINT_CTRL_ADDR     = 0x55550000,
    SIFIVE_CLIC_CRTL_ADDR      = 0x55550001,
    SIFIVE_TEST_CTRL_ADDR      = 0x55550002,
    SIFIVE_UART0_CTRL_ADDR     = 0x55550010,
    SIFIVE_GPIO0_CTRL_ADDR     = 0x55550020,
    SIFIVE_SPI0_CTRL_ADDR      = 0x55550030,
};

typedef struct auxval {
    unsigned long key;
    unsigned long val;
} auxval_t;


unsigned long getauxval(unsigned long key);
auxval_t __auxv[] = {
    { UART0_CLOCK_FREQ,         1843200    },
    { UART0_BAUD_RATE,          115200     },
    { NS16550A_UART0_CTRL_ADDR, 0x10000000 },
    { SIFIVE_TEST_CTRL_ADDR,    0x100000   },
    { 0, 0 }
};
enum {
    UART_RBR      = 0x00,  /* Receive Buffer Register */
    UART_THR      = 0x00,  /* Transmit Hold Register */
    UART_IER      = 0x01,  /* Interrupt Enable Register */
    UART_DLL      = 0x00,  /* Divisor LSB (LCR_DLAB) */
    UART_DLM      = 0x01,  /* Divisor MSB (LCR_DLAB) */
    UART_FCR      = 0x02,  /* FIFO Control Register */
    UART_LCR      = 0x03,  /* Line Control Register */
    UART_MCR      = 0x04,  /* Modem Control Register */
    UART_LSR      = 0x05,  /* Line Status Register */
    UART_MSR      = 0x06,  /* Modem Status Register */
    UART_SCR      = 0x07,  /* Scratch Register */

    UART_LCR_DLAB = 0x80,  /* Divisor Latch Bit */
    UART_LCR_8BIT = 0x03,  /* 8-bit */
    UART_LCR_PODD = 0x08,  /* Parity Odd */

    UART_LSR_DA   = 0x01,  /* Data Available */
    UART_LSR_OE   = 0x02,  /* Overrun Error */
    UART_LSR_PE   = 0x04,  /* Parity Error */
    UART_LSR_FE   = 0x08,  /* Framing Error */
    UART_LSR_BI   = 0x10,  /* Break indicator */
    UART_LSR_RE   = 0x20,  /* THR is empty */
    UART_LSR_RI   = 0x40,  /* THR is empty and line is idle */
    UART_LSR_EF   = 0x80,  /* Erroneous data in FIFO */
};

static volatile uint8_t *uart;
unsigned long getauxval(unsigned long key)
{
    auxval_t *auxv = __auxv;
    while(auxv->key) {
        if (auxv->key == key) {
            return auxv->val;
        }
        auxv++;
    }
    return 0;
}

static void ns16550a_init()
{
	uart = (uint8_t *)(void *)getauxval(NS16550A_UART0_CTRL_ADDR);
	uint32_t uart_freq = getauxval(UART0_CLOCK_FREQ);
	uint32_t baud_rate = getauxval(UART0_BAUD_RATE);
    uint32_t divisor = uart_freq / (16 * baud_rate);
    uart[UART_LCR] = UART_LCR_DLAB;
    uart[UART_DLL] = divisor & 0xff;
    uart[UART_DLM] = (divisor >> 8) & 0xff;
    uart[UART_LCR] = UART_LCR_PODD | UART_LCR_8BIT;
}

int ns16550a_getchar()
{
    if (uart[UART_LSR] & UART_LSR_DA) {
        return uart[UART_RBR];
    } else {
        return -1;
    }
}

 int ns16550a_putchar(int ch)
{
    while ((uart[UART_LSR] & UART_LSR_RI) == 0);
    return uart[UART_THR] = ch & 0xff;
}
putstr(char *p){
for (;*p;p++)
    ns16550a_putchar(*p);
}
test1(int x,int y){
return x+y;
}
char buf[2000];
// scratch area for timer interrupt, one per CPU.
uint64 mscratch0[ 32];
#define MS_PER_TICK 1

struct ip_addr re_ip_addr;
/* Generic data structure used for all lwIP network interfaces. */
static struct netif redrv_netif;

/* ----- PART NEEDED BY LWIP LIBRARY ----- */
static unsigned long start_time; /* save time from system start here */

/* returns current time in ms. Needed for lwip internal timeouts */
u32_t sys_now(void)
{
	unsigned long msec = mscratch0[6] * MS_PER_TICK - start_time;
	//printf("msec : %lu \n", msec);

	return msec;
}
// local interrupt controller, which contains the timer.
#define CLINT 0x2000000L
#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + 8*(hartid))
#define CLINT_MTIME (CLINT + 0xBFF8) // cycles since boot.

void timerintr(){
	printf("timerintr\n");
	time_since++;
}
void trap_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
	uint64 scause = r_scause();
	printf("traphandler\n");
	  if((scause & 0x8000000000000000L) &&
	     (scause & 0xff) == 9){
	    // this is a supervisor external interrupt, via PLIC.

	    // irq indicates which device interrupted.
	   // int irq = plic_claim();

	/*    if(irq == UART0_IRQ){
	      uartintr();
	    } else if(irq == VIRTIO0_IRQ){
	      virtio_disk_intr();
	    }*/

	  //  plic_complete(irq);
	    //return 1;
	  } else if(scause == 0x8000000000000001L){
	    // software interrupt from a machine-mode timer interrupt,
	    // forwarded by timervec in kernelvec.S.

	    //if(cpuid() == 0){
	      timerintr();
	    //}

	    // acknowledge the software interrupt by clearing
	    // the SSIP bit in sip.
	    w_sip(r_sip() & ~2);



	  }

}
void
timerinit()
{
  // each CPU has a separate source of timer interrupts.
  int id = 0;

  // ask the CLINT for a timer interrupt. fires 10000000 per sec
  int interval = 10000; // cycles; about 1/10th second in qemu.
  *(uint64*)CLINT_MTIMECMP(id) = *(uint64*)CLINT_MTIME + interval;

  // prepare information in scratch[] for timervec.
  // scratch[0..3] : space for timervec to save registers.
  // scratch[4] : address of CLINT MTIMECMP register.
  // scratch[5] : desired interval (in cycles) between timer interrupts.
  uint64 *scratch = mscratch0;
  scratch[4] = CLINT_MTIMECMP(id);
  scratch[5] = interval;
  scratch[6]=0;
  w_mscratch((uint64)scratch);

  // set the machine-mode trap handler.
  w_mtvec((uint64)timervec);

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}

int putchar(int ch){
	ns16550a_putchar( ch);
	return 1;
}
void sys_init(void)
{
	printf("sys_init() called!\n");
	start_time = time_since * MS_PER_TICK;
	//gettimeofday(&starttime, NULL);
}
/* ----- END OF PART NEEDED BY LWIP LIBRARY ----- */

int reip_init()
{
	struct ip_addr gw_addr, netmask;
	void * redrvnet_priv = NULL;
/*
	if (n_args == 4)
	{
		printf("4 args: %s , %s , %s \n", args[1], args[2], args[3]);

		if (!inet_aton(args[1], &re_ip_addr))
			return 1;
		if (!inet_aton(args[2], &netmask))
			return 1;
		if (!inet_aton(args[3], &gw_addr))
			return 1;
	}
	else
	{

	}*/
 
	if (!inet_aton(RE_IPADDR_BASE, &re_ip_addr))
				return 1;
			if (!inet_aton(RE_IPADDR_NETMASK, &netmask))
				return 1;
			if (!inet_aton(RE_IPADDR_GATEWAY, &gw_addr))
				return 1;
	/* Initialize LWIP */
	lwip_init();

	/* Initialize httpd_raw */
	httpd_init();
	printf("after httpd_init\n");
	/* Add a network interface to the list of lwIP netifs */
	if (!netif_add(&redrv_netif, &re_ip_addr, &netmask, &gw_addr,
			 NULL, reif_init, ethernet_input))
	{
		printf("netif_add failed\n");
		return 1;
	}
	printf("netif_add1\n");
	netif_set_default(&redrv_netif);
	netif_set_up(&redrv_netif);

	printf("Interface is %s \n", netif_is_up(&redrv_netif) ? "UP" : "DOWN");

	return 0;
}

// Regular polling mechanism.  This should be called each time through
// the main application loop (after each interrupt, regardless of source).
// It handles any received packets, permits NIC device driver house-keeping
// and invokes timer-based TCP/IP functions (TCP retransmissions, delayed
// acks, IP reassembly timeouts, ARP timeouts, etc.)
char packet1[1500] = {0};
void reip_poll(void)
{

	int len;
	//printf("call ethernet_rx\n");
	len = b_ethernet_rx(packet1);
	if (len)
	{
printf("reip got len %d\n",len);
		reif_input(&redrv_netif, packet1, len);
	}

	sys_check_timeouts();
}

int main()
{
test1(9,8);
    ns16550a_init();
    pci_init();
	printf("reip started v1.00\n");
	printf("reip started v1\n");
	timerinit();
	if (reip_init())
	{
		printf("reip_init() failed\n");
		return 1;
	}
	int s =sys_now();
	while (1) 
	{
		int t = sys_now();
		if (t-s == 1000){
			s=t;
			printf("time\n");
		}
		reip_poll();
	}
}
