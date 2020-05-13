# Integration using I2C

Default I2C Address: 0x10

## I2C Command Register Map

see [Pyr0-Piezo Parameters Explanation](../pyr0piezo-parameters/pyr0piezo-parameters.md) for more specific details on the command aliases.

|I2C Register|Command Alias|Arguments|Type|Bytes Length
|---|---|---|---|---|
|0x00|GAIN_F|Gain Factor|Input|2
|0x01|VFOL|Millivolts|Input|4
|0x02|VCOMP|Millivolts|Input|4
|0x03|LOOP_D|Milliseconds|Input|2
|0x04|TRG_D|Milliseconds|Input|2
|0x05|HYST|Millivolts|Input|2
|0x06|LOGIC|Boolean|Input|2
|0x07|PZDET|Boolean|Input|2
|0x08|CONST|Millivolts|Input|4
|0x09|CONFIG|None|Output|32
|0x0a|ERASE|None|Input|1
|0x0b|STATE|None|Output|10
|0x0c|VCCSW|Boolean|Input|1
|0x0d|VCCADJUST|Millivolts|Input|4
