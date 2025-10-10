#ifndef INCLUDE_CAN_H_
#define INCLUDE_CAN_H_

struct can_msg{
    uint8_t adress;
    uint8_t num_df;
    uint8_t data;
};

int8_t CAN_init(void);
int8_t CAN_read(void);
int8_t CAN_write(void);
int8_t CAN_request_to_send(void);
int8_t CAN_bit_modify(void);
int8_t CAN_reset(void);
int8_t CAN_read_status(void);



#endif // INCLUDE_CAN_H_