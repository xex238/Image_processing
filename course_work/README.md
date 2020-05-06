## Курсовая работа.

Студент: Новицкий Д. А.

Преподаватель: Полевой Д. В.

Дата: 06.05.20

URL --> [https://github.com/xex238/Image_processing/tree/master/course_work]

### Цель:

В данной работе необходимо оценить возможность применения индекса структурного сходства SSIM для оценки качества совмещения изображений для датасета MIDV-500.

Для достижения данной цели была разработана программа на языке программирования c++ в среде программирования Visual Studio 2019.

### Входные данные:

- Путь к эталонному изображению.
-	Путь к директории, в которой хранятся изображения, на которых присутствует целевой объект.
-	Путь к директории с .json файлами, в которых хранятся точки четырёхугольника, которые обозначают границы целевого объекта (четырёхугольника) на изображении. 

### Выходные данные:

•	Файл формата txt «SSIM_results.txt», в котором построчно содержатся числовые значения сходства набора изображений для оценки с эталонным изображением.
•	Файл формата txt «SSIM_results_{X}.txt», где X – уровень искажения изображения, в котором содержатся числовые значения сходства набора дополнительно искажённых с помощью «шума» изображений для оценки с эталонным изображением. В данном случае значения X = 20, 50, 100.
•	Файлы формата txt «SSIM_results_image_{X}», где X – порядковый номер обрабатываемого изображения, которые содержат значения SSIM при увеличении искажения изображения. В данном случае X = [1; 5].

### Результаты

![image_1](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_results.jpg)
Рис. 1. Файл "SSIM_results.txt"

![image_2](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_results_distorted.jpg)
Рис. 2. Файл "SSIM_results_distorted.txt"

![image_3](https://github.com/xex238/Image_processing/blob/master/course_work/images/Console_message.jpg)
Рис. 3. Сообщение в консоли по завершении работы программы

![image_4](https://github.com/xex238/Image_processing/blob/master/course_work/images/Example_without_noise.jpg)
Рис. 4. Пример неискажённого изображения

![image_5](https://github.com/xex238/Image_processing/blob/master/course_work/images/Example_with_noise.jpg)
Рис. 5. Пример искажённого изображения
