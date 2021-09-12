# Water_Level_UFOX_RS485

## CALLBACK SIGFOX (Custom payload config)
- s1::uint:24 s2::uint:24 s3::uint:16 s4::uint:16 reboot::bool:7 rsv1_bool::bool:6 fraude::bool:5 b5::bool:4 b4::bool:3 b3::bool:2 b2::bool:1 b1::bool:0 SumBoles::uint:8 bat::uint:8
 ## JSON DATA CALLBACK
 - POST METHOD
 - Content Type: application/json
 - { 
  "device" : "{device}",
  "s1":{customData#s1},
  "s2":{customData#s2},
  "s3":{customData#s3},
  "s4":{customData#s4},
  "reboot":{customData#reboot},
  "rsv1_bool":{customData#rsv1_bool},
  "fraude":{customData#fraude},
  "b5":{customData#b5},
  "b4":{customData#b4},
  "b3":{customData#b3},
  "b2":{customData#b2},
  "b1":{customData#b1},
  "bateria":{customData#bat},
  "timestamp":{time},
  "seqNumber": {seqNumber}
}

##  DATA
- device: ID de dispositivo (SigfoxBackend)
- s1: sensor 1 (24bits)
- s2: sensor 2 (24bits)
- s3: sensor 3 (16bits).
- s4: sensor 4 (16bits)
- reboot: bool, bandera que inidica reinicio.
- rsv1  bool, valor reservado.
- fraude: bool, bandera que indica fraude.
- b5:  bool, digital bit5.
- b4:  bool, digital bit4.
- b3:  bool, digital bit3.
- b2:  bool, digital bit2.
- b1:  bool, digital bit1.
- bateria: valor de bateria milivolts (1byte)
- timestamp: tiempo unix (SigfoxBackend)
- seqNumber: numero de sequence mensaje(SigfoxBackend)


# Esquemático

![](https://github.com/TECA-IOT/Water_Level_UFOX_RS485/blob/main/Image/esquematico%20rs485_shieldUfox_sonda_nivel_bb.png)
