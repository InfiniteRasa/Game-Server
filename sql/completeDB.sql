-- phpMyAdmin SQL Dump
-- version 3.3.0
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Oct 10, 2012 at 08:59 PM
-- Server version: 5.1.44
-- PHP Version: 5.2.13

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `ir_authentication`
--
DROP DATABASE `ir_authentication`;
CREATE DATABASE `ir_authentication` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE `ir_authentication`;

-- --------------------------------------------------------

--
-- Table structure for table `game_servers`
--

DROP TABLE IF EXISTS `game_servers`;
CREATE TABLE IF NOT EXISTS `game_servers` (
  `server_id` int(11) NOT NULL,
  `host` varchar(25) NOT NULL,
  `port` int(11) NOT NULL,
  `age_limit` int(11) NOT NULL,
  `pk_flag` int(11) NOT NULL,
  `current_users` int(11) NOT NULL,
  `max_users` int(11) NOT NULL,
  `status` int(11) NOT NULL,
  `check_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `static` int(11) NOT NULL,
  PRIMARY KEY (`host`,`port`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `game_servers`
--

INSERT INTO `game_servers` (`server_id`, `host`, `port`, `age_limit`, `pk_flag`, `current_users`, `max_users`, `status`, `check_time`, `static`) VALUES
(234, '127.0.0.1', 8001, 18, 0, 0, 10, 1, '2012-10-10 20:48:52', 1);

-- --------------------------------------------------------

--
-- Table structure for table `sessions`
--

DROP TABLE IF EXISTS `sessions`;
CREATE TABLE IF NOT EXISTS `sessions` (
  `session_id1` bigint(20) NOT NULL,
  `session_id2` bigint(20) NOT NULL,
  `uid` bigint(20) NOT NULL,
  `account` varchar(30) NOT NULL,
  `checktime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`session_id1`,`session_id2`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `sessions`
--


-- --------------------------------------------------------

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `birthday` date NOT NULL,
  `blocked` tinyint(1) NOT NULL,
  `logged_in` tinyint(4) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4 ;

--
-- Dumping data for table `users`
--

INSERT INTO `users` (`id`, `username`, `password`, `birthday`, `blocked`, `logged_in`) VALUES
(1, 'test', '098f6bcd4621d373cade4e832627b4f6', '2011-05-10', 0, 0);
--
-- Database: `ir_gameserver`
--
DROP DATABASE `ir_gameserver`;
CREATE DATABASE `ir_gameserver` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE `ir_gameserver`;

-- --------------------------------------------------------

--
-- Table structure for table `characters`
--

DROP TABLE IF EXISTS `characters`;
CREATE TABLE IF NOT EXISTS `characters` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
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
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=65 ;

--
-- Dumping data for table `characters`
--

INSERT INTO `characters` (`id`, `name`, `lastname`, `slotID`, `userID`, `gender`, `raceID`, `classID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`, `currentContextId`, `posX`, `posY`, `posZ`, `rotation`) VALUES
(50, 'Bootcamper', 'Warfare', 5, 1, 0, 1, 2, 18328, 4286611584, 18271, 4286611584, 18299, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3672, 4280300626, 18384, 4286611584, 18356, 4286611584, 20824, 4286886614, 0, 0, 0, 0, 0, 0, 0, 0, 1220, 184.43, 163.12, -53.75, 0),
(49, 'Richard', 'Garriott', 4, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3672, 4280300626, 0, 0, 0, 0, 20824, 4286886614, 0, 0, 0, 0, 0, 0, 0, 0, 1991, -87.67, 10, 100, 0),
(48, 'OMG', 'WTF', 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3672, 4280300626, 0, 0, 0, 0, 20824, 4286886614, 0, 0, 0, 0, 0, 0, 0, 0, 1220, 784, 295, 380, 0),
(61, 'Wexx', 'Gronzer', 6, 1, 0, 2, 1, 10908, 4286611584, 7054, 4286611584, 10909, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3812, 4280300626, 7052, 4286611584, 7053, 4286611584, 24743, 4284117347, 0, 0, 0, 0, 0, 0, 0, 0, 1148, -73.9, 116.9, 730.1, 0),
(62, 'Anthrax', 'Dennton', 3, 1, 0, 4, 1, 10908, 4286611584, 7054, 4286611584, 10909, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3812, 4280300626, 7052, 4286611584, 7053, 4286611584, 24744, 4280891507, 0, 0, 25336, 4282551464, 0, 0, 0, 0, 1220, 894.9, 307.9, 347.1, 0),
(63, 'Ololol', 'Name', 9, 1, 0, 1, 1, 10908, 4286611584, 7054, 4286611584, 10909, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9782, 4280300626, 7052, 4286611584, 7053, 4286611584, 24005, 4286886614, 0, 0, 25336, 4282551464, 25346, 4280300626, 0, 0, 1220, 894.9, 307.9, 347.1, 0);

-- --------------------------------------------------------

--
-- Table structure for table `mission`
--

DROP TABLE IF EXISTS `mission`;
CREATE TABLE IF NOT EXISTS `mission` (
  `missionId` int(11) NOT NULL,
  `dispenserNPC` int(11) NOT NULL DEFAULT '0',
  `collectorNPC` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`missionId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `mission`
--

INSERT INTO `mission` (`missionId`, `dispenserNPC`, `collectorNPC`) VALUES
(1992, 4116, 4148);

-- --------------------------------------------------------

--
-- Table structure for table `missionstate`
--

DROP TABLE IF EXISTS `missionstate`;
CREATE TABLE IF NOT EXISTS `missionstate` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `character` int(11) NOT NULL,
  `mission` int(11) NOT NULL,
  `status` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

--
-- Dumping data for table `missionstate`
--


-- --------------------------------------------------------

--
-- Table structure for table `npc`
--

DROP TABLE IF EXISTS `npc`;
CREATE TABLE IF NOT EXISTS `npc` (
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
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

--
-- Dumping data for table `npc`
--

INSERT INTO `npc` (`id`, `name`, `mapContextId`, `posX`, `posY`, `posZ`, `rotation`, `entityClassID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`) VALUES
(4116, 'Catherine Jones', 1985, -221.281, 101.051, -65.4648, 0, 20972, 0, 0, 18271, 4286611584, 18299, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12143, 4289039425, 9782, 4289039425, 18384, 4286611584, 18356, 4286611584, 7695, 4286886614, 0, 0, 0, 0, 25349, 4280300626, 0, 0),
(4148, 'John Adams', 1985, -206.129, 98.0234, -48.1094, 0, 20975, 0, 0, 18271, 4286611584, 18299, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6054, 0, 9355, 4293125348, 18384, 4286611584, 18356, 4286611584, 24010, 4286886614, 20824, 2155905152, 25336, 4282551464, 25349, 4293125348, 0, 0),
(4119, '13th of 23', 1985, -205.123, 98.3215, -48.3427, 0, 3868, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `pathnodes`
--

DROP TABLE IF EXISTS `pathnodes`;
CREATE TABLE IF NOT EXISTS `pathnodes` (
  `id` int(8) NOT NULL AUTO_INCREMENT,
  `spawnid` int(8) NOT NULL,
  `contextid` int(8) NOT NULL,
  `posx` float NOT NULL,
  `posy` float NOT NULL,
  `posz` float NOT NULL,
  `pindex` int(3) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=6 ;

--
-- Dumping data for table `pathnodes`
--

INSERT INTO `pathnodes` (`id`, `spawnid`, `contextid`, `posx`, `posy`, `posz`, `pindex`) VALUES
(2, -858993460, -858993460, -1.07374e+008, -1.07374e+008, -1.07374e+008, -858993460),
(3, -858993460, -858993460, -1.07374e+008, -1.07374e+008, -1.07374e+008, -858993460),
(4, 31, 1220, 0, -3.40282e+038, 0, 22756288),
(5, 31, 1220, 176.902, 163.381, -50.293, 1);

-- --------------------------------------------------------

--
-- Table structure for table `spawnpool`
--

DROP TABLE IF EXISTS `spawnpool`;
CREATE TABLE IF NOT EXISTS `spawnpool` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `type` int(10) NOT NULL,
  `posx` float NOT NULL,
  `posy` float NOT NULL,
  `posz` float NOT NULL,
  `contextid` int(5) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=110 ;

--
-- Dumping data for table `spawnpool`
--

INSERT INTO `spawnpool` (`id`, `type`, `posx`, `posy`, `posz`, `contextid`) VALUES
(1, 1, -200.345, 95.92, -24.234, 1985),
(2, 1, -197.001, 96.401, -44.003, 1985),
(3, 1, -217.4, 99.401, -54.004, 1985),
(4, 1, -135.441, 84.8375, 33.5898, 1985),
(16, 3, 74.6602, 102.427, 137.871, 1985),
(7, 1, -77.0938, 86.9195, 107.141, 1985),
(8, 1, -35.418, 91.068, 82.6289, 1985),
(15, 2, -201.101, 95.4, -25.034, 1985),
(10, 1, 39.3906, 109.134, 150.832, 1985),
(11, 1, 72.6602, 109.427, 136.871, 1985),
(12, 1, 88.582, 109.474, 149.184, 1985),
(17, 2, -177.441, 167.81, -75.0938, 1220),
(18, 2, 51.5938, 163.603, -84.3711, 1220),
(19, 2, 136.063, 164.74, 41.6992, 1220),
(20, 2, 198.988, 166.205, 76.0469, 1220),
(21, 2, 271.688, 167.576, -4.19922, 1220),
(22, 1, -484.191, 192.064, 159.422, 1220),
(23, 1, -482.203, 188.416, 225.371, 1220),
(24, 2, -388.121, 176.49, 183.266, 1220),
(25, 2, -296.105, 175.384, 209.758, 1220),
(26, 1, -149.922, 184.17, 177.512, 1220),
(27, 1, -112.922, 190.494, 170.102, 1220),
(28, 1, -66.7578, 199.033, 152.133, 1220),
(29, 1, -33.4492, 194.654, 127.254, 1220),
(30, 4, 200.345, 173.85, 200.45, 1220),
(31, 5, 205.444, 174.188, 210.567, 1220),
(32, 6, -205.1, 95.4, -24, 1985),
(33, 7, 198.1, 65.7, 76.5, 1220),
(34, 3, 37.1484, 171.076, 33.8477, 1220),
(35, 6, 133.965, 174.443, 235.266, 1220),
(36, 1, 126.32, 234.22, 520.156, 1220),
(37, 2, 19.6758, 168.521, -20.793, 1220),
(38, 4, 111.203, 173.931, 303.629, 1220),
(39, 4, 152.48, 211.779, 410.434, 1220),
(40, 2, 172.723, 243.834, 558.973, 1220),
(41, 2, 304.918, 274.15, 630.336, 1220),
(42, 2, 361.617, 281.654, 683.098, 1220),
(43, 6, 511.59, 290.146, 513.09, 1220),
(44, 6, 527.176, 288.326, 498.391, 1220),
(45, 6, 590.805, 288.478, 445.559, 1220),
(46, 1, 600.055, 287.459, 436.391, 1220),
(47, 1, 610.336, 287.638, 413.109, 1220),
(48, 4, 672.402, 286.826, 466.313, 1220),
(49, 5, 734.867, 295.845, 485.633, 1220),
(50, 5, 789.414, 300.478, 491.633, 1220),
(51, 5, 840.617, 301.623, 482.391, 1220),
(52, 6, 852.465, 298.146, 466.301, 1220),
(53, 6, 835.621, 293.927, 390.723, 1220),
(54, 6, 813.027, 294.416, 398.039, 1220),
(55, 5, 798.852, 294.541, 377.508, 1220),
(56, 7, 216.27, 163.666, -50.8867, 1220),
(57, 7, 182.91, 163.858, -58.4688, 1220),
(58, 7, 162.484, 163.604, -104.543, 1220),
(59, 6, 186.691, 163.721, -40.8438, 1220),
(60, 1, -263.922, 169.459, -231.488, 1220),
(61, 1, -251.133, 169.83, -257.801, 1220),
(62, 5, -266.543, 170.522, -279.246, 1220),
(63, 5, -283.598, 173.127, -328.086, 1220),
(64, 6, -316.691, 175.327, -407.375, 1220),
(65, 9, 187.477, 165.459, -10.3281, 1220),
(99, 6, 17.8008, 113.578, 388.129, 1148),
(67, 2, 226.457, 167.041, 10.7109, 1220),
(68, 1, 209.066, 167.948, 6.63672, 1220),
(69, 1, 180.473, 166.873, 14.543, 1220),
(70, 1, 205.004, 167.955, 22.375, 1220),
(71, 10, 234.414, 167.245, 8.22656, 1220),
(72, 10, 216.555, 166.959, 5.05078, 1220),
(73, 11, 202.012, 167.022, -0.902344, 1220),
(74, 11, 186.191, 165.94, -1.88672, 1220),
(75, 12, 205.543, 167.127, -0.703125, 1220),
(76, 12, 243.73, 165.315, -5.82031, 1220),
(77, 12, 253.172, 164.67, -24.4453, 1220),
(78, 11, 252.18, 164.866, -42.3398, 1220),
(79, 10, 257.73, 165.35, -25.1992, 1220),
(80, 11, 196.805, 163.725, -28.0547, 1220),
(82, 2, 181.277, 107.027, 175.762, 1148),
(83, 2, 146.301, 103.75, 180.992, 1148),
(84, 11, 55.9531, 62.6992, 149.117, 1148),
(85, 11, 52.3125, 63.3906, 116.559, 1148),
(86, 12, 51.7539, 61.2656, 84.1289, 1148),
(87, 3, 24.6563, 61.6094, 3.80469, 1148),
(89, 2, 167.414, 169.195, 168.297, 1220),
(98, 6, -73.8555, 111.371, 365.332, 1148),
(97, 7, -93.7227, 116.777, 447.781, 1148),
(92, 2, 652.586, 284.949, 240.445, 1220),
(93, 2, 607.039, 286.691, 270.539, 1220),
(94, 2, 584.379, 288.598, 329.262, 1220),
(95, 12, 572.762, 290.23, 358.055, 1220),
(96, 12, 500.254, 294.836, 500.102, 1220),
(100, 1, 13.3828, 91.3984, 319.141, 1148),
(101, 1, 39.0664, 63.7109, 195.191, 1148),
(102, 1, 42.0703, 48.2852, -119.363, 1148),
(103, 3, 94.9922, 48.2617, -167.398, 1148),
(104, 1, -178.844, 47.2617, -46.9492, 1148),
(105, 1, -124.199, 48.6914, -89.4102, 1148),
(106, 4, 831.574, 294.199, 395.039, 1220),
(107, 4, 833.164, 294.152, 385.871, 1220),
(108, 1, 835.961, 294.027, 385.156, 1220);

-- --------------------------------------------------------

--
-- Table structure for table `spawntype`
--

DROP TABLE IF EXISTS `spawntype`;
CREATE TABLE IF NOT EXISTS `spawntype` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `maxcreatures` int(4) NOT NULL,
  `spawnname` varchar(50) CHARACTER SET utf8 NOT NULL,
  `creatures` varchar(70) CHARACTER SET utf8 NOT NULL,
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
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=13 ;

--
-- Dumping data for table `spawntype`
--

INSERT INTO `spawntype` (`id`, `maxcreatures`, `spawnname`, `creatures`, `faction`, `spawnanim`, `locktime`, `attackspeed`, `velocity`, `attackaction`, `attakstyle`, `actionid`, `melee_damage`, `range_damage`, `hitpoints`) VALUES
(1, 1, 'thrax_footsoldiers', '25580', 0, 2, 15000, 700, 5.1, 1, 0, 1, 5, 8, 295),
(2, 1, 'boargar_general_spawn', '6031', 3, 0, 10000, 1000, 6.2, 10, 1, 174, 15, 15, 200),
(3, 1, 'bane_stalkerspawn_general', '3781', 0, 0, 40000, 2700, 3.3, 108, 0, 1, 37, 100, 1000),
(4, 1, 'forean_spearman_normal', '6043', 1, 0, 15000, 1800, 5.9, 204, 0, 1, 8, 9, 250),
(5, 1, 'forean_elder_normal', '6163', 1, 0, 25000, 2000, 4.9, 203, 0, 1, 9, 8, 200),
(6, 1, 'afs_soldier_light_m', '29765', 1, 0, 25000, 900, 5.7, 67, 0, 1, 5, 7, 210),
(7, 1, 'afs_soldier_medium_f', '29423', 1, 0, 25000, 1100, 5.1, 67, 0, 1, 7, 8, 310),
(8, 1, 'thrax_footsoldiers_rifle', '25581', 0, 0, 30000, 1200, 5.4, 79, 0, 1, 10, 11, 300),
(9, 1, 'afs_mech_standard', '10442', 1, 0, 30000, 2000, 5.6, 293, 0, 1, 10, 11, 400),
(10, 1, 'bane_hunter_invasion', '10166', 0, 2, 100000, 1500, 5.3, 1, 0, 1, 5, 9, 275),
(11, 1, 'bane_amoeboid_invasion', '6032', 0, 0, 100000, 1500, 5.1, 6032, 1, 174, 7, 7, 350),
(12, 1, 'thrax_footsoldier_invasion', '25580', 0, 2, 100000, 700, 5.1, 1, 0, 1, 10, 15, 350);

-- --------------------------------------------------------

--
-- Table structure for table `teleporter`
--

DROP TABLE IF EXISTS `teleporter`;
CREATE TABLE IF NOT EXISTS `teleporter` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `type` int(3) NOT NULL,
  `description` varchar(50) CHARACTER SET utf8 NOT NULL,
  `px` float NOT NULL,
  `py` float NOT NULL,
  `pz` float NOT NULL,
  `dx` float NOT NULL,
  `dy` float NOT NULL,
  `dz` float NOT NULL,
  `bx` float NOT NULL,
  `bz` float NOT NULL,
  `nameId` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=45 ;

--
-- Dumping data for table `teleporter`
--

INSERT INTO `teleporter` (`id`, `type`, `description`, `px`, `py`, `pz`, `dx`, `dy`, `dz`, `bx`, `bz`, `nameId`) VALUES
(41, 8, '', 807.316, 294.055, 391.301, 0, 0, 0, 0, 0, 57),
(42, 8, '', 508.633, 238.461, 224.934, 0, 0, 0, 0, 0, 49),
(43, 8, '', 153.156, 163.004, -110.672, 0, 0, 0, 0, 0, 61),
(44, 8, '', -142.109, 220.27, -499.461, 0, 0, 0, 0, 0, 51);
