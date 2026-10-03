CONFIG_DEFAULTS = sdkconfig.defaults
CONFIG_DEBUG = $(CONFIG_DEFAULTS);sdkconfig.debug

all: rndl

rndl:
	idf.py build

debug-rndl:
	idf.py build -DSDKCONFIG_DEFAULTS="$(CONFIG_DEBUG)"

flash:
	idf.py flash
	idf.py monitor

clean:
	idf.py clean
	rm -f sdkconfig sdkconfig.old

.PHONY: rndl debug-rndl clean
