#include "include/buddy.h"
#include <stdio.h>

int main(void) {
	buddy_init();

	for (int i = 0; i < 8; ++i) {
		printf("addr: %p\n", buddy_alloc(PGSZ * 2));
	}
	
	for (int i = 0; i < 8; i += 2)
		buddy_free((void *)(i * PGSZ));
	
	printf("done freeing...\n");
	// dump_free_list();

	for (int i = 0; i < 8; ++i) {
		printf("addr: %p\n", buddy_alloc(PGSZ));
	}
	return 0;
}