# Makefile for reip

LWIPDIR = src
LWIPARCH = reip
NEWLIBDIR = newlib
HTTPDDIR = httpd_raw
CC = /home/paul/Downloads/riscv64-unknown-elf-gcc-8.3.0-2019.08.0-x86_64-linux-ubuntu14/bin/riscv64-unknown-elf-gcc
LD = /home/paul/Downloads/riscv64-unknown-elf-gcc-8.3.0-2019.08.0-x86_64-linux-ubuntu14/bin/riscv64-unknown-elf-gcc
AS = $(CC)
CFLAGS = -mcmodel=medany -mno-relax  -c
#CFLAGS = -c -m64 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone
#CFLAGS:=$(CFLAGS) \
#	-I$(LWIPDIR)/include -I$(LWIPARCH)/include -I$(LWIPDIR)/include/ipv4 \
#	-I$(LWIPDIR)/include/ipv6 -I$(LWIPDIR) -I$(HTTPDDIR) -I$(NEWLIBDIR)/include -I.
CFLAGS:=$(CFLAGS) \
	-I$(LWIPDIR)/include -I$(LWIPARCH)/include -I$(LWIPDIR)/include/ipv4 \
	-I$(LWIPDIR)/include/ipv6 -I$(LWIPDIR) -I$(HTTPDDIR) -I.
LDFLAGS = -T default.lds -nostartfiles -nostdlib -nostdinc -static -lgcc \
                     -Wl,--nmagic -Wl,--gc-sections

# COREFILES, CORE4FILES: The minimum set of files needed for lwIP.
COREFILES=$(LWIPDIR)/core/def.c $(LWIPDIR)/core/dhcp.c $(LWIPDIR)/core/dns.c \
	$(LWIPDIR)/core/ipv4/inet_chksum.c $(LWIPDIR)/core/init.c $(LWIPDIR)/core/mem.c \
	$(LWIPDIR)/core/memp.c $(LWIPDIR)/core/netif.c $(LWIPDIR)/core/pbuf.c \
	$(LWIPDIR)/core/raw.c $(LWIPDIR)/core/stats.c $(LWIPDIR)/core/sys.c \
	$(LWIPDIR)/core/tcp.c $(LWIPDIR)/core/tcp_in.c \
	$(LWIPDIR)/core/tcp_out.c $(LWIPDIR)/core/timers.c $(LWIPDIR)/core/udp.c
CORE4FILES=$(LWIPDIR)/core/ipv4/autoip.c $(LWIPDIR)/core/ipv4/icmp.c \
	$(LWIPDIR)/core/ipv4/igmp.c $(LWIPDIR)/core/ipv4/ip_frag.c \
	$(LWIPDIR)/core/ipv4/ip.c $(LWIPDIR)/core/ipv4/ip_addr.c $(LWIPDIR)/api/err.c

# NETIFFILES: Files implementing various generic network interface functions.
NETIFFILES=$(LWIPDIR)/netif/etharp.c

# ARCHFILES: Architecture specific files.
ARCHFILES=httpd_raw/fs.c httpd_raw/httpd.c crtm.s pci.c e1000.c reip/reip.c reip/renetif.c reip/libBareMetal.c

# LWIPFILES: All the above.
LWIPFILES=$(COREFILES) $(CORE4FILES) $(NETIFFILES) $(ARCHFILES)
LWIPFILESW=$(wildcard $(LWIPFILES))
#LWIPOBJS=$(notdir $(LWIPFILESW:.c=.o))
#LWIPOBJS=$($(LWIPFILESW:.c=.o))
TARGET = liblwip.a

#.PHONY: all
all:	httpd.app

src/core/def.o: src/core/def.c
	$(CC) $(CFLAGS) -o src/core/def.o src/core/def.c
src/core/dhcp.o: src/core/dhcp.c
	$(CC) $(CFLAGS) -o src/core/dhcp.o src/core/dhcp.c
src/core/dns.o: src/core/dns.c
	$(CC) $(CFLAGS) -o src/core/dns.o src/core/dns.c
src/core/ipv4/inet_chksum.o: src/core/ipv4/inet_chksum.c
	$(CC) $(CFLAGS) -o src/core/ipv4/inet_chksum.o src/core/ipv4/inet_chksum.c
src/core/init.o: src/core/init.c
	$(CC) $(CFLAGS) -o src/core/init.o src/core/init.c
src/core/mem.o: src/core/mem.c
	$(CC) $(CFLAGS) -o src/core/mem.o src/core/mem.c
src/core/memp.o: src/core/memp.c
	$(CC) $(CFLAGS) -o src/core/memp.o src/core/memp.c
src/core/netif.o: src/core/netif.c
	$(CC) $(CFLAGS) -o src/core/netif.o src/core/netif.c
src/core/pbuf.o: src/core/pbuf.c
	$(CC) $(CFLAGS) -o src/core/pbuf.o src/core/pbuf.c
src/core/raw.o: src/core/raw.c
	$(CC) $(CFLAGS) -o src/core/raw.o src/core/raw.c
src/core/stats.o: src/core/stats.c
	$(CC) $(CFLAGS) -o src/core/stats.o src/core/stats.c
