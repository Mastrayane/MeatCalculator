# MeatCalculator

## Описание проекта
Это приложение для Android архитектуры arm64_v8a было создано, как мой первый опыт разработки посредством Qt для моей супруги.

Его задача разделить общую массу продeкта на 4 части.

Задача тривиальна, но у моего "заказчика" к этому был свой подход, который он попросил реализовать:

Заказчик привык не просто разделить общую массу на 4, чтобы полученный результат отвешивать поочередно в 4 пустые тары, а взвесвив общую массу тары с продуктом, оставить эту тару на весах, и из неё минусовать полученную 1/4 часть.

  Подход необычен, но в этом есть оптимальная логика, т. к. это сокращает количество физических действий. 4 тары могут в пустом виде иметь разный вес, и мало того, что их нужно переставить на весах, так ещё и каждый раз обнулять на весах вес пустой тары. В то время как поставив тару один раз, обнулив её вес и взвесив общую массу продукта, остаётся только минусовать эту общую массу до определенного значения.

Данный калькулятор предоставляет эти три значения для трех итерраций, дающих по итогу 4 равные порции продукта.
Причём, на тот случай, если на весах нет функции обнулить массу тары, он предоставляет возможность обработать это, введя массу пустой тары.

## Особенности реализации
- Меня не удовлетворил дизайн стандартных шрифтов в Qt и я все надписи реализовал сначала в Adobe Photoshop, но потом поняв, что качество png даёт потерю чёткости перерисовал в Adobe Illustrator некоторые надписи в формате svg.
  
- Дизайн кнопки так же реализован в Adobe Photoshop.
  
- Для меня показалось недостаточным, что нажатие на кнопку просто выдаёт мгновенный результат. Помимо эстетического фактора, мгновенная выдача результата, ощущалась несколько ***"мёртвой"***. В данном контексте, я предположил, что пользователю будет приятнее, если приложение ***создаст некоторую имитацию "живого", взяв очень короткую паузу на "раздумья", но при этом заполнив её каким-то откликом теплоты и заботы о пользователе***.
  Для этой цели в Adobe Photoshop мною были созданы несколько кадров плывущих подобно облакам бледно-розовых сердец, на белом фоне, и програмно реализована между ними плавная анимация перехода в момент нажатия кнопки.

- Рализован графический эффект виньетирования фона.

- Оптимизация загрузки ресурсов для мобильных устройств.
  

## Технологический стек:

- Qt 6 (Core, GUI, Widgets, SVG, Concurrent)
- C++17
- QMake система сборки
- QGraphicsEffect для кастомных эффектов
- QtConcurrent для многопоточной загрузки ресурсов

## Системные требования и установка

### Требования:
- Qt 6.0 или выше
- Комплект разработки Qt для Android
- Android NDK (версия, совместимая с Qt)
- Android SDK
- JDK 11 или выше

### Архитектуры:
Поддерживаемые архитектуры: arm64-v8a

### Минимальная версия Android:
5.0 Lollipop (API 21)

## Скачать приложение

[![Latest Release](https://img.shields.io/github/v/release/Mastrayane/MeatCalculator?label=Download%20APK&style=for-the-badge)](https://github.com/Mastrayane/MeatCalculator/releases/latest/download/MeatCalculator.apk)
  
