# RemoteControltoMQTT
Receive 433 MHz remote control signals on an ESP8266 and send code to MQTT to decide what to do with it. 

It's pretty simple. Get the 433 MHz Radio reciever of your choice, I bought mine from Aliexpress, and connect the data out pin of the receiver to the data in pin(D6 on a Wemos D1 Mini in my code here) and change the settings in the code section "USER CONFIG" to match your local setup. 

That will send any incoming remote control keypresses to your MQTT server and set the default topi "RadioTrans" to the incoming code. 

You can then use whatever method you like to then decide what to do when any given code comes in. 

![image](https://user-images.githubusercontent.com/1443724/144650647-e82292b8-ab5f-4fba-9774-cbfd8cc4b1d2.png)

For example, my setup uses Node-Red. I have a flow set up to monitor the incoming MQTT topic and use a switch node to decide what to do with each code.

![image](https://user-images.githubusercontent.com/1443724/144650829-97e17943-2d36-4a76-82f9-73506b6d2710.png)


When the code "3405409" comes in when I push a fob button, I can then use it to set another node to output to a switch or trigger an event or send a web request or turn on or off a light. The possibilities are endless. 

This is just my simple implementation of this process. Others have done it and it has been integrated into some of the home automation packages. 

