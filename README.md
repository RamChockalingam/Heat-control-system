# Heat control system
## Description
This is a heat control system which uses DHT22 sensor to detect the temperature and a logic to accordingly turn on or off the heater element. There are basically 5 states for the heater which are IDLE, HEATING, STABILIZED, UNDERHEAT, OVERHEAT. Also LED's are present to indicate the current state of the heater. To periodically check the temperature and act accordingly timer1 interrupt of arduino uno is used. 

## Components required:
* DHT22 temperature sensor
* Arduino uno
* Relevant heater element

## Wokwi simulation circuit
<img width="825" height="713" alt="image" src="https://github.com/user-attachments/assets/176bf69c-b887-4d1d-8618-759f401a483a" />

## Link
- [Wokwi simulation link](https://wokwi.com/projects/436723599076016129)
