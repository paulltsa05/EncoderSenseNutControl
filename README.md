# EncoderSenseNutControl

This project uses Encoder on the Air Tool Nut tightening machine.
Encoder Pulse of ENC A, ENC B, IDX pulse are use to detect When to Stop the Air flow.
It avoid using Torque sensor to detect the end of Nut tightening.
It uses detection of Deacceleration of the running when it approaches the end of Nut Run cycle.

It also has a modbus master implementation to connect to any PLC device to further do Industrial Automation.