src/core/sys.o: src/core/sys.c
	$(CC) $(CFLAGS) -o src/core/sys.o src/core/sys.c
src/core/tcp.o: src/core/tcp.c
	$(CC) $(CFLAGS) -o src/core/tcp.o src/core/tcp.c
src/core/tcp_in.o: src/core/tcp_in.c
	$(CC) $(CFLAGS) -o src/core/tcp_in.o src/core/tcp_in.c
src/core/tcp_out.o: src/core/tcp_out.c
	$(CC) $(CFLAGS) -o src/core/tcp_out.o src/core/tcp_out.c
src/core/timers.o: src/core/timers.c
	$(CC) $(CFLAGS) -o src/core/timers.o src/core/timers.c
src/core/udp.o: src/core/udp.c
	$(CC) $(CFLAGS) -o src/core/udp.o src/core/udp.c
src/core/ipv4/autoip.o: src/core/ipv4/autoip.c
	$(CC) $(CFLAGS) -o src/core/ipv4/autoip.o src/core/ipv4/autoip.c
src/core/ipv4/icmp.o: src/core/ipv4/icmp.c
	$(CC) $(CFLAGS) -o src/core/ipv4/icmp.o src/core/ipv4/icmp.c
src/core/ipv4/igmp.o: src/core/ipv4/igmp.c
	$(CC) $(CFLAGS) -o src/core/ipv4/igmp.o src/core/ipv4/igmp.c
src/core/ipv4/ip_frag.o: src/core/ipv4/ip_frag.c
	$(CC) $(CFLAGS) -o src/core/ipv4/ip_frag.o src/core/ipv4/ip_frag.c
src/core/ipv4/ip.o: src/core/ipv4/ip.c
	$(CC) $(CFLAGS) -o src/core/ipv4/ip.o src/core/ipv4/ip.c
src/core/ipv4/ip_addr.o: src/core/ipv4/ip_addr.c
	$(CC) $(CFLAGS) -o src/core/ipv4/ip_addr.o src/core/ipv4/ip_addr.c
src/api/err.o: src/api/err.c
	$(CC) $(CFLAGS) -o src/api/err.o src/api/err.c
src/netif/etharp.o: src/netif/etharp.c
	$(CC) $(CFLAGS) -o src/netif/etharp.o src/netif/etharp.c
httpd_raw/fs.o: httpd_raw/fs.c
	$(CC) $(CFLAGS) -o httpd_raw/fs.o httpd_raw/fs.c
httpd_raw/httpd.o: httpd_raw/httpd.c
	$(CC) $(CFLAGS) -o httpd_raw/httpd.o httpd_raw/httpd.c
crtm.o: crtm.s
	$(CC) $(CFLAGS) -o crtm.o crtm.s
pci.o: pci.c
	$(CC) $(CFLAGS) -o pci.o pci.c
e1000.o: e1000.c
	$(CC) $(CFLAGS) -o e1000.o e1000.c
reip/reip.o: reip/reip.c
	$(CC) $(CFLAGS) -o reip/reip.o reip/reip.c
reip/renetif.o: reip/renetif.c
	$(CC) $(CFLAGS) -o reip/renetif.o reip/renetif.c
reip/libBareMetal.o: reip/libBareMetal.c
	$(CC) $(CFLAGS) -o reip/libBareMetal.o reip/libBareMetal.c

#%.o: %.c
OBJS= src/core/def.o src/core/dhcp.o src/core/dns.o src/core/ipv4/inet_chksum.o src/core/init.o src/core/mem.o\
 src/core/memp.o src/core/netif.o src/core/pbuf.o src/core/raw.o src/core/stats.o src/core/sys.o\
 src/core/tcp.o src/core/tcp_in.o src/core/tcp_out.o src/core/timers.o src/core/udp.o\
 src/core/ipv4/autoip.o src/core/ipv4/icmp.o src/core/ipv4/igmp.o src/core/ipv4/ip_frag.o \
src/core/ipv4/ip.o src/core/ipv4/ip_addr.o src/api/err.o src/netif/etharp.o httpd_raw/fs.o\
 httpd_raw/httpd.o crtm.o pci.o e1000.o reip/reip.o reip/renetif.o reip/libBareMetal.o

httpd.app: $(OBJS)	 
#	echo $(LWIPOBJS)
	$(LD) $(LDFLAGS) -o  httpd.app $(OBJS) 
#reip.o renetif.o fs.o httpd.o libBareMetal.o crtm.o pci.o e1000.o
#	$(CC) $(CFLAGS) $(LWIPFILES)
#	ar -cvq $(TARGET) $(LWIPOBJS)
#	$(CC) $(CFLAGS) reip/reip.c reip/renetif.c reip/libBareMetal.c
#	$(CC) $(CFLAGS) httpd_raw/fs.c httpd_raw/httpd.c crtm.s pci.c e1000.c
#	$(LD) $(LDFLAGS) -o reip.app newlib/crt0.o reip.o renetif.o fs.o httpd.o libBareMetal.o liblwip.a newlib/libc.a
#	$(LD) $(LDFLAGS) -o  httpd.app reip.o renetif.o fs.o httpd.o libBareMetal.o crtm.o pci.o e1000.o $(LWIPOBJS) 
#	mv reip.app httpd.app

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET) *.app
