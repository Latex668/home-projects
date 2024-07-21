let current = document.getElementById('Current').value;
let voltage = document.getElementById('Voltage').value;
let resistance = document.getElementById('Resistance').value;
let RChanged = 0;



function ohmsLaw(voltage, current, resistance, RChanged, IChanged, VChanged){
    if(RChanged>2){
        RChanged=0;
    }
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

function changeUnit(RUnit, IUnit, VUnit){
    if(RUnit){
        console.log("Changed R unit");
        console.log(RChanged);
        switch(RChanged){
            case 0:
                RChanged=1;
                document.getElementById("RUnit").innerHTML = "K ohms";
                break;
            case 1:
                RChanged=2;
                document.getElementById("RUnit").innerHTML = "M ohms";
                break;
            case 2:
                RChanged=0;
                document.getElementById("RUnit").innerHTML = "Ohms";
                break;


        }
    }
}

ohmsLaw(voltage,current,resistance);
Power(voltage,current,resistance);
