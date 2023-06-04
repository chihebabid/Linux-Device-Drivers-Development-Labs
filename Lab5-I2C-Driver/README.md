Program implementing a driver to manage a PCF8574 I2C LCD connected to a Raspberry Pi 3.
One has to add the following entries into the `arch/arm/boot/dts/bcm2710-rpi-3-b.dts` within the `&i2c1` node as follows:

```
&i2c1 {
	pinctrl-names = "default";
	pinctrl-0 = <&i2c1_pins>;
	clock-frequency = <100000>;
	lcd2x16@27 {
		compatible = "training,lcd2x16";
		reg = <0x27>;
	};
};
```

In user space, the string to be displayed has to be sent to misc device file `/dev/lcd2x16`


