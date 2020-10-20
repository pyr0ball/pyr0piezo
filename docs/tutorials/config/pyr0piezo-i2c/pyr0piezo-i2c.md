# Integration using I2C

Default I2C Address: **0x10**

## I2C Command Register Map

see [Pyr0-Piezo Parameters Explanation](../pyr0piezo-parameters/pyr0piezo-parameters.md) for more specific details on the command aliases.

|I2C Register|Command Alias|Arguments|Type|Bytes Length
|---|---|---|---|---|
|0x00|GAIN_F|Gain Factor|Input|2
|0x01|VFOL|Millivolts|Input|2
|0x02|VCOMP|Millivolts|Input|2
|0x03|LOOP_D|Milliseconds|Input|2
|0x04|TRG_D|Milliseconds|Input|2
|0x05|HYST|Millivolts|Input|2
|0x06|LOGIC|Boolean|Input|2
|0x07|PZDET|Boolean|Input|2
|0x08|CONST|Millivolts|Input|4
|0x09|CONFIG|None|Output|32
|0x0a|ERASE|None|Input|1
|0x0b|STATE|None|Output|10
|0x0c|SIGVOL|Boolean|Input|1
|0x0d|VCCADJUST|Millivolts|Input|2

## I2C Command Syntax

I2C input syntax uses "Least Significant Byte First" formatting, which means the data bytes are sent in reverse order:

```serial
[Address Byte] [Command Byte] [Data 8] ... [Data 0]
```

For example, to set the Logic output to Active Low:

```serial
0x10 0x06 0x00
```

Or to set VCOMP to 2650:

```serial
0x10 0x02 0x5A 0x0A
```