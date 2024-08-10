# Сохранение данных с акселерометра LIS3DSH на SD-карту с помощью Arduino Nano

Этот проект предназначен для сбора данных об ускорении с помощью акселерометра LIS3DSH и записи их в файл. Данные включают ускорение по осям X, Y и Z, а также временные метки.

## Подключение модулей

1. Подключение LIS3DSH:
VCC → 3V3
GND → GND
SCL → A5
SDA → A4
CS → VCC (на LIS3DSH)

2. Подключение SD (Troyka):
DI → D11
DO → D12
SCK → D13
CS → D10
V → 5V
G → GND

3. Светодиод → D2 и GND


## Алгоритм работы

1. Создаётся файл ``ACC_###.txt``, где ``###`` — следующий номер.

2. Формируется заголовок файла:
```
Acceleration data file.
Acceleration units: G
Time units: microseconds
Accelerometer: LIS3DSH
Accelerometer datarate: 100 Hz
Accelerometer range: 16G
Accelerometer perfomance: High Resolution 12bit
X;Y;Z;dt
```

3. Как можно быстрее запрашиваются данные с акселерометра и записываются в файл.

Пример строки данных:
```
0.936;0.192;0.012;14596
 G_x ; G_y ; G_z ; dt
```
Где:
``G_x`` — ускорение вдоль оси Ox в единицах ускорения свободного падения (G)
``G_y`` — ускорение вдоль оси Oy в единицах ускорения свободного падения (G)
``G_z`` — ускорение вдоль оси Oz в единицах ускорения свободного падения (G)
``dt`` — временной интервал после предыдущих данных в микросекундах
1G = 9.80665 м/с^2

## Сигнализация светодиода

Нормальная работа — мигание каждую секунду.

1. Ошибки настройки
   1. 1 мигание каждые 500 мс — Ошибка инициализации LIS3DH
   1. 2 мигания каждые 500 мс — Ошибка инициализации SD
   1. 3 мигания каждые 500 мс — Ошибка создания файла
   1. 4 мигания каждые 500 мс — Ошибка записи заголовка файла
1. Ошибки в цикле
   1. 10 миганий каждые 500 мс — Файл поврежден

При любой ошибке индикация в бесконечном цикле. Требуется перезагрузка.
Для получения дополнительной информации смотрите Serial на 115200 бод.