# OS_HW2
Выполнин Пересторонин Максим Дмитриевич БПИ 217 на оценку 6

***Вариант 33***
```Пляшущие человечки. На тайном собрании глав преступного
мира города Лондона председатель собрания профессор Мориарти
постановил: отныне вся переписка между преступниками должна
вестись тайнописью. В качестве стандарта были выбраны «пляшущие человечки», шифр, в котором каждой букве латинского алфавита соответствует хитроумный значок. Реализовать приложение, шифрующее исходный текст (в качестве ключа
используется кодовая таблица, устанавливающая однозначное соответствие между каждой буквой и какимнибудь числом). Каждый процесс–шифровальщик шифрует свои
кусочки текста, многократно получаемые от менеджера. Распределение фрагментов текста между процессами–шифровальщиками
осуществляется процессом–менеджером, который передает каждому процессу участок текста, получает от него результат, передает
следующий незашифрованный фрагмент. Он же собирает из отдельных фрагментов зашифрованный текст. Количество процессов
задается опционально. Каждый процесс может выполнять свою работу за случайное время.
```
### Про шифрование
Для упрощения написания и проверки алгоритма шифрования текста (как я считаю, он в этой задаче не главный) я решил сопоставить каждой букве английского алфавита ее ASCII значение. Для упрощения проверки кодирования, ASCII числа выводятся через пробел. ASCII таблицу прилагаю:
![](https://upload.wikimedia.org/wikipedia/commons/1/1b/ASCII-Table-wide.svg)
