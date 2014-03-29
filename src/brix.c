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
#include "runtime/pcode_repository.h"
#include "runtime/timer.h"

int main(int argc, char* argv[]) {

	BX_LOG(LOG_INFO, "main", "Initializing...");
	bx_vm_virtual_machine_init();
	bx_dm_document_manager_init();
	bx_pr_init();
	bx_tm_init();

	printf("Type q :return to quit\n");
	char input = 0;
	while(input != 'q') {
		input = getchar();
	}
	bx_tm_destroy();

	return 0;
}

