
/*
 * File:  car_black_box.h
 * Author: MOHAMMAED SUHAN
 * project :Car black box
 *
 * Created on 24 November, 2024, 9:15 AM
 */
#ifndef CAR_H
#define	CAR_H
#include <xc.h>   
void store_event(int event_index2);
void menu_screen(void);
void get_time(void);
void view_log(void);
void download_log(void);
void clear_log(void);
void set_time(void);
void current_time(void);
void update_time_in_ds1307(void);
#endif	

