-- phpMyAdmin SQL Dump
-- version 2.6.3-pl1
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Erstellungszeit: 07. Dezember 2011 um 21:01
-- Server Version: 5.0.11
-- PHP-Version: 5.1.4
-- 
-- Datenbank: `trasa`
-- 

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `characters`
-- 

CREATE TABLE `characters` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(64) NOT NULL,
  `lastname` varchar(64) NOT NULL,
  `slotID` tinyint(4) NOT NULL,
  `userID` int(11) NOT NULL,
  `gender` tinyint(4) NOT NULL,
  `raceID` tinyint(4) NOT NULL,
  `classID` tinyint(4) NOT NULL,
  `ad1_classId` int(11) NOT NULL COMMENT 'Helmet',
  `ad1_hue` int(10) unsigned NOT NULL,
  `ad2_classId` int(11) NOT NULL COMMENT 'Boots',
  `ad2_hue` int(10) unsigned NOT NULL,
  `ad3_classId` int(11) NOT NULL COMMENT 'Gloves',
  `ad3_hue` int(10) unsigned NOT NULL,
  `ad4_classId` int(11) NOT NULL,
  `ad4_hue` int(10) unsigned NOT NULL,
  `ad5_classId` int(11) NOT NULL,
  `ad5_hue` int(10) unsigned NOT NULL,
  `ad6_classId` int(11) NOT NULL,
  `ad6_hue` int(10) unsigned NOT NULL,
  `ad7_classId` int(11) NOT NULL,
  `ad7_hue` int(10) unsigned NOT NULL,
  `ad8_classId` int(11) NOT NULL,
  `ad8_hue` int(10) unsigned NOT NULL,
  `ad9_classId` int(11) NOT NULL,
  `ad9_hue` int(10) unsigned NOT NULL,
  `ad10_classId` int(11) NOT NULL,
  `ad10_hue` int(10) unsigned NOT NULL,
  `ad11_classId` int(11) NOT NULL,
  `ad11_hue` int(10) unsigned NOT NULL,
  `ad12_classId` int(11) NOT NULL,
  `ad12_hue` int(10) unsigned NOT NULL,
  `ad13_classId` int(11) NOT NULL,
  `ad13_hue` int(10) unsigned NOT NULL,
  `ad14_classId` int(11) NOT NULL,
  `ad14_hue` int(10) unsigned NOT NULL,
  `ad15_classId` int(11) NOT NULL COMMENT 'Vest',
  `ad15_hue` int(10) unsigned NOT NULL,
  `ad16_classId` int(11) NOT NULL COMMENT 'Legs',
  `ad16_hue` int(10) unsigned NOT NULL,
  `ad17_classId` int(11) NOT NULL,
  `ad17_hue` int(10) unsigned NOT NULL,
  `ad18_classId` int(11) NOT NULL,
  `ad18_hue` int(10) unsigned NOT NULL,
  `ad19_classId` int(11) NOT NULL,
  `ad19_hue` int(10) unsigned NOT NULL,
  `ad20_classId` int(11) NOT NULL,
  `ad20_hue` int(10) unsigned NOT NULL,
  `ad21_classId` int(11) NOT NULL,
  `ad21_hue` int(10) unsigned NOT NULL,
  `currentContextId` int(11) NOT NULL COMMENT 'the current map id',
  `posX` float NOT NULL,
  `posY` float NOT NULL,
  `posZ` float NOT NULL,
  `rotation` float NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 AUTO_INCREMENT=63 ;

-- 
-- Daten für Tabelle `characters`
-- 

