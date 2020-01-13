# RescueBot
Technische Informatica - Project 2 (RescueBot) - Group 5

## Slave to Master protocol
Values to receive from nano:

- `distance_to_object` represents the distance the the object in front of the car in cm
- `distance_to_left` represents the distance to first left object / wall in cm
- `distance_to_right` represents the distance to first right object / wall in cm
- `distance_to_ground` represents the distance to the ground in cm
- `ir_left` the value of the ir sensor on the left site as boolean (1 means border found)
- `ir_right` the value of the ir sensor on the right site as boolean (1 means border found)

```
struct slave_to_master {
    uint8_t distance_to_object;
    uint8_t distance_to_left;
    uint8_t distance_to_right;
    uint8_t distance_to_ground;
    uint8_t ir_left;
    uint8_t ir_right;
};
```
