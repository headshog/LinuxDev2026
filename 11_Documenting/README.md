# Документирование программы угадывания чисел

Основа — программа из `08_I18n`. Добавлены `-r` / `--roman`, английский
и русский `--help`, `--version`, две функции преобразования римских чисел
и Doxygen-комментарии к функциям, макросам и глобальным таблицам.
Выполнен необязательный пункт: диапазон расширен до **1–3999**.

В режиме `-r` римскими цифрами отображаются границы, вопросы и результат.
Ответы остаются `Yes` / `No` или `Да` / `Нет` в зависимости от языка.
Само число программа не запрашивает: `roman_to_int` предоставлен и
проверяется отдельно, а игра использует `int_to_roman` для вывода.
Разрешены только канонические прописные римские числа: например,
`IV` допустимо, а `IIII`, `IC`, `iv` — нет.

## Окружение ALT

От root при необходимости:

```sh
apt-get update
apt-get install gcc make glibc-devel autoconf automake gettext gettext-tools doxygen help2man graphviz fonts-ttf-freefont python3 glibc-locales rpm-build
```

## Сборка и запуск

В `11_Documenting`, от обычного пользователя:

```sh
autoreconf -fi
mkdir -p build
cd build
../configure --prefix="$PWD/local"
make -j4
make check
make install
```

```sh
LC_ALL=C LANGUAGE=C ./local/bin/guess-number --help
LC_ALL=ru_RU.UTF-8 LANGUAGE=ru ./local/bin/guess-number --help
LC_ALL=ru_RU.UTF-8 LANGUAGE=ru ./local/bin/guess-number -r
TEST_LANGUAGE=ru GUESS_NUMBER="$PWD/local/bin/guess-number" python3 ../test.py
```

Перевод проверяется у установленного бинарника: путь к каталогам gettext
задаётся при `configure`. Для русского языка нужна локаль `ru_RU.utf8`.
Неверный ответ повторяет вопрос, EOF завершает игру с ошибкой.
Для любого числа достаточно не более 12 вопросов.

## Документация

Обычный `make` автоматически создаёт:

- `mainpage.dox` из английского `guess-number --help-md`;
- `docs/html/index.html` и API-страницы из комментариев с помощью Doxygen;
- `guess-number.1` из английских `--help` и `--version` с помощью help2man.

Титульная страница и man не содержат вручную скопированной справки:
они используют одну функцию `print_help`. `--help-md` принудительно
выбирает английский язык независимо от текущей локали.
Диаграммы вызовов генерируются Graphviz. Предупреждения Doxygen
считаются ошибками сборки.

Просмотр из каталога `build`:

```sh
man -l guess-number.1
python3 -m http.server --bind 127.0.0.1 --directory docs/html 8000
```

При просмотре с Mac порт можно пробросить отдельной SSH-командой:

```sh
ssh -N -L 8000:127.0.0.1:8000 headshog@192.168.64.13
```

Затем открыть `http://127.0.0.1:8000` в браузере. Сервер нужно запускать
отдельно; при сборке он автоматически не запускается.

`make check` проверяет все 3999 значений обеих функций по независимой
таблице разрядов, ошибочные римские записи и границы буфера. Отдельно
проверяются оба игровых режима, границы диапазона, помощь, ошибки и EOF.

## Пакет

Из `build`:

```sh
make srpm
```

В корне исходников появится `guess-number-0.2.0-alt1.src.rpm`.
Пакет содержит программу, русский `.mo`, английскую man-страницу и
сгенерированный HTML в `/usr/share/doc/guess-number-0.2.0/html/`.
Генерация документации выполняется при сборке RPM. Тесты, `.po`
и промежуточные `.dox` в бинарный пакет не устанавливаются.
`License: Other` указан, потому что лицензия проекта не объявлена.

При настроенном hasher, из корня исходников:

```sh
mkdir -p "$HOME/hasher-documenting"
hsh --lazy-cleanup --mountpoints=/proc \
    --no-sisyphus-check=gpg,packager,changelog \
    "$HOME/hasher-documenting" "$PWD/guess-number-0.2.0-alt1.src.rpm"
```

Исключения относятся к локальной подписи и адресу автора, не являющемуся
адресом ALT Team. Проверки сборки, зависимостей и тесты сохраняются.
