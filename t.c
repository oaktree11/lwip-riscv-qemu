char *arr[]={"src/core/def.c", "src/core/dhcp.c", "src/core/dns.c", "src/core/ipv4/inet_chksum.c", "src/core/init.c", "src/core/mem.c", "src/core/memp.c", "src/core/netif.c", "src/core/pbuf.c", "src/core/raw.c", "src/core/stats.c", "src/core/sys.c", "src/core/tcp.c", "src/core/tcp_in.c", "src/core/tcp_out.c", "src/core/timers.c","src/core/udp.c", "src/core/ipv4/autoip.c","src/core/ipv4/icmp.c", "src/core/ipv4/igmp.c", "src/core/ipv4/ip_frag.c", "src/core/ipv4/ip.c", "src/core/ipv4/ip_addr.c", "src/api/err.c", "src/netif/etharp.c", "httpd_raw/fs.c", "httpd_raw/httpd.c", "crtm.s", "pci.c", "e1000.c", "reip/reip.c", "reip/renetif.c", "reip/libBareMetal.c",(char*)0 };
char buf[400];
char buf1[400];
char * strchr(char *,int);
char arr1[100][40];
main(){
	int i;
	for (i=0;arr[i]!=(char*)0;i++){
		strcpy(buf1,arr[i]);
		char *p = strchr(buf1,'.');
if (!p) printf("fred");
else{
		char ch =*(p+1); 
		*(p+1) = 'o';
		strcpy(arr1[i],buf1);
	//	printf("=%s=ch=%c\n",p,ch);
		*(p+1) =0; 
	//	printf(" =%s=ch=%c\n",buf1);
		strcpy(buf,buf1);
		*(p+1) = ch;
	//	printf("%s\n",buf);
		printf("%so: %s\n\t$(CC) $(CFLAGS) -o %so %s\n", buf,buf1,buf,buf1);
			    //	$(LWIPDIR)/core/dhcp.c
}	//$(CC) $(CFLAGS) -o dhcp.o $(LWIPDIR)/core/dhcp.c
	}
	for (int j=0;j<i;j++){
	printf("%s ",arr1[j]);
	}

}

