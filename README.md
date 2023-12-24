YoRC-Chat

Описание:
YoRC-Chat — это приложение чата для локальной сети, созданное для работы в среде Windows. 
Приложение состоит из двух отдельных компонентов: клиентской и серверной частей, которые запускаются отдельно друг от друга. 
Приложение позволяет пользователям обмениваться сообщениями в реальном времени в рамках локальной сети.

Основные Компоненты:
Сервер (Server): Управляет подключениями клиентов, обрабатывает запросы и взаимодействует с базой данных MySQL для хранения данных о пользователях и сообщениях.
Клиент (Client): Позволяет пользователям подключаться к серверу, отправлять и получать сообщения, просматривать список пользователей и истории сообщений.
User: Представляет собой пользователя чата.
Message: Отвечает за структуру сообщений.

Технологии:
C++
Windows Sockets для сетевого взаимодействия
MySQL для хранения данных
Многопоточность для обработки нескольких клиентских подключений

Это промежуточная версия приложения. Подразумевается дальнейшая разработка.
