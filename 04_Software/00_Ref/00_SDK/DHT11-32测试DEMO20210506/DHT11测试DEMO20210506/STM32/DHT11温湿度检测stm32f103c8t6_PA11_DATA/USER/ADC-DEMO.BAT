SET PATH=D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin;D:\Program Files (tech)\stm8 stvp\asm;C:\Program Files (x86)\Common Files\Intel\Shared Files\cpp\bin\Intel64;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;D:\Program Files (tech)\HI-TECH Software\PICC\9.83\bin;D:\Program Files (tech)\Microchip\xc8\v2.05\bin;C:\Users\hp\AppData\Local\GitHubDesktop\bin
SET CPU_TYPE=STM32F103C8
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=ADC-DEMO
SET CPU_CLOCK=0x007A1200
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\startup_stm32f10x_md._ia"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\main.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\led.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\usart1.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\delay.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\dht11.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\sys.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_it.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_gpio.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_rcc.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_usart.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_adc.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_dma.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_flash.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_rtc.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\misc.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_tim.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_exti.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\core_cm3.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\system_stm32f10x.__i"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\STM32-DEMO.lnp"
"D:\Program Files (tech)\mdkKeil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\STM32-DEMO.axf" --i32combined --output ".\STM32-DEMO.hex"
