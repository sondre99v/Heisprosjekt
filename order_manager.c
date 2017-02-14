#include "order_manager.h"
#include <stdlib.h>

// The order queue is implemented as a singly linked list.
struct Order_list_node_t;
typedef struct {
	Order_t order;
	Order_list_node_t* next;
} Order_list_node_t;

static Order_list_node_t* order_queue = NULL;

static Order_list_node_t* _find_node_with_order (Order_t* order) {
	if (order_queue == NULL) {
		return NULL;
	}
	
	Order_list_node_t* node = order_queue;
	
	while (node -> order != order) {
		node = node -> next;
		if (node == NULL) {
			return NULL;
		}
	}
	
	return node;
}

static Order_list_node_t* _create_new_node (Floor_t pickup_floor, Direction_t direction, Floor_t dropoff_floor, Order_list_node_t* next) {
	// Allocate memory for new order
	Order_list_node_t* new_order_node = malloc(sizeof(Order_list_node_t));
	
	// Add values to the new node
	new_order_node -> order.pickup_floor = pickup_floor;
	new_order_node -> order.pickup_direction = direction;
	new_order_node -> order.dropoff_floor = dropoff_floor;
	new_order_node -> next = next;
	
	return new_order_node;
}

void om_add_new_order (Floor_t pickup_floor, Direction_t direction) {
	// Create new node with given values
	Order_list_node_t* new_order_node = _create_new_node(pickup_floor, pickup_direction, floor_unknown, order_queue);
	
	// Set the new node as the first in the queue
	order_queue = new_order_node;
}

Order_t* om_get_first_order (void) {
	if (order_queue == NULL) {
		// Order queue is empty
		return NULL;
	} else {
		return order_queue -> order;
	}
}

void om_add_dropoff_to_order (Order_t* order, Floor_t dropoff_floor) {
	if (order -> dropoff_floor == floor_unknown) {
		// Order has no drop-off floor, add it.
		order -> dropoff_floor = dropoff_floor;
	} else {
		// Order already has a drop-off floor. Create a copy with the new drop-off floor and place it behind.
		Order_list_node_t* node = _find_node_with_order(order);
		Order_list_node_t* new_node = _create_new_node(order -> pickup_floor, order -> pickup_direction, dropoff_floor, node -> next);
		node -> next = new_node;
	}
}

void om_remove_order (Order_t* order) {
	if (order_queue == NULL) {
		// There are no orders to remove
		return;
	}
	
	if (order_queue -> order == order) {
		// Order to remove was first in the queue
		Order_list_node_t* to_delete = order_queue;
		order_queue = order_queue -> next;
		free(to_delete);
		return;
	}
	
	// Search through queue to find the order before the one to remove
	Order_list_node_t* node = order_queue;
	
	while (node -> next.order != order) {
		node = node -> next;
		if (node -> next == NULL) {
			// Order to remove was not found in the queue
			return;
		}
	}
	
	// Found order before the one to remove
	Order_list_node_t* to_delete = node -> next;
	node -> next = node -> next -> next;
	free(to_delete);
}

void om_clear_all_orders (void) {
	while (order_queue != NULL) {
		om_remove_order (order_queue -> order);
	}
}

bool om_contains_pickup (Floor_t pickup_floor, Direction_t direction) {
	Order_list_node_t* node;
	for (node = order_queue ; node != NULL ; node = node -> next) {
		if (node->order.pickup_floor == pickup_floor && node->order.pickup_direction == direction) {
			return true;
		}
	}
	
	return false;
}

bool om_contains_dropoff (Floor_t dropoff_floor) {
	Order_list_node_t* node;
	for (node = order_queue ; node != NULL ; node = node -> next) {
		if (node->order.dropoff_floor == dropoff_floor) {
			return true;
		}
	}
	
	return false;
}