INSERT INTO `characters` (`id`, `name`, `lastname`, `slotID`, `userID`, `gender`, `raceID`, `classID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`, `currentContextId`, `posX`, `posY`, `posZ`, `rotation`) VALUES (50, 'Bootcamper', 'Warfare', 5, 1, 0, 1, 2, 18328, 4286611584, 18271, 4286611584, 18299, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3672, 4280300626, 18384, 4286611584, 18356, 4286611584, 20824, 4286886614, 0, 0, 0, 0, 0, 0, 0, 0, 1220, 184.43, 163.12, -53.75, 0);
INSERT INTO `characters` (`id`, `name`, `lastname`, `slotID`, `userID`, `gender`, `raceID`, `classID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`, `currentContextId`, `posX`, `posY`, `posZ`, `rotation`) VALUES (49, 'Richard', 'Garriott', 4, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3672, 4280300626, 0, 0, 0, 0, 20824, 4286886614, 0, 0, 0, 0, 0, 0, 0, 0, 1991, -87.67, 10, 100, 0);
INSERT INTO `characters` (`id`, `name`, `lastname`, `slotID`, `userID`, `gender`, `raceID`, `classID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`, `currentContextId`, `posX`, `posY`, `posZ`, `rotation`) VALUES (48, 'OMG', 'WTF', 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3672, 4280300626, 0, 0, 0, 0, 20824, 4286886614, 0, 0, 0, 0, 0, 0, 0, 0, 1220, 784, 295, 380, 0);
INSERT INTO `characters` (`id`, `name`, `lastname`, `slotID`, `userID`, `gender`, `raceID`, `classID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`, `currentContextId`, `posX`, `posY`, `posZ`, `rotation`) VALUES (52, 'Kyuu', '', 1, 4, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9783, 4280300626, 0, 0, 0, 0, 7695, 4286886614, 0, 0, 25335, 4282551464, 25349, 4280300626, 0, 0, 1985, -218.328, 100.023, -58.4531, 0);
INSERT INTO `characters` (`id`, `name`, `lastname`, `slotID`, `userID`, `gender`, `raceID`, `classID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`, `currentContextId`, `posX`, `posY`, `posZ`, `rotation`) VALUES (59, 'Gort', 'Laa', 1, 7, 0, 1, 1, 10908, 4286611584, 7054, 4286611584, 10909, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3672, 4280300626, 7052, 4286611584, 7053, 4286611584, 20824, 4286886614, 0, 0, 0, 0, 0, 0, 0, 0, 1985, -218.328, 100.023, -58.4531, 0);
INSERT INTO `characters` (`id`, `name`, `lastname`, `slotID`, `userID`, `gender`, `raceID`, `classID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`, `currentContextId`, `posX`, `posY`, `posZ`, `rotation`) VALUES (60, 'Schorty', 'Schurr', 3, 1, 0, 2, 1, 10908, 4286611584, 7054, 4286611584, 10909, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3812, 4280300626, 7052, 4286611584, 7053, 4286611584, 28610, 4284117347, 0, 0, 0, 0, 0, 0, 0, 0, 1985, -218.328, 100.023, -58.4531, 0);
INSERT INTO `characters` (`id`, `name`, `lastname`, `slotID`, `userID`, `gender`, `raceID`, `classID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`, `currentContextId`, `posX`, `posY`, `posZ`, `rotation`) VALUES (61, 'Wexx', 'Gronzer', 6, 1, 0, 2, 1, 10908, 4286611584, 7054, 4286611584, 10909, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3812, 4280300626, 7052, 4286611584, 7053, 4286611584, 24743, 4284117347, 0, 0, 0, 0, 0, 0, 0, 0, 1148, -73.9, 116.9, 730.1, 0);
INSERT INTO `characters` (`id`, `name`, `lastname`, `slotID`, `userID`, `gender`, `raceID`, `classID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`, `currentContextId`, `posX`, `posY`, `posZ`, `rotation`) VALUES (62, 'Anthrax', 'Dennton', 3, 7, 0, 4, 1, 10908, 4286611584, 7054, 4286611584, 10909, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3812, 4280300626, 7052, 4286611584, 7053, 4286611584, 24744, 4280891507, 0, 0, 25336, 4282551464, 0, 0, 0, 0, 1220, 894.9, 307.9, 347.1, 0);

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `game_servers`
-- 

