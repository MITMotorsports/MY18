#ifndef _PAGE_MANAGER_H_
#define _PAGE_MANAGER_H_

#include "carstats.h"
#include "NHD_US2066.h"

typedef enum {
    DASH_PAGE_CRITICAL = 0,
    //DASH_PAGE_TAKEOVER,
    //DASH_PAGE_TRACTION,
    //DASH_PAGE_WHEEL_SPEED,
    DASH_PAGE_CHARGE,
    DASH_PAGE_COUNT
} dash_page_type;

typedef struct {
    dash_page_type page;
    carstats_t *stats;
} page_manager_t;

void page_manager_init(page_manager_t *pm, carstats_t *stats);
void page_manager_next_page(page_manager_t *pm);
void page_manager_prev_page(page_manager_t *pm);
void page_manager_set_page(page_manager_t *pm, dash_page_type page);
void page_manager_update(page_manager_t *pm, NHD_US2066_OLED *oled);

#endif
