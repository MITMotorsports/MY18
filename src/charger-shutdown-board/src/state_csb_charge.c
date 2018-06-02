#include "state_csb_charge.h"

can0_ChargerCommand_T charge;

void enter_csb_state_charge(void) {
  Board_Print("[CSB FSM : CHARGE] ENTERED!\n");

  // Configure charging parameters
  charge.enable         = true;
  charge.powerReference = 1000; // 10 * percentage
  charge.VoltageLimit   = 3000; // 10 * V
  charge.CurrentLimit   = 100;  // 10 * A

  // Configure initial printouts
  lcd_clear();
  lcd_set_cursor(0, 0);
  lcd_print("VOL ");
  lcd_set_cursor(0, 1);
  lcd_print("CUR ");
}

void update_csb_state_charge(void) {
  send_ChargerCommand(&charge);

  lcd_set_cursor_clearahead(4, 0, 3);
  lcd_print_num(sensor_readings.voltage, 10);
  lcd_write('V');

  lcd_set_cursor_clearahead(4, 1, 3);
  lcd_print_num(sensor_readings.current, 10);
  lcd_write('A');
}
