#include "stdafx.h"
#include "Error.h"

namespace Error {
	ERROR errors[ERROR_MAX_ENTRY] = {
		// --- 0-99 (Системные) ---
		ERROR_ENTRY(0, "Недопустимый код ошибки"),
		ERROR_ENTRY(1, "Системный сбой"),
		ERROR_ENTRY_NODEF(2), ERROR_ENTRY_NODEF(3), ERROR_ENTRY_NODEF(4),
		ERROR_ENTRY_NODEF(5), ERROR_ENTRY_NODEF(6), ERROR_ENTRY_NODEF(7), ERROR_ENTRY_NODEF(8), ERROR_ENTRY_NODEF(9),
		ERROR_ENTRY_NODEF10(10), ERROR_ENTRY_NODEF10(20), ERROR_ENTRY_NODEF10(30), ERROR_ENTRY_NODEF10(40),
		ERROR_ENTRY_NODEF10(50), ERROR_ENTRY_NODEF10(60), ERROR_ENTRY_NODEF10(70), ERROR_ENTRY_NODEF10(80), ERROR_ENTRY_NODEF10(90),

		// --- 100-199 (Параметры и Лексер) ---
		ERROR_ENTRY(100, "Параметр -in должен быть задан"),
		ERROR_ENTRY_NODEF(101), ERROR_ENTRY_NODEF(102), ERROR_ENTRY_NODEF(103),
		ERROR_ENTRY(104, "Превышена длина входного параметра"),
		ERROR_ENTRY_NODEF(105), ERROR_ENTRY_NODEF(106), ERROR_ENTRY_NODEF(107), ERROR_ENTRY_NODEF(108), ERROR_ENTRY_NODEF(109),
		ERROR_ENTRY(110, "Ошибка при открытии файла с исходным кодом (-in)"),
		ERROR_ENTRY(111, "Недопустимый символ в исходном файле (-in)"),
		ERROR_ENTRY(112, "Ошибка при создании файла протокола (-log)"),
		ERROR_ENTRY(113, "Ошибка при создании выходного файла"),
		ERROR_ENTRY(114, "Недопустимый символ (лексическая ошибка)"),
		ERROR_ENTRY(115, "Недопустимый литерал"),
		ERROR_ENTRY(116, "Ошибка в записи литерала"),
		ERROR_ENTRY(117, "Идентификатор не может начинаться с цифры"),
		ERROR_ENTRY(118, "Пустой символьный/строковый литерал"),
		ERROR_ENTRY(119, "Незакрытая строковая константа"),
		ERROR_ENTRY_NODEF10(120), ERROR_ENTRY_NODEF10(130), ERROR_ENTRY_NODEF10(140), ERROR_ENTRY_NODEF10(150),
		ERROR_ENTRY_NODEF10(160), ERROR_ENTRY_NODEF10(170), ERROR_ENTRY_NODEF10(180), ERROR_ENTRY_NODEF10(190),

		// --- 200-299 (Таблицы) ---
		ERROR_ENTRY(200, "Переполнение таблицы идентификаторов"),
		ERROR_ENTRY(201, "Идентификатор уже существует в таблице"),
		ERROR_ENTRY(202, "Идентификатор не найден в таблице"),
		ERROR_ENTRY(203, "Неверная запись идентификатора"),
		ERROR_ENTRY(204, "Превышен макс. размер таблицы ID"),
		ERROR_ENTRY(205, "Недопустимый размер таблицы ID"),
		ERROR_ENTRY(206, "Ошибка создания таблицы ID"),
		ERROR_ENTRY(207, "Попытка добавить пустой ID"),
		ERROR_ENTRY_NODEF(208), ERROR_ENTRY_NODEF(209),
		ERROR_ENTRY(210, "Переполнение лексической таблицы"),
		ERROR_ENTRY(211, "Недопустимый размер LT"),
		ERROR_ENTRY(212, "Индекс выходит за границы LT"),
		ERROR_ENTRY(213, "Неверная запись в LT"),
		ERROR_ENTRY(214, "Ошибка создания LT"),
		ERROR_ENTRY_NODEF(215), ERROR_ENTRY_NODEF(216), ERROR_ENTRY_NODEF(217), ERROR_ENTRY_NODEF(218), ERROR_ENTRY_NODEF(219),
		ERROR_ENTRY_NODEF10(220), ERROR_ENTRY_NODEF10(230), ERROR_ENTRY_NODEF10(240), ERROR_ENTRY_NODEF10(250),
		ERROR_ENTRY_NODEF10(260), ERROR_ENTRY_NODEF10(270), ERROR_ENTRY_NODEF10(280), ERROR_ENTRY_NODEF10(290),

		// --- 300-399 (Семантика - объявления) ---
		ERROR_ENTRY_NODEF(300),
		ERROR_ENTRY(301, "Идентификатор не объявлен"),
		ERROR_ENTRY(302, "Повторное объявление идентификатора"),
		ERROR_ENTRY(303, "Несоответствие типов данных"),
		ERROR_ENTRY(304, "Использование функции как переменной"),
		ERROR_ENTRY(305, "Отсутствует точка входа (entry)"),
		ERROR_ENTRY(306, "Неверное количество аргументов функции"),
		ERROR_ENTRY(307, "Оператор break вне цикла"),
		ERROR_ENTRY(308, "Функция возвращает неверный тип данных"),
		ERROR_ENTRY(309, "Деление на ноль (константное выражение)"),
		ERROR_ENTRY(310, "Дублирование точки входа (entry)"),
		ERROR_ENTRY(311, "Отсутствует тело функции или пустой блок"),
		ERROR_ENTRY_NODEF(312), ERROR_ENTRY_NODEF(313), ERROR_ENTRY_NODEF(314), ERROR_ENTRY_NODEF(315),ERROR_ENTRY_NODEF(316),ERROR_ENTRY_NODEF(317),ERROR_ENTRY_NODEF(318), ERROR_ENTRY_NODEF(319), ERROR_ENTRY_NODEF10(320), ERROR_ENTRY_NODEF10(330), ERROR_ENTRY_NODEF10(340),
		ERROR_ENTRY_NODEF10(350), ERROR_ENTRY_NODEF10(360), ERROR_ENTRY_NODEF10(370), ERROR_ENTRY_NODEF10(380), ERROR_ENTRY_NODEF10(390),

		// --- 400-599 (ПУСТОТА) ---
		// Расписываем через NODEF10, чтобы точно забить место
		ERROR_ENTRY_NODEF10(400), ERROR_ENTRY_NODEF10(410), ERROR_ENTRY_NODEF10(420), ERROR_ENTRY_NODEF10(430), ERROR_ENTRY_NODEF10(440),
		ERROR_ENTRY_NODEF10(450), ERROR_ENTRY_NODEF10(460), ERROR_ENTRY_NODEF10(470), ERROR_ENTRY_NODEF10(480), ERROR_ENTRY_NODEF10(490),
		ERROR_ENTRY_NODEF10(500), ERROR_ENTRY_NODEF10(510), ERROR_ENTRY_NODEF10(520), ERROR_ENTRY_NODEF10(530), ERROR_ENTRY_NODEF10(540),
		ERROR_ENTRY_NODEF10(550), ERROR_ENTRY_NODEF10(560), ERROR_ENTRY_NODEF10(570), ERROR_ENTRY_NODEF10(580), ERROR_ENTRY_NODEF10(590),

		// --- 600-699 (Синтаксис и Семантика вызовов) ---
		ERROR_ENTRY(600, "Неверная структура программы"),
		ERROR_ENTRY(601, "Ошибочный оператор"),
		ERROR_ENTRY(602, "Ошибка в выражении"),
		ERROR_ENTRY(603, "Ошибка в параметрах функции"),
		ERROR_ENTRY(604, "Неверный тип параметра функции"),
		ERROR_ENTRY_NODEF(605), ERROR_ENTRY_NODEF(606), ERROR_ENTRY_NODEF(607), ERROR_ENTRY_NODEF(608), ERROR_ENTRY_NODEF(609),
		ERROR_ENTRY_NODEF10(610), ERROR_ENTRY_NODEF10(620), ERROR_ENTRY_NODEF10(630), ERROR_ENTRY_NODEF10(640),
		ERROR_ENTRY_NODEF10(650), ERROR_ENTRY_NODEF10(660), ERROR_ENTRY_NODEF10(670), ERROR_ENTRY_NODEF10(680), ERROR_ENTRY_NODEF10(690),

		// --- 700-799 (ПОЛИЗ) ---
		ERROR_ENTRY(700, "Ошибка преобразования в ПОЛИЗ"),
		ERROR_ENTRY(701, "Ошибка ПОЛИЗ: Стек пуст"),
		ERROR_ENTRY(702, "Ошибка ПОЛИЗ: Нарушен баланс скобок"),
		ERROR_ENTRY_NODEF(703), ERROR_ENTRY_NODEF(704), ERROR_ENTRY_NODEF(705), ERROR_ENTRY_NODEF(706), ERROR_ENTRY_NODEF(707), ERROR_ENTRY_NODEF(708), ERROR_ENTRY_NODEF(709),
		ERROR_ENTRY_NODEF10(710), ERROR_ENTRY_NODEF10(720), ERROR_ENTRY_NODEF10(730), ERROR_ENTRY_NODEF10(740),
		ERROR_ENTRY_NODEF10(750), ERROR_ENTRY_NODEF10(760), ERROR_ENTRY_NODEF10(770), ERROR_ENTRY_NODEF10(780), ERROR_ENTRY_NODEF10(790),

		// --- 800-999 (Резерв) ---
		ERROR_ENTRY_NODEF10(800), ERROR_ENTRY_NODEF10(810), ERROR_ENTRY_NODEF10(820), ERROR_ENTRY_NODEF10(830), ERROR_ENTRY_NODEF10(840),
		ERROR_ENTRY_NODEF10(850), ERROR_ENTRY_NODEF10(860), ERROR_ENTRY_NODEF10(870), ERROR_ENTRY_NODEF10(880), ERROR_ENTRY_NODEF10(890),
		ERROR_ENTRY_NODEF10(900), ERROR_ENTRY_NODEF10(910), ERROR_ENTRY_NODEF10(920), ERROR_ENTRY_NODEF10(930), ERROR_ENTRY_NODEF10(940),
		ERROR_ENTRY_NODEF10(950), ERROR_ENTRY_NODEF10(960), ERROR_ENTRY_NODEF10(970), ERROR_ENTRY_NODEF10(980), ERROR_ENTRY_NODEF10(990)
	};

	ERROR geterror(int id) {
		if (id < 0 || id >= ERROR_MAX_ENTRY) return errors[0];
		return errors[id];
	}

	ERROR geterrorin(int id, int line, int col) {
		if (id < 0 || id >= ERROR_MAX_ENTRY) return errors[0];
		ERROR error = errors[id];
		error.inext.col = col;
		error.inext.line = line;
		return error;
	}
};