# Table of truth

| name              | type     | Updatable | topic                      | Comment                    |
|-------------------|----------|-----------|----------------------------|----------------------------|
| Command           | ANY      | true      | ./CMD                      | used to update ./control#           
| Data              | ANY      | true      | ./DATA                     | used to update ./#   
|
| Reboot            | Boolean  | true      | ./control/reboot           |
| Rebirth           | Boolean  | true      | ./control/rebirth          |
| Scan rate         | UInt16   | true      | ./control/scanRate         |
|
| Hardware Model    | Text     | false     | ./system/hardwareModel     |
| Hardware Version  | UInt8    | false     | ./system/hardwareVersion   |
| Software Version  | UInt32   | false     | ./system/softwareVersion   |
| Core Voltage      | Float    | false     | ./system/coreVoltage       |
| Core Temperature  | Float    | false     | ./system/coreTemperature   |

 Configuration Change Count

{
"timestamp": 1486144502122,
"metrics": [{
       "name": "Node Control/Rebirth",
       "dataType": "Boolean",
       "value": true
}]
}