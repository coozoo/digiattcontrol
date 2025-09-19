# digiattcontrol

**digiattcontrol** is a Qt-based tool for controlling digital Chinese RF attenuator devices via a serial port. It provides a cross-platform GUI and API for setting and reading attenuation values, as well as automatic device type detection so all devices suppored by one app.
It was created becuse I need widet type like to integrate it with RF power metere and default crappy dotnet app is hosted only on baidu where you need to have personalized account in order to download it sure it's not good to share your private data with chinese service even and more for each device you need separate app.

<img width="762" height="290" alt="image" src="https://github.com/user-attachments/assets/a1c70af4-8e43-45df-94b9-c98bba6b2c2f" />


## Supported Devices

Device looks like this

<img width="1000" height="1000" alt="image" src="https://github.com/user-attachments/assets/9e80430d-48e9-46fd-9a0a-7f5667c377d5" />


The following digital attenuator models are supported:

- **DC-6GHZ-120DB**
  - Step: 0.25 dB
  - Max: 124.75 dB
  - Format: `att-000.00\r\n`
- **DC-6GHZ-90DB-V3**
  - Step: 0.25 dB
  - Max: 95.25 dB
  - Format: `att-000.00\r\n`
- **DC-3G-90DB-V2**
  - Step: 0.5 dB
  - Max: 94.5 dB
  - Format: `att-000.00\r\n`
- **DC-6GHZ-30DB**
  - Step: 0.25 dB
  - Max: 31.75 dB
  - Format: `att-00.00\r\n`
- **DC-8GHZ-30DB-0.1DB**
  - Step: 0.1 dB
  - Max: 30.0 dB
  - Format: `att-00.00\r\n`
  
Current value for all of the devices can be requested with ```READ\r\n```

## Features

- Automatic device type probing and recognition
- Set and read attenuation values
- Serial port communication with configurable parameters
- Qt signals/slots for easy integration

## License

MIT License
