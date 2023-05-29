Program implementing a platform driver that manages two virtual misc char devices. One has to add the following entries into the arch/arm/boot/dts/bcm2710-rpi-3-b.dts within the soc node:

helloplatform {
	compatible = "training,helloplatform";
	label="Instance1";
};
helloplatform {
	compatible = "training,helloplatform";
	label="Instance2";
};

