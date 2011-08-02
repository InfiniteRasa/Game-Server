/*
Navicat MySQL Data Transfer

Source Server         : Localhost
Source Server Version : 50141
Source Host           : localhost:3306
Source Database       : tabuladb

Target Server Type    : MYSQL
Target Server Version : 50141
File Encoding         : 65001

Date: 2011-02-19 20:08:18
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `characters`
-- ----------------------------
DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(64) NOT NULL,
  `slotID` tinyint(4) NOT NULL,
  `userID` int(11) NOT NULL,
  `gender` tinyint(4) NOT NULL,
  `raceID` tinyint(4) NOT NULL,
  `classID` tinyint(4) NOT NULL,
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
  `currentContextId` int(11) NOT NULL COMMENT 'the current map id',
  `posX` float NOT NULL,
  `posY` float NOT NULL,
  `posZ` float NOT NULL,
  `rotation` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=32 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of characters
-- ----------------------------

-- ----------------------------
-- Table structure for `mission`
-- ----------------------------
DROP TABLE IF EXISTS `mission`;
CREATE TABLE `mission` (
  `missionId` int(11) NOT NULL,
  `dispenserNPC` int(11) NOT NULL DEFAULT '0',
  `collectorNPC` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`missionId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of mission
-- ----------------------------
INSERT INTO `mission` VALUES ('1992', '4116', '4148');

-- ----------------------------
-- Table structure for `missionstate`
-- ----------------------------
DROP TABLE IF EXISTS `missionstate`;
CREATE TABLE `missionstate` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `character` int(11) NOT NULL,
  `mission` int(11) NOT NULL,
  `status` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of missionstate
-- ----------------------------

-- ----------------------------
-- Table structure for `npc`
-- ----------------------------
DROP TABLE IF EXISTS `npc`;
CREATE TABLE `npc` (
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

-- ----------------------------
-- Records of npc
-- ----------------------------
INSERT INTO `npc` VALUES ('4116', 'NPC 1', '1985', '-221.281', '101.051', '-65.4648', '0', '20972', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '20824', '4278255360', '0', '0', '0', '0', '0', '0');
INSERT INTO `npc` VALUES ('4148', 'NPC 2', '1985', '-206.129', '98.0234', '-48.1094', '0', '20972', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '20824', '2155905152', '0', '0', '0', '0', '0', '0');
