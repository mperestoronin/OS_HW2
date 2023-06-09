# На 6 баллов
## Задача:
Множество процессов взаимодействуют с использованием неименованных POSIX семафоров расположенных в разделяемой
памяти. Обмен данными также ведется через разделяемую память в стандарте POSIX.

## Описание проекта:
***Структура ThreadData содержит следующие поля:***
- text - исходный текст
- encrypted_text - зашифрованный текст
- start_index и end_index - индексы начала и конца обрабатываемого участка текста
- thread_id - идентификатор потока
- semaphore - семафор для синхронизации работы потоков

***Структура ProcessManagerData содержит следующие поля:***
- num_threads - количество потоков
- thread_data - массив структур ThreadData
- semaphore - семафор для синхронизации работы потоков
Функция encrypt_text выполняет шифрование текста. Ей передается указатель на структуру ThreadData. Функция вычисляет количество символов, затем записывает их в зашифрованный текст, используя функцию sprintf. 

Функция process_manager управляет работой потоков. Ей передается указатель на структуру ProcessManagerData.

Затем в функции main вычисляется необходимое количество потоков num_threads, которое нужно для шифрования всего текста.
Потом инициализируется семафор semaphore, используемый для синхронизации между потоками. Для этого используется функция sem_init, которая принимает указатель на переменную sem_t и начальное значение. Затем цикл for создает num_threads потоков, каждый из которых получает часть текста для шифрования и выполняет функцию encrypt_text. В конце каждый поток вызывает функцию sem_post, чтобы уведомить семафор о завершении своей работы.

Затем создается структура ProcessManagerData, которая содержит информацию о количестве потоков и данных каждого потока, а также семафор, используемый для синхронизации между процессами. Затем инициализируется семафор process_manager_semaphore, используемый для синхронизации между главным потоком и процессом-менеджером. Затем создается процесс-менеджер в отдельном потоке, который вызывает функцию process_manager и передает ей данные о количестве потоков и информацию каждого потока. После создания процесс-менеджера главный поток вызывает функцию sem_wait для process_manager_semaphore, чтобы ожидать завершения всех потоков.

После того, как все потоки завершили свою работу, процесс-менеджер объединяет результаты каждого потока в одну строку и выводит ее на экран. Затем освобождаются ресурсы, выделенные для семафоров и процесс-менеджера, с помощью функций sem_destroy и munmap.
