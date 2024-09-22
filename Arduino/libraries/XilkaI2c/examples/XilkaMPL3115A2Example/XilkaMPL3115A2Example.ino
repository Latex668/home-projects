/*
 MPL3115A2 Barometric Pressure Sensor Library Example Code
 By: Nathan Seidle
 SparkFun Electronics
 Date: September 24th, 2013
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This example coverts pressure from Pascals to inches of mercury, altimeter setting adjusted.
 This type of pressure reading is used in the USA on Wunderground. I find it SUPER weird, but it
 is needed for home weather stations and aircraft.

 You must set the correct altitude (station_elevation_m) below. Find this altitude using
 a GPS unit such as your cell phone. Be sure to put the elevation into meters (1meter = 0.3048ft).

 Hardware Connections (Breakoutboard to Arduino):
 -VCC = 3.3V
 -SDA = A4 (use inline 10k resistor if your board is 5V)
 -SCL = A5 (use inline 10k resistor if your board is 5V)
 -INT pins can be left unconnected for this demo

	During testing, GPS with 9 satellites reported 5393ft,
	sensor reported 5360ft (delta of 33ft). Very close!

*/

#include <Wire.h>
#include <XilkaMPL3115A2.h>

using namespace Xilka;

MPL3115A2 mpl;

void setup()
{
	Serial.begin(115200);
	Serial.println("Xilka::MPL3115A2 example");

	Wire.begin();
	Wire.setClock(MPL3115A2::MaxI2CSpeed);
    delay(250);

    if ( ! mpl.begin() )
    {
        Serial.println("Couldn't find MPL3115A2 sensor!");

        while ( true )
			;
    }
	else
	{
		// mpl.setModeAltimeter(); // Measure altitude above sea level in meters
		// mpl.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa

		mpl.setOversampleRate(7); // Set Oversample to the recommended 128
		mpl.enableEventFlags(); // Enable all three pressure and temp event flags
	}
}

void loop()
{
	mpl.powerUp();
	mpl.setModeBarometer();
	delay(1000);

	float pressure(mpl.getPressureHg());

	if ( pressure != -999.0 )
	{
		Serial.print("Pressure(Hg): ");
		Serial.print(pressure, 2);
		Serial.print("\t");
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastErrorMessage());
	}

	float temp(mpl.getTempF());

	if ( temp != -999.0 )
	{
		Serial.print(" Temp(f): ");
		Serial.print(temp, 2);
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastErrorMessage());
	}

	mpl.setModeAltimeter();

	float alt(mpl.getAltitudeFeet());

	if ( alt != -999.0 )
	{
		Serial.print(" Altitude(ft): ");
		Serial.print(alt, 2);
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastErrorMessage());
	}

#if 0
	/*
	 * References:
	 * Definition of "altimeter setting": http://www.crh.noaa.gov/bou/awebphp/definitions_pressure.php
	 * Altimeter setting: http://www.srh.noaa.gov/epz/?n=wxcalc_altimetersetting
	 * Altimeter setting: http://www.srh.noaa.gov/images/epz/wxcalc/altimeterSetting.pdf
	 * Verified against Boulder, CO readings: http://www.crh.noaa.gov/bou/include/webpres.php?product=webpres.txt
	 */

	// const int station_elevation_ft = 5374; //Must be obtained with a GPS unit
	// float station_elevation_m = station_elevation_ft * 0.3048; //I'm going to hard code this
	const int station_elevation_m = 1638; //Accurate for the roof on my house
	// 1 pascal = 0.01 millibars
	pressure /= 100; //pressure is now in millibars

	float part1 = pressure - 0.3; //Part 1 of formula

	const float part2 = 8.42288 / 100000.0;
	float part3 = pow((pressure - 0.3), 0.190284);
	float part4 = (float)station_elevation_m / part3;
	float part5 = (1.0 + (part2 * part4));
	float part6 = pow(part5, (1.0/0.190284));
	float altimeter_setting_pressure_mb = part1 * part6; //Output is now in adjusted millibars
	float baroin = altimeter_setting_pressure_mb * 0.02953;

	Serial.print(" Altimeter setting InHg:");
	Serial.print(baroin, 2);
#endif

	mpl.powerDown();
	Serial.println();
}