CREATE TABLE `game_servers` (
  `server_id` int(11) NOT NULL,
  `host` bigint(16) NOT NULL,
  `port` int(11) NOT NULL,
  `age_limit` int(11) NOT NULL,
  `pk_flag` int(11) NOT NULL,
  `current_users` int(11) NOT NULL,
  `max_users` int(11) NOT NULL,
  `status` int(11) NOT NULL,
  `check_time` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `static` int(11) NOT NULL,
  PRIMARY KEY  (`host`,`port`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Daten für Tabelle `game_servers`
-- 

INSERT INTO `game_servers` (`server_id`, `host`, `port`, `age_limit`, `pk_flag`, `current_users`, `max_users`, `status`, `check_time`, `static`) VALUES (101, 16777343, 8001, 18, 1, 1, 100, 1, '2011-08-06 10:43:40', 1);

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `mission`
-- 

CREATE TABLE `mission` (
  `missionId` int(11) NOT NULL,
  `dispenserNPC` int(11) NOT NULL default '0',
  `collectorNPC` int(11) NOT NULL default '0',
  PRIMARY KEY  (`missionId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `mission`
-- 

INSERT INTO `mission` (`missionId`, `dispenserNPC`, `collectorNPC`) VALUES (1992, 4116, 4148);

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `missionstate`
-- 

CREATE TABLE `missionstate` (
  `id` int(11) NOT NULL auto_increment,
  `character` int(11) NOT NULL,
  `mission` int(11) NOT NULL,
  `status` int(11) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- 
-- Daten für Tabelle `missionstate`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `npc`
-- 

CREATE TABLE `npc` (
  `id` int(12) NOT NULL,
  `name` varchar(56) NOT NULL,
  `mapContextId` int(11) NOT NULL,
  `posX` float NOT NULL,
  `posY` float NOT NULL,
  `posZ` float NOT NULL,
  `rotation` float NOT NULL,
  `entityClassID` int(11) NOT NULL,
  `ad1_classId` int(11) NOT NULL,
  `ad1_hue` int(10) unsigned NOT NULL,
  `ad2_classId` int(11) NOT NULL,
  `ad2_hue` int(10) unsigned NOT NULL,
  `ad3_classId` int(11) NOT NULL,
  `ad3_hue` int(10) unsigned NOT NULL,
  `ad4_classId` int(11) NOT NULL,
  `ad4_hue` int(10) unsigned NOT NULL,
  `ad5_classId` int(11) NOT NULL,
  `ad5_hue` int(10) unsigned NOT NULL,
  `ad6_classId` int(11) NOT NULL,
  `ad6_hue` int(10) unsigned NOT NULL,
  `ad7_classId` int(11) NOT NULL,
  `ad7_hue` int(10) unsigned NOT NULL,
  `ad8_classId` int(11) NOT NULL,
  `ad8_hue` int(10) unsigned NOT NULL,
  `ad9_classId` int(11) NOT NULL,
  `ad9_hue` int(10) unsigned NOT NULL,
  `ad10_classId` int(11) NOT NULL,
  `ad10_hue` int(10) unsigned NOT NULL,
  `ad11_classId` int(11) NOT NULL,
  `ad11_hue` int(10) unsigned NOT NULL,
  `ad12_classId` int(11) NOT NULL,
  `ad12_hue` int(10) unsigned NOT NULL,
  `ad13_classId` int(11) NOT NULL,
  `ad13_hue` int(10) unsigned NOT NULL,
  `ad14_classId` int(11) NOT NULL,
  `ad14_hue` int(10) unsigned NOT NULL,
  `ad15_classId` int(11) NOT NULL,
  `ad15_hue` int(10) unsigned NOT NULL,
  `ad16_classId` int(11) NOT NULL,
  `ad16_hue` int(10) unsigned NOT NULL,
  `ad17_classId` int(11) NOT NULL,
  `ad17_hue` int(10) unsigned NOT NULL,
  `ad18_classId` int(11) NOT NULL,
  `ad18_hue` int(10) unsigned NOT NULL,
  `ad19_classId` int(11) NOT NULL,
  `ad19_hue` int(10) unsigned NOT NULL,
  `ad20_classId` int(11) NOT NULL,
  `ad20_hue` int(10) unsigned NOT NULL,
  `ad21_classId` int(11) NOT NULL,
  `ad21_hue` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `npc`
-- 

INSERT INTO `npc` (`id`, `name`, `mapContextId`, `posX`, `posY`, `posZ`, `rotation`, `entityClassID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`) VALUES (4116, 'Catherine Jones', 1985, -221.281, 101.051, -65.4648, 0, 20972, 0, 0, 18271, 4286611584, 18299, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12143, 4289039425, 9782, 4289039425, 18384, 4286611584, 18356, 4286611584, 7695, 4286886614, 0, 0, 0, 0, 25349, 4280300626, 0, 0);
INSERT INTO `npc` (`id`, `name`, `mapContextId`, `posX`, `posY`, `posZ`, `rotation`, `entityClassID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`) VALUES (4148, 'John Adams', 1985, -206.129, 98.0234, -48.1094, 0, 20975, 0, 0, 18271, 4286611584, 18299, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6054, 0, 9355, 4293125348, 18384, 4286611584, 18356, 4286611584, 24010, 4286886614, 20824, 2155905152, 25336, 4282551464, 25349, 4293125348, 0, 0);
INSERT INTO `npc` (`id`, `name`, `mapContextId`, `posX`, `posY`, `posZ`, `rotation`, `entityClassID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`) VALUES (4119, '13th of 23', 1985, -205.123, 98.3215, -48.3427, 0, 3868, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `pathnodes`
-- 

CREATE TABLE `pathnodes` (
  `id` int(8) NOT NULL auto_increment,
  `spawnid` int(8) NOT NULL,
  `contextid` int(8) NOT NULL,
  `posx` float NOT NULL,
  `posy` float NOT NULL,
  `posz` float NOT NULL,
  `pindex` int(3) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=6 ;

-- 
-- Daten für Tabelle `pathnodes`
-- 

INSERT INTO `pathnodes` (`id`, `spawnid`, `contextid`, `posx`, `posy`, `posz`, `pindex`) VALUES (2, -858993460, -858993460, -1.07374e+008, -1.07374e+008, -1.07374e+008, -858993460);
INSERT INTO `pathnodes` (`id`, `spawnid`, `contextid`, `posx`, `posy`, `posz`, `pindex`) VALUES (3, -858993460, -858993460, -1.07374e+008, -1.07374e+008, -1.07374e+008, -858993460);
INSERT INTO `pathnodes` (`id`, `spawnid`, `contextid`, `posx`, `posy`, `posz`, `pindex`) VALUES (4, 31, 1220, 0, -3.40282e+038, 0, 22756288);
INSERT INTO `pathnodes` (`id`, `spawnid`, `contextid`, `posx`, `posy`, `posz`, `pindex`) VALUES (5, 31, 1220, 176.902, 163.381, -50.293, 1);

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `sessions`
-- 

CREATE TABLE `sessions` (
  `session_id1` bigint(20) NOT NULL,
  `session_id2` bigint(20) NOT NULL,
  `uid` bigint(20) NOT NULL,
  `account` varchar(30) NOT NULL,
  `checktime` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Daten für Tabelle `sessions`
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
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=106 ;

-- 
-- Daten für Tabelle `spawnpool`
-- 

INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (1, 1, -200.345, 95.92, -24.234, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (2, 1, -197.001, 96.401, -44.003, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (3, 1, -217.4, 99.401, -54.004, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (4, 1, -135.441, 84.8375, 33.5898, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (16, 3, 74.6602, 102.427, 137.871, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (7, 1, -77.0938, 86.9195, 107.141, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (8, 1, -35.418, 91.068, 82.6289, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (15, 2, -201.101, 95.4, -25.034, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (10, 1, 39.3906, 109.134, 150.832, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (11, 1, 72.6602, 109.427, 136.871, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (12, 1, 88.582, 109.474, 149.184, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (17, 2, -177.441, 167.81, -75.0938, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (18, 2, 51.5938, 163.603, -84.3711, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (19, 2, 136.063, 164.74, 41.6992, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (20, 2, 198.988, 166.205, 76.0469, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (21, 2, 271.688, 167.576, -4.19922, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (22, 1, -484.191, 192.064, 159.422, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (23, 1, -482.203, 188.416, 225.371, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (24, 2, -388.121, 176.49, 183.266, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (25, 2, -296.105, 175.384, 209.758, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (26, 1, -149.922, 184.17, 177.512, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (27, 1, -112.922, 190.494, 170.102, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (28, 1, -66.7578, 199.033, 152.133, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (29, 1, -33.4492, 194.654, 127.254, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (30, 4, 200.345, 173.85, 200.45, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (31, 5, 205.444, 174.188, 210.567, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (32, 6, -205.1, 95.4, -24, 1985);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (33, 7, 198.1, 65.7, 76.5, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (34, 3, 37.1484, 171.076, 33.8477, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (35, 6, 133.965, 174.443, 235.266, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (36, 1, 126.32, 234.22, 520.156, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (37, 2, 19.6758, 168.521, -20.793, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (38, 4, 111.203, 173.931, 303.629, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (39, 4, 152.48, 211.779, 410.434, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (40, 2, 172.723, 243.834, 558.973, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (41, 2, 304.918, 274.15, 630.336, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (42, 2, 361.617, 281.654, 683.098, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (43, 6, 511.59, 290.146, 513.09, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (44, 6, 527.176, 288.326, 498.391, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (45, 6, 590.805, 288.478, 445.559, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (46, 1, 600.055, 287.459, 436.391, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (47, 1, 610.336, 287.638, 413.109, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (48, 4, 672.402, 286.826, 466.313, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (49, 5, 734.867, 295.845, 485.633, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (50, 5, 789.414, 300.478, 491.633, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (51, 5, 840.617, 301.623, 482.391, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (52, 6, 852.465, 298.146, 466.301, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (53, 6, 835.621, 293.927, 390.723, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (54, 6, 813.027, 294.416, 398.039, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (55, 5, 798.852, 294.541, 377.508, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (56, 7, 216.27, 163.666, -50.8867, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (57, 7, 182.91, 163.858, -58.4688, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (58, 7, 162.484, 163.604, -104.543, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (59, 6, 186.691, 163.721, -40.8438, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (60, 1, -263.922, 169.459, -231.488, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (61, 1, -251.133, 169.83, -257.801, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (62, 5, -266.543, 170.522, -279.246, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (63, 5, -283.598, 173.127, -328.086, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (64, 6, -316.691, 175.327, -407.375, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (65, 9, 187.477, 165.459, -10.3281, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (99, 6, 17.8008, 113.578, 388.129, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (67, 2, 226.457, 167.041, 10.7109, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (68, 1, 209.066, 167.948, 6.63672, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (69, 1, 180.473, 166.873, 14.543, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (70, 1, 205.004, 167.955, 22.375, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (71, 10, 234.414, 167.245, 8.22656, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (72, 10, 216.555, 166.959, 5.05078, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (73, 11, 202.012, 167.022, -0.902344, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (74, 11, 186.191, 165.94, -1.88672, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (75, 12, 205.543, 167.127, -0.703125, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (76, 12, 243.73, 165.315, -5.82031, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (77, 12, 253.172, 164.67, -24.4453, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (78, 11, 252.18, 164.866, -42.3398, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (79, 10, 257.73, 165.35, -25.1992, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (80, 11, 196.805, 163.725, -28.0547, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (82, 2, 181.277, 107.027, 175.762, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (83, 2, 146.301, 103.75, 180.992, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (84, 11, 55.9531, 62.6992, 149.117, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (85, 11, 52.3125, 63.3906, 116.559, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (86, 12, 51.7539, 61.2656, 84.1289, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (87, 3, 24.6563, 61.6094, 3.80469, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (89, 2, 167.414, 169.195, 168.297, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (98, 6, -73.8555, 111.371, 365.332, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (97, 7, -93.7227, 116.777, 447.781, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (92, 2, 652.586, 284.949, 240.445, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (93, 2, 607.039, 286.691, 270.539, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (94, 2, 584.379, 288.598, 329.262, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (95, 12, 572.762, 290.23, 358.055, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (96, 12, 500.254, 294.836, 500.102, 1220);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (100, 1, 13.3828, 91.3984, 319.141, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (101, 1, 39.0664, 63.7109, 195.191, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (102, 1, 42.0703, 48.2852, -119.363, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (103, 3, 94.9922, 48.2617, -167.398, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (104, 1, -178.844, 47.2617, -46.9492, 1148);
INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES (105, 1, -124.199, 48.6914, -89.4102, 1148);

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
  `locktime` int(8) NOT NULL,
  `attackspeed` int(8) NOT NULL,
  `velocity` float NOT NULL,
  `attackaction` int(5) NOT NULL,
  `attakstyle` int(2) NOT NULL,
  `actionid` int(8) NOT NULL,
  `melee_damage` int(10) NOT NULL,
  `range_damage` int(10) NOT NULL,
  `hitpoints` int(10) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=13 ;

-- 
-- Daten für Tabelle `spawntype`
-- 

INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (1, 1, 'thrax_footsoldiers', '25580', 0, 2, 15000, 700, 5.1, 1, 0, 1, 5, 8, 295);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (2, 1, 'boargar_general_spawn', '6031', 3, 0, 10000, 1000, 6.2, 10, 1, 174, 15, 15, 200);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (3, 1, 'bane_stalkerspawn_general', '3781', 0, 0, 40000, 2700, 3.3, 108, 0, 1, 37, 100, 1000);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (4, 1, 'forean_spearman_normal', '6043', 1, 0, 15000, 1800, 5.9, 204, 0, 1, 8, 9, 250);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (5, 1, 'forean_elder_normal', '6163', 1, 0, 25000, 2000, 4.9, 203, 0, 1, 9, 8, 200);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (6, 1, 'afs_soldier_light_m', '29765', 1, 0, 25000, 900, 5.7, 67, 0, 1, 5, 7, 210);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (7, 1, 'afs_soldier_medium_f', '29423', 1, 0, 25000, 1100, 5.1, 67, 0, 1, 7, 8, 310);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (8, 1, 'thrax_footsoldiers_rifle', '25581', 0, 0, 30000, 1200, 5.4, 79, 0, 1, 10, 11, 300);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (9, 1, 'afs_mech_standard', '10442', 1, 0, 30000, 2000, 5.6, 293, 0, 1, 10, 11, 400);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (10, 1, 'bane_hunter_invasion', '10166', 0, 2, 100000, 1500, 5.3, 1, 0, 1, 5, 9, 275);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (11, 1, 'bane_amoeboid_invasion', '6032', 0, 0, 100000, 1500, 5.1, 6032, 1, 174, 7, 7, 350);
INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES (12, 1, 'thrax_footsoldier_invasion', '25580', 0, 2, 100000, 700, 5.1, 1, 0, 1, 10, 15, 350);

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `teleporter`
-- 

CREATE TABLE `teleporter` (
  `id` int(10) NOT NULL auto_increment,
  `type` int(3) NOT NULL,
  `description` varchar(50) character set utf8 NOT NULL,
  `px` float NOT NULL,
  `py` float NOT NULL,
  `pz` float NOT NULL,
  `dx` float NOT NULL,
  `dy` float NOT NULL,
  `dz` float NOT NULL,
  `modelid` int(10) NOT NULL,
  `bx` float NOT NULL,
  `bz` float NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=41 ;

-- 
-- Daten für Tabelle `teleporter`
-- 

INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (1, 7, 'foreas_base_hq_1', -42.53, 116.31, 436.95, -2.31, 56, 399.57, 7895, 2, 2);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (2, 7, 'foreas_base_hq_2', -2.31, 56, 399.57, -42.53, 116.31, 436.95, 7895, 2, 2);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (3, 11, 'aliadas_turret_cmd_1', 851.92, 295.42, 396.76, 0, 0, 0, 11302, 2, 2);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (20, 4, 'wilderness_landingzone_cp', 197.758, 162.27, -53.0273, 0, 0, 0, 3814, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (21, 10, 'wilderness_landingzone_ff', 197.773, 163.205, -33.9063, 0, 0, 0, 20000001, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (18, 11, '', 157.84, 169.246, -26.7969, 0, 0, 0, 4064, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (19, 11, '', 238.258, 169.246, -26.7617, 0, 0, 0, 4064, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (17, 8, 'wp_aliadas_1', 806.414, 294.316, 391.16, 0, 0, 0, 25651, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (22, 5, '', 184.098, 120.961, 137.863, 0, 0, 0, 7296, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (23, 9, 'test_door_aliadas', 806, 296, 380, 0, 0, 0, 7607, 2, 2);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (24, 8, '', 824.07, 293.773, 387.191, 0, 0, 0, 3579, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (25, -858993460, '', 154.063, 163.004, -109.848, 0, 0, 0, 3579, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (26, 14, '', -39.2891, 115.465, 764.637, 0, 0, 0, 25408, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (27, 15, '', -80.0859, 115.902, 530.797, 0, 0, 0, 7343, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (28, 8, '', -32.1641, 116.215, 568.746, 0, 0, 0, 3579, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (29, 10, '', 10.3828, 118.723, 407.883, 0, 0, 0, 9572, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (30, 10, '', -91.332, 117.504, 412.156, 0, 0, 0, 9572, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (31, 11, '', -79.9453, 117.387, 398.883, 0, 0, 0, 4064, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (32, 11, '', -0.769531, 116.648, 382.871, 0, 0, 0, 4064, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (33, 11, '', -203.801, 63.2461, 82.9648, 0, 0, 0, 4064, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (34, 11, '', -179.992, 71.9805, 69.8828, 0, 0, 0, 4064, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (35, 4, '', -243.121, 57.293, 44.4336, 0, 0, 0, 3814, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (36, 11, '', -204.125, 63.2461, -5.10156, 0, 0, 0, 4064, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (37, 11, '', -243.992, 63.2461, -5.19141, 0, 0, 0, 4064, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (38, 11, '', -259.328, 74.6992, -17.457, 0, 0, 0, 4064, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (39, 8, '', 65.6289, 84.6875, -67.5078, 0, 0, 0, 3579, 0, 0);
INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `modelid`, `bx`, `bz`) VALUES (40, 11, '', 211.203, 73.793, -120.824, 0, 0, 0, 4064, 0, 0);

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `users`
-- 

CREATE TABLE `users` (
  `id` int(11) NOT NULL auto_increment,
  `username` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `birthday` date NOT NULL,
  `blocked` tinyint(1) NOT NULL,
  `logged_in` tinyint(4) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=15 ;

-- 
-- Daten für Tabelle `users`
-- 

INSERT INTO `users` (`id`, `username`, `password`, `birthday`, `blocked`, `logged_in`) VALUES (1, 'manolo', '12cdb9b24211557ef1802bf5a875fd2c', '0000-00-00', 0, 0);
INSERT INTO `users` (`id`, `username`, `password`, `birthday`, `blocked`, `logged_in`) VALUES (5, 'waffo', 'b0c8dbca2003ac80b5d6316aaa49a4f5', '0000-00-00', 0, 0);
INSERT INTO `users` (`id`, `username`, `password`, `birthday`, `blocked`, `logged_in`) VALUES (6, 'kyuu', 'c467322c1baa90349d49cd76d881c661', '0000-00-00', 0, 0);
INSERT INTO `users` (`id`, `username`, `password`, `birthday`, `blocked`, `logged_in`) VALUES (7, 'dennton', '7b113a80dcfd1002ab89b5b58ed3b9d7', '0000-00-00', 0, 0);
INSERT INTO `users` (`id`, `username`, `password`, `birthday`, `blocked`, `logged_in`) VALUES (9, 'dahrkael', '7f47b86ad439e88b387d118f99e81ef0', '0000-00-00', 0, 0);
INSERT INTO `users` (`id`, `username`, `password`, `birthday`, `blocked`, `logged_in`) VALUES (10, 'thuvvik', '164b02a66c15fc5c6fcde972385bd06d', '0000-00-00', 0, 0);
INSERT INTO `users` (`id`, `username`, `password`, `birthday`, `blocked`, `logged_in`) VALUES (13, 'commodore', 'a5171c45ebf9310e817f35a5111aa86e', '0000-00-00', 0, 0);
INSERT INTO `users` (`id`, `username`, `password`, `birthday`, `blocked`, `logged_in`) VALUES (14, 'mdagli', '44b8b4d5a8272f1661af8ca25a312571', '0000-00-00', 0, 0);