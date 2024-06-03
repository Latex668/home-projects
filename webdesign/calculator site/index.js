let current = document.getElementById('Current').value;
let voltage = document.getElementById('Voltage').value;
let resistance = document.getElementById('Resistance').value;

function ohmsLaw(voltage, current, resistance){
    if(resistance == 0){
        return voltage/current;
    }else if(voltage == 0){
        return resistance*current;
    }else if(current == 0){
        return voltage/resistance;
    }
}
function Power(voltage, current, resistance){
    if(resistance == 0){
        return voltage * current;
    }else if(current == 0){
        return (voltage * voltage)/resistance;
    }else if(voltage == 0){
        return current * current * resistance;
    }
}

document.getElementById('Resistance').innerHTML = ohmsLaw(voltage, current, 0);
document.getElementById('Voltage').innerHTML = ohmsLaw(0, current, resistance);
document.getElementById('Current').innerHTML = ohmsLaw(voltage, 0, resistance);
document.getElementById('Power').innerHTML = Power(voltage, current, resistance);
