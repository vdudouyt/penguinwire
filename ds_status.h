struct ds_status {
	uint8_t			enable;
	uint8_t			speed;
	uint8_t			pullup_dur;
	uint8_t			ppuls_dur;
	uint8_t			pulldown_slew;
	uint8_t			write1_time;
	uint8_t			write0_time;
	uint8_t			reserved0;
	uint8_t			status;
	uint8_t			command0;
	uint8_t			command1;
	uint8_t			command_buffer_status;
	uint8_t			data_out_buffer_status;
	uint8_t			data_in_buffer_status;
	uint8_t			reserved1;
	uint8_t			reserved2;
};
