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

-	Файл формата txt «SSIM_results.txt», в котором построчно содержатся числовые значения сходства набора изображений для оценки с эталонным изображением.
-	Файл формата txt «SSIM_results_{X}.txt», где X – уровень искажения изображения, в котором содержатся числовые значения сходства набора дополнительно искажённых с помощью «шума» изображений для оценки с эталонным изображением. В данном случае значения X = 3, 6, 9.
-	Файлы формата txt «SSIM_results_image_{X}», где X – порядковый номер обрабатываемого изображения, которые содержат значения SSIM при увеличении искажения изображения. В данном случае X = [2; 5].
- Файл формата txt «r», в котором хранится значение коэффициента корреляции.

### Результаты

![image_1](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_images_1.jpg)
Рис. 1. Графики зависимости значения SSIM от уровня искажения изображения (документ 01_alb_id)

![image_2](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_images_2.jpg)
Рис. 2. Графики зависимости значения SSIM от уровня искажения изображения (документ 02_aut_drvlic_new)

![image_3](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_images_3.jpg)
Рис. 3. Графики зависимости значения SSIM от уровня искажения изображения (документ 03_aut_drvlic_new)

![image_4](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_differents_1.jpg)
Рис. 4. Графики зависимости значения SSIM от исходного изображения (документ 01_alb_id)

![image_5](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_differents_2.jpg)
Рис. 5. Графики зависимости значения SSIM от исходного изображения (документ 02_aut_drvlic_new)

![image_6](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_differents_3.jpg)
Рис. 6. Графики зависимости значения SSIM от исходного изображения (документ 03_aut_id_old)

![image_7](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_differents_4.jpg)
Рис. 7. Графики зависимости значения SSIM от исходного изображения (документ 04_aut_id)

![image_8](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_differents_5.jpg)
Рис. 8. Графики зависимости значения SSIM от исходного изображения (документ 05_aze_passport)

![image_9](https://github.com/xex238/Image_processing/blob/master/course_work/images/SSIM_differents_6.jpg)
Рис. 9. Графики зависимости значения SSIM от исходного изображения (документ 06_bra_passport)
