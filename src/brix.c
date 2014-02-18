/*
 * brix.c
 *
 *  Created on: Feb 2, 2014
 *      Author: sidewinder
 */
#include <stdio.h>
#include "logging.h"
#include "virtual_machine/virtual_machine.h"
#include "virtual_machine/instruction_set.h"

int main(int argc, char* argv[]) {

	BX_LOG(LOG_INFO, "main", "Initializing...");
	bx_vm_init();
	bx_instr_init();

	return 0;
}

