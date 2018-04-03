#ifndef ForceARP_h
#define ForceARP_h

#define ARP_FREQUENCY 1000

unsigned long lastTick = 0;

extern "C" {
  extern char *netif_list;
  uint8_t etharp_request(char *, char *);
}

void forceARP() {
  char *netif = netif_list;

  while (netif)
  {
    etharp_request((netif), (netif+4));
    netif = *((char **) netif);
  }
}

void forceARPLoop() {
    unsigned long now = millis();
    if (now - lastTick > ARP_FREQUENCY) {
      lastTick = now;
      forceARP();
    }
}

#endif // end ForceARP_h
