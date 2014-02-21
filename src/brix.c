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

int main(int argc, char* argv[]) {

	BX_LOG(LOG_INFO, "main", "Initializing...");
	bx_vm_virtual_machine_init();
	bx_dm_document_manager_init();

	return 0;
}

