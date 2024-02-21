#ifndef __DUINODE_V3_H__
#define __DUINODE_V3_H__

/**
 * @file DuinodeV1.h
 *
 * Contains hardware definitions for RF Duinode V1 (3V3)
 */

#define PINS_DEFINED 1 
#define __PLATFORM__ "RF Duinode V1 (3V3)"

const int rf_irq = 0;

const int led_red = 3;
const int led_yellow = 4;
const int led_green = 5;
const int button_a = 6;

const int rf_ce = 8;
const int rf_csn = 7;

const int temp_pin = 2; // analog
const int voltage_pin = 3; // analog

#endif // __DUINODE_V3_H__
// vim:cin:ai:sts=2 sw=2 ft=cpp
