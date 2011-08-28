-- phpMyAdmin SQL Dump
-- version 2.6.3-pl1
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Erstellungszeit: 26. August 2011 um 23:02
-- Server Version: 5.0.11
-- PHP-Version: 5.1.4
-- 
-- Datenbank: `trasa`
-- 

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `spawnpool`
-- 

CREATE TABLE `spawnpool` (
  `id` int(10) NOT NULL auto_increment,
  `type` int(10) NOT NULL,
  `posx` float NOT NULL,
  `posy` float NOT NULL,
  `posz` float NOT NULL,
  `contextid` int(5) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=35 ;

-- 
-- Daten für Tabelle `spawnpool`
-- 

INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (1, 1, -200.345, 95.52, -24.234, 1985),
(2, 1, -197.001, 96.001, -44.003, 1985),
(3, 1, -217.4, 99.001, -54.004, 1985),
(4, 1, -135.441, 84.4375, 33.5898, 1985),
(16, 3, 74.6602, 102.027, 137.871, 1985),
(7, 1, -77.0938, 86.5195, 107.141, 1985),
(8, 1, -35.418, 90.668, 82.6289, 1985),
(15, 2, -201.101, 95, -25.034, 1985),
(10, 1, 39.3906, 108.734, 150.832, 1985),
(11, 1, 72.6602, 109.027, 136.871, 1985),
(12, 1, 88.582, 109.074, 149.184, 1985),
(17, 2, -177.441, 167.41, -75.0938, 1220),
(18, 2, 51.5938, 163.203, -84.3711, 1220),
(19, 2, 136.063, 164.34, 41.6992, 1220),
(20, 2, 198.988, 165.805, 76.0469, 1220),
(21, 2, 271.688, 167.176, -4.19922, 1220),
(22, 1, -484.191, 191.664, 159.422, 1220),
(23, 1, -482.203, 188.016, 225.371, 1220),
(24, 2, -388.121, 176.09, 183.266, 1220),
(25, 2, -296.105, 174.984, 209.758, 1220),
(26, 1, -149.922, 183.77, 177.512, 1220),
(27, 1, -112.922, 190.094, 170.102, 1220),
(28, 1, -66.7578, 198.633, 152.133, 1220),
(29, 1, -33.4492, 194.254, 127.254, 1220),
(30, 4, 200.345, 173.45, 200.45, 1220),
(31, 5, 205.444, 173.788, 210.567, 1220),
(32, 6, -205.1, 95, -24, 1985),
(33, 7, 198.1, 65.3, 76.5, 1220),
(34, 3, 37.1484, 170.676, 33.8477, 1220);

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `spawntype`
-- 

CREATE TABLE `spawntype` (
  `id` int(10) NOT NULL auto_increment,
  `maxcreatures` int(4) NOT NULL,
  `spawnname` varchar(50) character set utf8 NOT NULL,
  `creatures` varchar(70) character set utf8 NOT NULL,
  `faction` int(4) NOT NULL,
  `spawnanim` int(4) NOT NULL,
  `activespawns` int(4) NOT NULL,
  `locktime` int(8) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=8 ;

-- 
-- Daten für Tabelle `spawntype`
-- 

INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `activespawns`, `locktime`) VALUES (1, 2, 'thrax_footsoldiers', '25580', 0, 2, 7, 15000),
(2, 2, 'boargar_general_spawn', '6031', -1, 0, 2, 10000),
(3, 1, 'bane_stalkerspawn_general', '3781', 0, 0, 1, 30000),
(4, 2, 'forean_spearman_normal', '6043', 1, 0, 2, 15000),
(5, 2, 'forean_elder_normal', '6163', 1, 0, 2, 25000),
(6, 2, 'afs_soldier_light_m', '29765', 1, 0, 2, 25000),
(7, 2, 'afs_soldier_medium_f', '29423', 1, 0, 2, 25000);