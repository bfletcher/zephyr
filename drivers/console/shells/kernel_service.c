/*
 * Copyright (c) 2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <misc/printk.h>
#include <misc/shell.h>
#include <init.h>

#define SHELL_KERNEL "kernel"

static int hex2int(char *pos, int *val)
{

   while (*pos){
      if ((*pos > 0x2F) && (*pos < 0x3A)) *val += *pos - 0x30;
         else if ((*pos > 0x40) && (*pos < 0x47)) *val += *pos - 0x37;
            else return 1;
      pos++;
      if (*pos) *val = *val << 4;
   }
   return 0;
}

static int shell_cmd_read(int argc, char *argv[])
{
/* 1st arg is address as a string of HEX (caps) digits - no lead 0x */
        int addr=0;
        int *result;
        int err=0;
        if(argc<2) err=1;
        err |= hex2int(argv[1], &addr);
        if(err){
            printk("Usage : read HEX_address\n");
            return 1;
        }
        result = (int *) addr;
        printk("%X = %X\n", addr, *result);

        return 0;
}

static int shell_cmd_write(int argc, char *argv[])
{
/* 2 args - address and value as strings of hex digits */
        int addr=0;
        int value=0;
        int err=0;
        int *tmp;
        if(argc<3) err=1;
        err |= hex2int(argv[1], &addr);
        err |= hex2int(argv[2], &value);
        if(err){
            printk("Usage : write HEX_address HEX_content\n");
            return 1;
        }
        tmp = (int *) addr;
        *tmp = value;
        printk("%X <= %X\n", addr, value);

        return 0;
}

static int shell_cmd_dump(int argc, char *argv[])
{
/* 1st arg is address as a string of hex digits, 2nd is range of hex words to dump */
        int addr=0;
        int range=0;
        int *result;
        int err=0;
        if(argc<3) err=1;
        err |= hex2int(argv[1], &addr);
        err |= hex2int(argv[2], &range);
        if(err){
            printk("Usage : dump HEX_address HEX_range\n");
            return 1;
        }
        for (int i = addr ; i < addr+range*4 ; i+=4 ){
           result = (int *) i ;
           printk("%X  %X\n", i, *result);
        }

        return 0;
}


static int shell_cmd_version(int argc, char *argv[])
{
	uint32_t version = sys_kernel_version_get();

	printk("Zephyr version %d.%d.%d\n",
	       SYS_KERNEL_VER_MAJOR(version),
	       SYS_KERNEL_VER_MINOR(version),
	       SYS_KERNEL_VER_PATCHLEVEL(version));
	return 0;
}

static int shell_cmd_uptime(int argc, char *argv[])
{
	printk("uptime: %u ms\n", k_uptime_get_32());

	return 0;
}

static int shell_cmd_cycles(int argc, char *argv[])
{
	printk("cycles: %u hw cycles\n", k_cycle_get_32());

	return 0;
}


struct shell_cmd kernel_commands[] = {
	{ "version", shell_cmd_version, "show kernel version" },
	{ "uptime", shell_cmd_uptime, "show system uptime in milliseconds" },
	{ "cycles", shell_cmd_cycles, "show system hardware cycles" },
        { "read", shell_cmd_read, "display a word of memory" },
        { "write", shell_cmd_write, "modify a word of memory" },
        { "dump", shell_cmd_dump, " output a memory range" },
	{ NULL, NULL }
};


SHELL_REGISTER(SHELL_KERNEL, kernel_commands);
