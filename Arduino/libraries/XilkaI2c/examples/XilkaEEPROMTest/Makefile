TTY = /dev/arduino
ARDUINO = /opt/arduino16/arduino
SRC = XilkaEEPROMTest.ino

default:
	@echo "make protrinket5|protrinket3|due|protrinket5-upload|protrinket3-upload:due-upload"

protrinket5:
	$(ARDUINO) --board adafruit:avr:protrinket:volts=five,cable=ftdi --verify --verbose $(SRC)

protrinket5-upload:
	$(ARDUINO) --board adafruit:avr:protrinket:volts=five,cable=ftdi --port $(TTY) --upload --verbose $(SRC)

protrinket3:
	$(ARDUINO) --board adafruit:avr:protrinket:volts=three,cable=ftdi --verify --verbose $(SRC)

protrinket3-upload:
	$(ARDUINO) --board adafruit:avr:protrinket:volts=three,cable=ftdi --port $(TTY) --upload --verbose $(SRC)

trinket5:
	$(ARDUINO) --board adafruit:avr:trinket:volts=five --verify --verbose $(SRC)

trinket5-upload:
	$(ARDUINO) --board adafruit:avr:trinket:volts=five --port $(TTY) --upload --verbose $(SRC)

trinket3:
	$(ARDUINO) --board adafruit:avr:trinket:volts=three --verify --verbose $(SRC)

trinket3-upload:
	$(ARDUINO) --board adafruit:avr:trinket:volts=three --port $(TTY) --upload --verbose $(SRC)

due:
	$(ARDUINO) --board arduino:sam:arduino_due_x_dbg --verify --verbose $(SRC)

due-upload:
	$(ARDUINO) --board arduino:sam:arduino_due_x_dbg --port $(TTY) --upload --verbose $(SRC)

