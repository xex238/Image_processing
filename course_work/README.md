## Курсовая работа.

Студент: Новицкий Д. А.

Преподаватель: Полевой Д. В.

Дата: 30.04.20

URL --> [https://github.com/xex238/Image_processing/tree/master/course_work]

### Цель:

Написать программу, с помощью которой можно оценить возможность применения индекса структурного сходства SSIM для оценки качества совмещения изображений для датасета MIDV-500.

### Входные данные:

- Путь к эталонному изображению.
- Путь к директории, в которой хранятся изображения, для которых необходимо определить значение сходства с эталонной моделью.

### Выходные данные:

- Файл формата txt «**SSIM_results.txt**», в котором построчно содержатся числовые значения сходства набора изображений для оценки с эталонным изображением.
- Файл формата txt «**SSIM_results_distorted.txt**», в котором содержатся числовые значения сходства набора дополнительно искажённых с помощью «шума» изображений для оценки с эталонным изображением.
- Результат поэлементного сравнения двух вышеперечисленных текстовых файлов в виде **сообщения в консоли**.

### Результаты

![image_1](https://github.com/xex238/Image_processing/tree/master/course_work/images/SSIM_results.jpg)
Рис. 1. Файл "SSIM_results.txt"

![image_2](https://github.com/xex238/Image_processing/tree/master/course_work/images/SSIM_results_distorted.jpg)
Рис. 2. Файл "SSIM_results_distorted.txt"

![image_3](https://github.com/xex238/Image_processing/tree/master/course_work/images/Console_message.jpg)
Рис. 3. Сообщение в консоли по завершении работы программы

![image_4](https://github.com/xex238/Image_processing/tree/master/course_work/images/Example_without_noise.jpg)
Рис. 4. Пример неискажённого изображения

![image_5](https://github.com/xex238/Image_processing/tree/master/course_work/images/Example_with_noise.jpg)
Рис. 5. Пример искажённого изображения

