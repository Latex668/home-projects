let current = document.getElementById('Current').value;
let voltage = document.getElementById('Voltage').value;
let resistance = document.getElementById('Resistance').value;

function ohmsLaw(voltage, current, resistance){
    if(resistance == 0){
        return document.getElementById('Resistance').value = voltage/current;
    }else if(voltage == 0){
        return document.getElementById('Voltage').value = resistance*current;
    }else if(current == 0){
        return document.getElementById('Current').value = voltage/resistance;
    }
}

function Power(voltage, current, resistance){
    if(resistance == 0){
        return document.getElementById('Power').value = voltage * current;
    }else if(current == 0){
        return document.getElementById('Power').value = (voltage * voltage)/resistance;
    }else{
        return document.getElementById('Power').value = current * current * resistance;
    }
}

ohmsLaw(voltage,current,resistance);
Power(voltage,current,resistance);
