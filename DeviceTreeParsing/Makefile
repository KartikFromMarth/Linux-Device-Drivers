obj-m += DeviceTreeProbe.o

all: module dt
	echo Builded Device Tree Overlay and kernel module

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

dt: DeviceTreeOverlay.dts
	dtc -@ -I dts -O dtb -o DeviceTreeOverlay.dtbo DeviceTreeOverlay.dts

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf DeviceTreeOverlay.dtbo
