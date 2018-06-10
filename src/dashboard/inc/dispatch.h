#ifndef _DISPATCH_H_
#define _DISPATCH_H_

void dispatch_init(void);
void dispatch_update(void);

#define LIMIT(period)                                       \
  static uint32_t last_sent = 0;                            \
  if (msTicks - last_sent < period) return;                 \
  last_sent = msTicks;

#endif
