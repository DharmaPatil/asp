#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);

/* ��ע��4-5֮������ȼ���ΪPIP���߱�����7-9֮������ȼ���LwIPЭ��ջռ�� */




#endif 
