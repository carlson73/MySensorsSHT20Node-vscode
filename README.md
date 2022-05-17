# MySensorsNode

17.05.2022

Поправил код для nRF52811.
Теперь работает корретно с родным чипом. 
Также сделал через define OLD выбор между старой V2 и новой V3 версией платы.
Рекомендуется удалить C:\Users\YOUR_User\.platformio\platforms\nordicnrf52 и C:\Users\YOUR_User\.platformio\packages\framework-arduinonordicnrf5,
чтобы они обновились при компиляции программы. Там были корректировки для nRF52811
