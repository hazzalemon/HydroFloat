#ifndef ACCEL_H
#define ACCEL_H

void i2c_update_reg(i2c_master_dev_handle_t accelo, int address, int value);

void i2c_read_regs(i2c_master_dev_handle_t accelo, int address, int read_len, uint8_t *read_buffer);

void update_accel(i2c_master_dev_handle_t accelo, int* X, int* Y, int* Z, double* temp);

void setup_accel(i2c_master_dev_handle_t accelo);

void init_accel_i2c(i2c_master_dev_handle_t* accelo);

#endif