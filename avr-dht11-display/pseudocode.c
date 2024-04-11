

wait_for_40us() {
	timer_setup(40us);
	every_3rd_call {
		every_interval();
	}
}

do_acquire() {
	wait_for_data_lo(); // may be unnec
	wait_for_data_hi(); // 80us lo
	wait_for_data_lo(); // 80us hi
	wait_for_data_hi(); // first 50us lo

	for (int i = 0; i < 40; ++i) {
		wait_for_40us();
		if (data is lo) {
			shift 0 into sensor_data
			wait_for_30us();
		} else {
			shift 1 into sensor_data
			wait_for_70us();
		}
		wait_for_data_hi();
	}

	read temperature from sensor_data;
	acquire_state = STATE_IDLE;

	timer_normal();
}





MODE_ACQUIRE
MODE_SET

STATE_IDLE
STATE_WAKE
STATE_DATA

char mode = MODE_ACQUIRE;
char acquire_state = STATE_IDLE;

uint16_t acquire_ctr = 4000;

uint8_t sensor_data[5];

main() {
	setup();

	for (;;) {
		every_interval();
		wait_for_timer();
	}
}


every_interval() {
	switch (mode) {
	case MODE_ACQUIRE:
		show_digits(temperature);
		break;
	case MODE_SET:
		show_digits(threshold);
		break;
	}

	debounce(rpg_a);
	debounce(rpg_b);
	debounce(btn);

	if (acquire_state != STATE_DATA && !--acquire_ctr) {
		switch (acquire_state) {
		case STATE_IDLE:
			acquire_ctr = 4 * 18; // 18 ms
			acquire_state = STATE_WAKE;
			pull data pin lo
			break;
		case STATE_WAKE:
			acquire_ctr = 4000;
			acquire_state = STATE_DATA;
			release data pin
			do_acquire();
			break;
		}
	}
}


rpg_ccw() {
	if (mode == MODE_SET)
		--threshold;
}

rpg_cw() {
	if (mode == MODE_SET)
		++threshold;
}


btn_changed() {
	if (pressed) {
		switch (mode) {
		case MODE_ACQUIRE:
			mode = MODE_SET;
			break;
		case MODE_SET:
			mode = MODE_ACQUIRE;
			break;
		}
	}
}