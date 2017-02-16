//Author: Finn Matras & Sondre Ninive Andersen

#include "order_manager.h"
#include <stdio.h>
#include <stdlib.h>

// The order queue is implemented as a singly linked list.
typedef struct Order_list_node_t{
	Order_t order;
	struct Order_list_node_t* next;
} Order_list_node_t;

static Order_list_node_t* order_queue = NULL;

static void _print_order_queue() {
	Order_list_node_t* node;
	
	int position = 1;
	
	printf("Order queue:\n");
	for (node = order_queue; node != NULL; node = node -> next) {
		printf("%d - ", position);
		position++;
		
		switch (node -> order.pickup_direction) {
			case direction_up: printf("Up from "); break;
			case direction_down: printf("Down from "); break;
			case direction_none: printf("??? from "); break;
		}
		
		switch (node -> order.pickup_floor) {
			case floor_1st: printf("1st to "); break;
			case floor_2nd: printf("2nd to "); break;
			case floor_3rd: printf("3rd to "); break;
			case floor_4th: printf("4th to "); break;
			case floor_unknown: printf("??? to "); break;
		}
		
		switch (node -> order.dropoff_floor) {
			case floor_1st: printf("1st\n"); break;
			case floor_2nd: printf("2nd\n"); break;
			case floor_3rd: printf("3rd\n"); break;
			case floor_4th: printf("4th\n"); break;
			case floor_unknown: printf("???\n"); break;
		}
	}
}

static Order_list_node_t* _find_node_with_order (Order_t* order) {
	if (order_queue == NULL) {
		return NULL;
	}
	
	Order_list_node_t* node = order_queue;
	
	while (&(node -> order) != order) {
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
	// Check if queue contains this order already
	Order_t* old_order = om_contains_pickup(pickup_floor, direction);
	if (old_order != NULL && old_order -> dropoff_floor == floor_unknown) {
#ifdef _DEBUG_MESSAGES_
		printf("Order already present.\n");
#endif
		return;
	}

	// Create new node with given values
	Order_list_node_t* new_order_node = _create_new_node(pickup_floor, direction, floor_unknown, order_queue);
	
	// Set the new node as the first in the queue
	order_queue = new_order_node;
	
#ifdef _DEBUG_MESSAGES_
	_print_order_queue();
#endif
}

void om_add_new_dropoff_only_order (Floor_t dropoff_floor) {
	// Check if queue contains this order already
	if (om_contains_dropoff(dropoff_floor)) {
#ifdef _DEBUG_MESSAGES_
		printf("Dropoff floor already present.\n");
#endif
		return;
	}

	// Create new node with given values
	Order_list_node_t* new_order_node = _create_new_node(floor_unknown, direction_none, dropoff_floor, order_queue);
	
	// Set the new node as the first in the queue
	order_queue = new_order_node;
	
#ifdef _DEBUG_MESSAGES_
	_print_order_queue();
#endif
}

Order_t* om_get_first_order (void) {
	if (order_queue == NULL) {
		// Order queue is empty
		return NULL;
	} else {
		Order_list_node_t* node = order_queue;
		while (node -> next != NULL) {
			node = node -> next;
		}
		return node -> order;
	}
}

void om_add_dropoff_to_order (Order_t* order, Floor_t dropoff_floor) {
	assert(order != NULL);
	
	if (order -> dropoff_floor == floor_unknown) {
		// Order has no drop-off floor, add it.
		order -> dropoff_floor = dropoff_floor;
	} else {
		// Order already has a drop-off floor. Create a copy with the new drop-off floor and place it behind.
		Order_list_node_t* node = _find_node_with_order(order);
		Order_list_node_t* new_node = _create_new_node(order -> pickup_floor, order -> pickup_direction, dropoff_floor, node -> next);
		node -> next = new_node;
	}
	
#ifdef _DEBUG_MESSAGES_
	printf("Dropoff floor added successfully!\n");
	_print_order_queue();
#endif
}

void om_remove_order (Order_t* order) {
	if (order_queue == NULL) {
		// There are no orders to remove
		return;
	}
	
	if (&(order_queue -> order) == order) {
		// Order to remove was first in the queue
		Order_list_node_t* to_delete = order_queue;
		order_queue = order_queue -> next;
		free(to_delete);
#ifdef _DEBUG_MESSAGES_
		printf("Order removed successfully (was 1st in queue)\n");
		_print_order_queue();
#endif
		return;
	}
	
	// Search through queue to find the order before the one to remove
	Order_list_node_t* node = order_queue;
	
	while (&(node -> next -> order) != order) {
		node = node -> next;
		if (node -> next == NULL) {
			// Order to remove was not found in the queue
			return;	
#ifdef _DEBUG_MESSAGES_
			printf("Order to delete was not found\n");
#endif
		}
	}
	
	// Found order before the one to remove
	Order_list_node_t* to_delete = node -> next;
	node -> next = node -> next -> next;
	free(to_delete);
	
#ifdef _DEBUG_MESSAGES_
	printf("Order removed successfully!\n");
	_print_order_queue();
#endif
}

void om_clear_all_orders (void) {
	while (order_queue != NULL) {
		om_remove_order (&(order_queue -> order));
	}
#ifdef _DEBUG_MESSAGES_
	printf("Order queue cleared successfully!\n");
	_print_order_queue();
#endif
}

Order_t* om_contains_pickup (Floor_t pickup_floor, Direction_t direction) {
	Order_list_node_t* node;
	for (node = order_queue ; node != NULL ; node = node -> next) {
		if (node -> order.pickup_floor == pickup_floor && node -> order.pickup_direction == direction) {
#ifdef _DEBUG_MESSAGES_
			printf("Found order with pickup!\n");
#endif
			return &(node -> order);
		}
	}
	
	return NULL;
}

Order_t* om_contains_dropoff (Floor_t dropoff_floor) {
	Order_list_node_t* node;
	for (node = order_queue ; node != NULL ; node = node -> next) {
		if (node -> order.dropoff_floor == dropoff_floor) {
#ifdef _DEBUG_MESSAGES_
			printf("Found order with dropoff!\n");
#endif
			return &(node -> order);
		}
	}
	
	return NULL;
}
