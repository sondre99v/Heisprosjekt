#ifndef _ORDER_MANAGER_H_
#define _ORDER_MANAGER_H_

typedef enum {
	floor_unknown = -1
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
	Floor_t pickup_floor;			// Settes når bestillingen legges til i køen
	Direction_t pickup_direction;	// Settes når bestillingen legges til i køen
	Floor_t dropoff_floor;			// Settes når heisen har nådd pickup-floor
} Order_t;


void om_add_new_order (Order_t)
Order_t om_get_first_order (void)
bool om_add_dropoff_to_order (Order_t, Floor_t)
void om_remove_order (Order_t)
bool om_contains_pickup (Floor_t, Direction_t)
bool om_contains_dropoff (Floor_t)

#endif // _ORDER_MANAGER_H_