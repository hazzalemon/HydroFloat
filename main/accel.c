
#include "esp_system.h"
#include "driver/i2c_master.h"

void i2c_update_reg(i2c_master_dev_handle_t accelo, int address, int value){
    uint8_t buf[1];
    buf[0] = address;
    buf[1] = value;
    // printf("write called! %u, %u\n", buf[0], buf[1]);
    i2c_master_transmit(accelo, buf, 2, -1);
}

void i2c_read_regs(i2c_master_dev_handle_t accelo, int address, int read_len, uint8_t *read_buffer){
    uint8_t buf;
    buf = address;
    // printf("read called! %u, %u\n", buf[0], buf[1]);
    if (read_len > sizeof(read_buffer)){
        read_len = sizeof(read_buffer);
    }
    i2c_master_transmit_receive(accelo, &buf, 1, read_buffer, read_len, -1);
}

void update_accel(i2c_master_dev_handle_t accelo, int16_t* X, int16_t* Y, int16_t* Z, double* temp){
    uint8_t raw_vals[1];
    i2c_read_regs(accelo, 59, 2, raw_vals);
    *X = (raw_vals[0] << 8) | raw_vals[1];
    i2c_read_regs(accelo, 61, 2, raw_vals);
    *Y = (raw_vals[0] << 8) | raw_vals[1];
    i2c_read_regs(accelo, 63, 2, raw_vals);
    *Z = (raw_vals[0] << 8) | raw_vals[1];
    i2c_read_regs(accelo, 65, 2, raw_vals);

    int temp_temp;
    temp_temp = (raw_vals[0] << 8) | raw_vals[1];
    *temp = (temp_temp/326.8);


}

void setup_accel(i2c_master_dev_handle_t accelo){
    // first byte of write_buff is reg_address, second byte is value
    i2c_update_reg(accelo, 25, 0b00000001);
    i2c_update_reg(accelo, 26, 0b00000110);
    i2c_update_reg(accelo, 27, 0b00000000);
    // // printf("done 2\n");
    i2c_update_reg(accelo, 28, 0b00000000);
    // // printf("done 3\n");
    i2c_update_reg(accelo, 29, 0b00110110);
    // // printf("done 4\n");
    i2c_update_reg(accelo, 30, 0b00000000);
    // // printf("done 5\n");
    i2c_update_reg(accelo, 35, 0b00011000);
    // // printf("done 6\n");
    i2c_update_reg(accelo, 104, 0b00000011);
    i2c_update_reg(accelo, 105, 0b01000010);
    i2c_update_reg(accelo, 106, 0b01000000);
    i2c_update_reg(accelo, 107, 0b00000000);
    i2c_update_reg(accelo, 108, 0b00000000);
    // printf("done 7, returning\n");
}

void init_accel_i2c(i2c_master_dev_handle_t* accelo){
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 1,
        .scl_io_num = 10,
        .sda_io_num = 11,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x68,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, accelo));
    return;
}