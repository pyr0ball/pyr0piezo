# EasyPiezi Wiring Harness Pinout

## Main Connector Controller Side

The Main Connector for the EasyPiezi has 6 pins, with Pin1 marked with a small embossed arrow:

https://docs.pyroballpcbs.com/tutorials/wiring/easy-piezi/harness-pinout/
![Main Connecctor Wiring Harness Input](../../assets/images/Main-Connector-Wiring-Harness.png)

Pin numbers are laid out in the following image and table:

![Input Connector Pin Assignments](../../assets/images/Main-Connector-Wiring-Harness.png)

| Pin Number | Wire Color | Signal Name | Req/Opt |
|------------|------------|-------------|---------|
| 1          | <strong><span style="color:green">Green</span></strong>      | SDA (I2C)   | Optional|
| 2          | <strong><span style="color:yellow; background-color:black; ">Yellow</span></strong>     | SCL (I2C)   | Optional|
| 3          | <strong><span style="color:red">Red</span></strong>        | +6-38V DC   | Required|
| 4          | <strong>Black</strong>      | Ground      | Required|
| 5          | <strong><span style="color:white; background-color:black">White</strong>      | Z-Trigger   | Required|
| 6          | <strong><span style="color:blue">Blue</strong>       | PT100 Anlg  | Optional|

## Main Connector Sensor Side

The EasyPiezi's Main Connector Pinout is printed on the bottom of the board:

![EasyPiezi Board Main Connector Pinout](../../assets/images/Main-Connector-Pinout.png)

They are arranged from bottom to top:

| Pin Number | Wire Color | Signal Name | Req/Opt |
|------------|------------|-------------|---------|
| 1          | <strong><span style="color:green">Green</span></strong>      | SDA (I2C)   | Optional|
| 2          | <strong><span style="color:yellow; background-color:black; ">Yellow</span></strong>     | SCL (I2C)   | Optional|
| 3          | <strong>Black</strong>      | Ground      | Required|
| 4          | <strong><span style="color:red">Red</span></strong>        | +6-38V DC   | Required|
| 5          | <strong><span style="color:white; background-color:black">White</strong>      | Z-Trigger   | Required|
| 6          | <strong><span style="color:blue">Blue</strong>       | PT100 Anlg  | Optional|