var but1 = gpio.pin_mode(18, gpio.INPUT_PULLUP) # Make button 1 and 2 pull up buttons
var but2 = gpio.pin_mode(19, gpio.INPUT_PULLUP)
var but1_State = gpio.digital_read(18)
var but1_State = gpio.digital_read(19)

while true
    if but1_State == 0
        print("Button 1 is ON")
    else
        print("Button 1 is OFF")
    end
    tasmota.delay(500)
end