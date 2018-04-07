# Button Bank

Sends out the `ButtonRequest` message at the CAN spec prescribed period.
Each segment represents the state of a button.

More specifically, its value is true if the button was pressed at any moment
since the `ButtonRequest` message was last sent. Otherwise it's false.
