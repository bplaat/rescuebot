# RescueBot
Technische Informatica - Project 2 (RescueBot) - Group 5

## Slave to Master protocol
Values to receive from nano:

- `distance_to_ground` represents the distance to the ground in cm.
- `distance_to_object` represents the distance the the object in front of the car  in cm.
- `ir_left` the value of the sensor on the left site as boolean.
- `ir_right` the value of the sensor on the right site as boolean.

```
struct slave_to_master {
    uint8_t distance_to_ground;
    uint8_t distance_to_object;
    uint8_t ir_left;
    uint8_t ir_right;
};
```
