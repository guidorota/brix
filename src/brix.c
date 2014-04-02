/*
 * brix.c
 *
 *  Created on: Feb 2, 2014
 *      Author: sidewinder
 */
#include <stdio.h>
#include "logging.h"
#include "document_manager/document_manager.h"
#include "virtual_machine/virtual_machine.h"
#include "runtime/pcode_manager.h"
#include "runtime/task_scheduler.h"
#include "runtime/timer.h"
#include "runtime/critical_section.h"

int main(int argc, char* argv[]) {

	BX_LOG(LOG_INFO, "main", "Initializing...");
	bx_critical_init();
	bx_vm_virtual_machine_init();
	bx_docman_init();
	bx_pcode_init();
	bx_timer_init();
	bx_sched_init();

	printf("Type q :return to quit\n");
	char input = 0;
	while(input != 'q') {
		input = getchar();
	}
	bx_timer_destroy();
	bx_critical_destroy();

	return 0;
}

