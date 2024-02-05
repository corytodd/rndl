CONFIG_DEBUG = sdkconfig.debug

all: rndl

rndl:
	idf.py build

debug-rndl:
	idf.py build -DSDKCONFIG_DEFAULTS=$(CONFIG_DEBUG)

flash:
	idf.py flash
	idf.py monitor

clean:
	idf.py clean
	rm sdkconfig

.PHONY: rndl debug-rndl clean
