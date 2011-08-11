-- phpMyAdmin SQL Dump
-- version 3.3.0
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Aug 11, 2011 at 10:03 PM
-- Server version: 5.1.44
-- PHP Version: 5.2.13

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `infinite rasa`
--

-- --------------------------------------------------------

--
-- Table structure for table `characters`
--

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
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=59 ;

--
-- Dumping data for table `characters`
--

INSERT INTO `characters` (`id`, `name`, `lastname`, `slotID`, `userID`, `gender`, `raceID`, `classID`, `ad1_classId`, `ad1_hue`, `ad2_classId`, `ad2_hue`, `ad3_classId`, `ad3_hue`, `ad4_classId`, `ad4_hue`, `ad5_classId`, `ad5_hue`, `ad6_classId`, `ad6_hue`, `ad7_classId`, `ad7_hue`, `ad8_classId`, `ad8_hue`, `ad9_classId`, `ad9_hue`, `ad10_classId`, `ad10_hue`, `ad11_classId`, `ad11_hue`, `ad12_classId`, `ad12_hue`, `ad13_classId`, `ad13_hue`, `ad14_classId`, `ad14_hue`, `ad15_classId`, `ad15_hue`, `ad16_classId`, `ad16_hue`, `ad17_classId`, `ad17_hue`, `ad18_classId`, `ad18_hue`, `ad19_classId`, `ad19_hue`, `ad20_classId`, `ad20_hue`, `ad21_classId`, `ad21_hue`, `currentContextId`, `posX`, `posY`, `posZ`, `rotation`) VALUES
(50, 'Bootcamper', 'Warfare', 5, 1, 0, 1, 2, 18328, 4286611584, 18271, 4286611584, 18299, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3672, 4280300626, 18384, 4286611584, 18356, 4286611584, 20824, 4286886614, 0, 0, 0, 0, 0, 0, 0, 0, 1985, -218.328, 100.023, -58.4531, 0),
(49, 'Richard', 'Garriott', 4, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3672, 4280300626, 0, 0, 0, 0, 20824, 4286886614, 0, 0, 0, 0, 0, 0, 0, 0, 1991, -87.67, 10, 100, 0),
(48, 'OMG', 'WTF', 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3672, 4280300626, 0, 0, 0, 0, 20824, 4286886614, 0, 0, 0, 0, 0, 0, 0, 0, 1220, 784, 295, 380, 0),
(52, 'Kyuu', '', 1, 4, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9783, 4280300626, 0, 0, 0, 0, 7695, 4286886614, 0, 0, 25335, 4282551464, 25349, 4280300626, 0, 0, 1985, -218.328, 100.023, -58.4531, 0);

-- --------------------------------------------------------

--
-- Table structure for table `mission`
--

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

CREATE TABLE IF NOT EXISTS `npc` (
  `id` int(11) NOT NULL,
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
(4148, 'John Adams', 1985, -206.129, 98.0234, -48.1094, 0, 20975, 0, 0, 18271, 4286611584, 18299, 4286611584, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6054, 0, 9355, 4293125348, 18384, 4286611584, 18356, 4286611584, 24010, 4286886614, 20824, 2155905152, 25336, 4282551464, 25349, 4293125348, 0, 0);