#ifndef RTC_H__
#define RTC_H__

#include <stdint.h>
#include <string.h>
#include "ble_cts_c.h"

#ifdef __cplusplus
extern "C" {
#endif


void initial_calendar(void);
void rtc2_config(void);
void module_time_print(void);
void set_calendar(ble_cts_c_evt_t * p_evt);

#ifdef __cplusplus
}
#endif

#endif // RTC_H__

/** @} */