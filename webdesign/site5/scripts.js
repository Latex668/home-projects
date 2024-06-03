function exponentiate(number, exponent){
    let result = 1;
    if(exponent == 0){
        return 1;
    }
    for(let i = 1; i <= exponent; i++){
        result = number * result;
    }
    return result;
}

function fibonacci(times){
    let num0 = 0;
    let num1 = 1;
    for(let i = 1; i < times; i++){
        result = num0 + num1;
        num0 = num1;
        num1 = result;  
    }
    return result;
}
let fibvalue = document.getElementById('infib').value;
let exponent = document.getElementById('inexp').value;
document.getElementById('p1').innerHTML = fibonacci(fibvalue);
document.getElementById('p2').innerHTML = exponentiate(2, exponent);