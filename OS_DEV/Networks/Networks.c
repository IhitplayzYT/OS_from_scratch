/* Networks.c */
#include "Networks.h"

i32 global_id;

int main() { 
srand(getpid());
global_id = rand() % 50000;
Icmp * pkt;
i8* str = (i8*)"Hello";
pkt = init_icmp(echo,str,6);
Ip* ip = init_ip(UDP,(i8*)"192.168.10.5",(i8*)"212.16.16.214",0);
if (!ip) return -1;
ip->payload = pkt;
show_ip(ip,1);
printf("0x%hhx 0x%hhx\n",ip->dstaddr,ip->srcaddr);
print_hex(eval_ip(ip),sizeof(Raw_ip) + sizeof(Raw_icmp) + ip->payload->size);
free(ip->payload);
free(ip);
return 0; }

public void show_ip(Ip* ip,i8 flag){
if (!ip) return;
printf("\n---IP HEADER---\nID : %d\nType : %s\n[ %s ] -> [ %s ]\n   (SRC)      ->     (DST)\n",ip->id,(ip->type == TCP)?"TCP":(ip->type == UDP)?"UDP":(ip->type == ICMP)?"ICMP":"Invalid Type",ipstr(ip->srcaddr),ipstr(ip->dstaddr));
if (ip->payload) helper_ip_icmp(ip->payload,flag);
else printf("-------------\n\n");}


public i8* eval_icmp(Icmp* icmp){
if (!icmp) return (i8*)0;
i8 * p;
Raw_icmp raw;
switch(icmp->type){
case echo:
raw.type = 8;
raw.code = 0;
break;
case echo_reply:
raw.type = 0;
raw.code = 0;
break;
case None:
default:
return (i8*)0;
break;
}
i32 size = sizeof(Raw_icmp) + icmp->size;
if (size & 1) size++;
p = (i8*)malloc(size);
size = sizeof(Raw_icmp);
memcopy(p,&raw,size);
memcopy(p+size,icmp->header,icmp->size);
i16 check = checksum(p,sizeof(Raw_icmp) + icmp->size); 
Raw_icmp * rawp = (Raw_icmp*)p;
rawp->checksum = check;
return p;
}

public i16 checksum(i8* str,i16 size){
i32 acc = 0;
i16 carry,n = size,b,sum;
i16 *p;
for (p = (i16*)str;n;p++,n-=2){
b =*p;
acc += b;
}
carry = (acc & 0xffff0000) >> 16;
sum = acc & 0x0000ffff;
return endian(~(sum + carry));
}


public i32 ipaddr(i8* s){
i8 a[4] = {0},*p,c = 0;
i32 ret;
for (p = s;*p;p++){
if (*p == '.' || *p == '-' || *p == ':') c++;
else{
a[c] *= 10;
a[c] += *p - '0';
}}
ret = (a[3] << 24) | (a[2] << 16) | (a[1] << 8) | a[0];
return ret;
}

public i8* ipstr(i32 addr){
i8 *buff = (i8*)malloc(16);
zero(buff,16);
i8 a[4];
a[0] = (addr & 0xff000000) >> 24;
a[1] = (addr & 0x00ff0000) >> 16;
a[2] = (addr & 0x0000ff00) >> 8;
a[3] = (addr & 0x000000ff);
snprintf((char*)buff, 16, "%u.%u.%u.%u", a[3], a[2], a[1], a[0]);
return buff;
}

public void show_icmp(Icmp* icmp,i8 df){
if (!icmp){return;}
printf("\n---ICMP HEADER---\nType : %s\nSize : %d Bytes\n",(icmp->type == None)?"Default":(icmp->type == echo)?"Echo":(icmp->type == echo_reply)?"Echo Reply":"Invalid Type",icmp->size);
if (df) print_hex(icmp->header,icmp->size);
printf("-------------\n\n");
}

public void helper_ip_icmp(Icmp* icmp,i8 df){
if (!icmp){return;}
printf("\n  [Payload]\n\tType : %s\n\tSize : %d Bytes\n\t",(icmp->type == None)?"Default":(icmp->type == echo)?"Echo":(icmp->type == echo_reply)?"Echo Reply":"Invalid Type",icmp->size);
if (df) print_hex(icmp->header,icmp->size);
printf("-------------\n\n");
}

public i8 * eval_ip(Ip* ip){
if (!ip) return (i8*)0;
Raw_ip raw;
switch(ip->type){
case ICMP:
raw.protocol = 1;
default:
break;} 

raw.checksum = 0;
raw.dscp = 0;
raw.dstaddr = ip->dstaddr;
raw.srcaddr = ip->srcaddr;
raw.ecn = 0;
raw.flags = 0;
raw.frag_offset= 0;
raw.id = endian(ip->id);
raw.ihl = sizeof(Raw_ip)/4;
i16 le;
if (ip->payload) le = raw.ihl*4 +ip->payload->size+ sizeof(Raw_icmp);
else le = raw.len = raw.ihl*4;
raw.len = (ip->payload)? endian(le):raw.ihl*4;
raw.ttl = 250;
raw.version = 4;
if (le % 2) le++;

i8 * p = (i8*)malloc(le);
if (!p){return (i8*)0;}
zero(p,le);
memcopy(p,&raw,sizeof(Raw_ip));
p+=sizeof(Raw_ip);

if (ip->payload){
    i8*k = eval_icmp(ip->payload);
    if (k){copy(p,k,ip->payload->size);}
    free(k);
}
Raw_ip * rawp;
rawp = (Raw_ip*)p;
rawp->checksum = checksum(p,raw.len);
return p;
}

public i16 endian(i16 x){
i8 a,b;
i16 c;
a = x & 0x00ff;
b = (x & 0xff00) >> 8;
c = (a << 8) | b;
return c;
}




