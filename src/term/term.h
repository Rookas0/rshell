#pragma once
#define _GNU_SOURCE
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

void disable_raw_mode(void);
void enable_raw_mode(void);
