/**
 * includes.h
 *
 * Master include file for the smartest little computer.
 */

#ifndef _INCLUDES_H
#define _INCLUDES_H

// system include files

#include <cstdlib>
#include <iostream>
#include <stdio.h>

// 'arch' library

#include <Clock.h>
#include <Bus.h>
#include <StorageObject.h>
#include <Clearable.h>
#include <Counter.h>
#include <Memory.h>
#include <BusALU.h>

using namespace std;

// program-specific includes

#include "globals.h"
#include "prototypes.h"

#endif
