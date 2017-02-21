//Author: Finn Matras & Sondre Ninive Andersen

#ifndef __ORDER_MANAGER_H__
#define __ORDER_MANAGER_H__

#include <stdbool.h>

#define _DEBUG_MESSAGES_

typedef enum {
	floor_unknown = -1,
	floor_1st = 1,
	floor_2nd = 2,
	floor_3rd = 3,
	floor_4th = 4
} Floor_t;

typedef enum {
	direction_down,
	direction_none,
	direction_up
} Direction_t;

typedef struct {
	Floor_t pickup_floor;
	Direction_t pickup_direction;
	Floor_t dropoff_floor;
} Order_t;


void om_add_new_order(Floor_t pickup_floor, Direction_t direction);

void om_add_new_dropoff_only_order(Floor_t dropoff_floor);

void om_add_dropoff_to_order(Order_t* order, Floor_t dropoff_floor);

void om_remove_order(Order_t* order);

void om_remove_orders_with_dropoff(Floor_t dropoff_floor);

void om_remove_all_orders( void );

Order_t* om_get_first_order( void );

Order_t* om_contains_pickup(Floor_t pickup_floor, Direction_t direction);

Order_t* om_contains_dropoff(Floor_t dropoff_floor);

#endif // #ifndef __ORDER_MANAGER_H__