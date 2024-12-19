-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Хост: localhost
-- Время создания: Дек 08 2024 г., 13:53
-- Версия сервера: 5.6.43-84.3-log
-- Версия PHP: 8.0.30

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- База данных: `bezru`
--

-- --------------------------------------------------------

--
-- Структура таблицы `vcc_stat`
--

DROP TABLE IF EXISTS `vcc_stat`;
CREATE TABLE `vcc_stat` (
  `id` int(11) NOT NULL,
  `voltage` float DEFAULT NULL,
  `temperature` float DEFAULT NULL,
  `coeff` float DEFAULT NULL,
  `rssi` int(11) DEFAULT NULL,
  `mac` varchar(20) DEFAULT NULL,
  `ip` varchar(20) DEFAULT NULL,
  `count` int(11) DEFAULT NULL,
  `send_period` int(11) DEFAULT NULL,
  `ver` varchar(20) DEFAULT NULL,
  `chip_id` int(11) DEFAULT NULL,
  `freemem` int(11) DEFAULT NULL,
  `timestamp` timestamp NULL DEFAULT NULL,
  `stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Индексы сохранённых таблиц
--

--
-- Индексы таблицы `vcc_stat`
--
ALTER TABLE `vcc_stat`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT для сохранённых таблиц
--

--
-- AUTO_INCREMENT для таблицы `vcc_stat`
--
ALTER TABLE `vcc_stat`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
