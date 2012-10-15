/*
MySQL Data Transfer
Source Host: localhost
Source Database: ir_gameserver
Target Host: localhost
Target Database: ir_gameserver
Date: 15.10.2012 20:55:43
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for actionarguments
-- ----------------------------
DROP TABLE IF EXISTS `actionarguments`;
CREATE TABLE `actionarguments` (
  `actionId` int(11) NOT NULL,
  `actionArg` int(11) NOT NULL DEFAULT '0',
  `windupDelayMs` int(11) DEFAULT NULL,
  `windupAnimationFamilyId` int(11) DEFAULT NULL,
  `recoveryDelayMs` int(11) DEFAULT NULL,
  `recoveryAnimationFamilyId` int(11) DEFAULT NULL,
  `maxRange` int(11) DEFAULT NULL,
  `reuseTimeMs` int(11) DEFAULT NULL,
  `preload` int(11) DEFAULT NULL,
  `startReuseTimerOnPerform` int(11) DEFAULT NULL,
  PRIMARY KEY (`actionId`,`actionArg`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for actionmodules
-- ----------------------------
DROP TABLE IF EXISTS `actionmodules`;
CREATE TABLE `actionmodules` (
  `id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for characters
-- ----------------------------
DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters` (
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
) ENGINE=MyISAM AUTO_INCREMENT=65 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for creature_action
-- ----------------------------
DROP TABLE IF EXISTS `creature_action`;
CREATE TABLE `creature_action` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `description` varchar(50) DEFAULT NULL,
  `actionId` int(10) NOT NULL,
  `actionArgId` int(10) NOT NULL,
  `rangeMin` float NOT NULL,
  `rangeMax` float NOT NULL,
  `recoverTime` int(10) NOT NULL,
  `recoverTimeGlobal` int(10) NOT NULL,
  `windupTime` int(10) NOT NULL,
  `minDamage` int(10) NOT NULL,
  `maxDamage` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for creature_type
-- ----------------------------
DROP TABLE IF EXISTS `creature_type`;
CREATE TABLE `creature_type` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) CHARACTER SET utf8 NOT NULL,
  `nameId` int(10) NOT NULL,
  `classId` varchar(70) CHARACTER SET utf8 NOT NULL,
  `faction` int(4) NOT NULL,
  `walkspeed` float NOT NULL,
  `runspeed` float NOT NULL,
  `hitpoints` int(10) NOT NULL,
  `action1` int(10) DEFAULT NULL,
  `action2` int(11) DEFAULT NULL,
  `action3` int(11) DEFAULT NULL,
  `action4` int(11) DEFAULT NULL,
  `action5` int(11) DEFAULT NULL,
  `action6` int(11) DEFAULT NULL,
  `action7` int(11) DEFAULT NULL,
  `action8` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=13 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for mission
-- ----------------------------
DROP TABLE IF EXISTS `mission`;
CREATE TABLE `mission` (
  `missionId` int(11) NOT NULL,
  `dispenserNPC` int(11) NOT NULL DEFAULT '0',
  `collectorNPC` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`missionId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for missionstate
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
-- Table structure for npc
-- ----------------------------
DROP TABLE IF EXISTS `npc`;
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
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for outdated_spawntype
-- ----------------------------
DROP TABLE IF EXISTS `outdated_spawntype`;
CREATE TABLE `outdated_spawntype` (
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
) ENGINE=MyISAM AUTO_INCREMENT=13 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for pathnodes
-- ----------------------------
DROP TABLE IF EXISTS `pathnodes`;
CREATE TABLE `pathnodes` (
  `id` int(8) NOT NULL AUTO_INCREMENT,
  `spawnid` int(8) NOT NULL,
  `contextid` int(8) NOT NULL,
  `posx` float NOT NULL,
  `posy` float NOT NULL,
  `posz` float NOT NULL,
  `pindex` int(3) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for spawnpool
-- ----------------------------
DROP TABLE IF EXISTS `spawnpool`;
CREATE TABLE `spawnpool` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `mode` tinyint(2) NOT NULL,
  `animType` tinyint(2) NOT NULL,
  `posx` float NOT NULL,
  `posy` float NOT NULL,
  `posz` float NOT NULL,
  `contextid` int(5) NOT NULL,
  `creatureType1` int(10) DEFAULT NULL,
  `creatureMinCount1` int(10) DEFAULT NULL,
  `creatureMaxCount1` int(10) DEFAULT NULL,
  `creatureType2` int(10) DEFAULT NULL,
  `creatureMinCount2` int(10) DEFAULT NULL,
  `creatureMaxCount2` int(10) DEFAULT NULL,
  `creatureType3` int(10) DEFAULT NULL,
  `creatureMinCount3` int(10) DEFAULT NULL,
  `creatureMaxCount3` int(10) DEFAULT NULL,
  `creatureType4` int(10) DEFAULT NULL,
  `creatureMinCount4` int(10) DEFAULT NULL,
  `creatureMaxCount4` int(10) DEFAULT NULL,
  `creatureType5` int(10) DEFAULT NULL,
  `creatureMinCount5` int(10) DEFAULT NULL,
  `creatureMaxCount5` int(10) DEFAULT NULL,
  `creatureType6` int(10) DEFAULT NULL,
  `creatureMinCount6` int(10) DEFAULT NULL,
  `creatureMaxCount6` int(10) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=114 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for teleporter
-- ----------------------------
DROP TABLE IF EXISTS `teleporter`;
CREATE TABLE `teleporter` (
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
) ENGINE=MyISAM AUTO_INCREMENT=45 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records 
-- ----------------------------
INSERT INTO `actionarguments` VALUES ('1', '1', '0', null, '500', '1028', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '3', '0', null, '660', '739', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '9', '0', null, '2000', null, '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '66', '0', null, '1500', '491', '20', '0', '0', '0');
INSERT INTO `actionarguments` VALUES ('1', '67', '0', null, '250', '490', '20', '50', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '68', '0', null, '500', '502', '60', '150', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '78', '0', null, '1333', '623', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '79', '0', null, '700', '512', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '80', '0', null, '700', '490', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '81', '0', null, '250', '490', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '82', '0', null, '0', null, '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '84', '0', null, '3000', null, '1', '3000', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '85', '0', '619', '2966', '662', '50', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '86', '0', null, '3000', '662', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '87', '0', null, '500', '671', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '88', '0', '715', '200', '648', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '89', '0', null, '500', '519', '60', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '91', '0', null, '250', '518', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '92', '0', null, '1500', '627', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '93', '0', null, '1333', '634', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '94', '0', null, '3000', '653', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '95', '0', null, '2333', '654', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '96', '0', null, '250', '502', '60', '500', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '97', '0', null, '1333', '722', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '102', '0', null, '5000', '750', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '105', '0', null, '1000', '785', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '108', '0', null, '500', '894', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '109', '0', null, '1000', '808', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '114', '0', null, '500', '490', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '116', '0', null, '1300', '829', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '121', '0', null, '1500', '491', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '122', '0', null, '1500', '491', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '123', '0', null, '1500', '491', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '128', '0', null, '1000', '785', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '129', '0', null, '1000', '785', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '130', '0', null, '0', null, '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '131', '0', null, '2000', '780', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '132', '0', null, '1000', '785', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '133', '0', null, '250', '490', '20', '150', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '134', '0', null, '500', '502', '60', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '135', '0', null, '250', '490', '20', '100', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '136', '0', null, '500', '502', '60', '200', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '137', '0', null, '500', '519', '60', '200', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '138', '0', null, '250', '518', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '139', '0', null, '250', '518', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '141', '0', '715', '200', '648', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '144', '0', null, '500', '519', '60', '150', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '145', '0', null, '1999', '872', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '146', '0', null, '3300', '846', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '147', '0', null, '1500', '491', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '149', '1400', '1484', '3033', '888', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '151', '0', null, '1000', '887', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '152', '0', null, '1500', '887', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '153', '0', null, '1500', '891', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '154', '0', null, '1200', '874', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '160', '0', null, '3000', '909', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '162', '0', null, '500', '894', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '163', '0', null, '500', '894', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '164', '0', null, '500', '894', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '165', '0', null, '500', '894', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '168', '0', null, '500', '502', '60', '350', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '169', '0', null, '500', '502', '60', '350', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '170', '0', null, '250', '490', '20', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '171', '0', null, '250', '518', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '177', '0', null, '1000', '785', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '180', '0', null, '4000', '940', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '181', '0', null, '250', '490', '20', '200', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '182', '0', null, '500', '502', '60', '300', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '185', '0', null, '1500', '491', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '186', '0', null, '1000', '785', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '190', '0', null, '1733', '968', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '195', '0', null, '1000', null, '2', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '203', '566', '1187', '933', '980', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '204', '0', null, '1000', '995', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '207', '0', null, '0', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '209', '2966', '962', '1166', '963', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '211', '0', null, '2000', '10000012', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '212', '0', null, '0', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '213', '0', null, '0', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '214', '0', null, '0', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '215', '0', null, '0', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '216', '0', null, '0', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '217', '0', null, '0', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '218', '0', null, '0', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '222', '0', null, '0', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '224', '1000', '1023', '1000', '1508', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '225', '0', null, '0', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '227', '2966', '962', '1166', '963', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '229', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '232', '0', null, '1000', '887', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '238', '0', null, '750', '502', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '239', '0', null, '0', '861', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '241', '0', null, '1333', '1092', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '242', '0', null, '500', '1100', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '243', '0', null, '1000', '1123', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '244', '0', null, '766', '1013', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '245', '0', null, '1000', '1123', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '247', '0', null, '1333', '560', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '249', '0', null, '1000', '995', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '250', '0', null, '1333', '1260', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '252', '0', null, '1000', '1266', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '255', '500', '1322', '2500', '1321', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '261', '833', '1316', '1733', '1317', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '262', '0', null, '1300', '1346', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '263', '0', null, '2133', '739', '2', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '264', '0', null, '2133', '739', '2', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '265', '0', null, '700', '502', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '266', '0', null, '1500', '732', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '267', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '268', '3000', '1350', '1000', '727', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '270', '1499', '1091', '2000', '785', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '271', '0', null, '1500', '491', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '272', '0', null, '500', '502', '60', '150', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '273', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '274', '0', null, '500', '1087', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '275', '0', null, '933', '85', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '276', '0', null, '0', null, '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '277', '0', null, '0', '780', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '278', '0', null, '1300', '491', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '279', '0', null, '2000', '780', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '280', '0', null, '2000', '780', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '281', '1499', '1091', '1000', '785', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '282', '200', '1207', '266', '1208', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '283', '0', null, '0', null, '2', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '284', '0', null, '500', '1028', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '285', '0', null, '500', '1028', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '286', '0', null, '2000', '491', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '287', '0', null, '2500', '491', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '288', '0', null, '500', '502', '60', '300', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '289', '0', null, '500', '502', '60', '300', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '290', '0', null, '250', '490', '20', '500', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '291', '500', '1487', '500', null, '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '292', '0', null, '1000', '1330', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '293', '0', '638', '200', '637', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '295', '0', null, '1000', '785', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '296', '0', null, '500', '1028', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '297', '1000', '1023', '2000', '1025', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '298', '0', null, '1066', '1493', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '299', '0', null, '1000', null, '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '300', '2000', '1506', '2000', '1505', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '301', '0', '1514', '200', '1190', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '302', '0', null, '2000', '1194', '50', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '303', '1000', '1023', '1000', '1508', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '304', '0', null, '1000', '953', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '305', '0', null, '1000', '785', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('1', '306', '0', '1514', '1000', '1190', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('2', '1', '2666', '1178', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '2', '5994', '1183', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '4', '3663', '1106', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '6', '2500', '1188', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '7', '2266', '1186', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '8', '3996', '1128', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '9', '2830', '1118', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '10', '2566', '1197', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '11', '1933', '1198', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '12', '5000', '1127', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '13', '1833', '1200', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '14', '6166', '1196', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '15', '4150', '1179', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '16', '3164', '1172', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '17', '9000', '1177', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '18', '2400', '1180', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '19', '4600', '1202', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '20', '3830', '1117', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '21', '2000', '1181', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '22', '2997', '1174', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '23', '3164', '1176', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '24', '2166', '1126', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '25', '3000', '1120', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '26', '0', null, '-1', '1121', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '27', '10323', '1182', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '28', '5000', '1173', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '30', '6327', '1201', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '31', '9158', '1119', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '33', '4733', '1199', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '34', '6400', '1125', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '35', '4666', '1159', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '36', '11322', '1210', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '37', '9833', '1297', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '38', '3000', '1298', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '39', '12000', '1299', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '40', '9324', '1306', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '41', '9333', '1308', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '42', '2000', '1311', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '43', '4000', '1309', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '44', '6993', '1338', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '45', '4333', '1339', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '46', '12000', '1299', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '47', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '48', '4828', '1313', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '49', '0', null, '-1', '1329', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '50', '2833', '1327', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '51', '2166', '1383', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '52', '8666', '1384', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '53', '4662', '1382', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '55', '4662', null, '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '56', '3000', '1372', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '57', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '58', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '59', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '60', '3000', '1328', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '61', '9324', '1306', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '64', '4833', '1363', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '67', '0', null, '-1', '1369', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '68', '1800', '1377', '-1', '1380', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '72', '9324', '1306', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '73', '9324', '1306', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '74', '9324', '1306', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '75', '0', null, '-1', '1387', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '76', '2500', '1388', '-1', '1389', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '77', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '78', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '79', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '80', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '81', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '82', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '83', '2166', '1392', '-1', '1393', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '84', '2500', '1394', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '85', '0', null, '-1', '1395', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '86', '2000', '1360', '-1', '1381', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '87', '3000', '1396', '-1', '1397', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '88', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '89', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '90', '2166', '1413', '-1', '1414', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '91', '1900', '1415', '-1', '1416', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '92', '3000', '1417', '-1', '1418', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '93', '7700', '1423', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '94', '9200', '1429', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '95', '2966', '1431', '-1', '1432', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '96', '6000', '1312', '0', null, '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '97', '2000', '1436', '-1', '1437', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '98', '0', null, '-1', '1440', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('2', '99', '14666', '1510', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('74', '1', '0', null, '0', null, '1000', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('80', '1', '0', '154', '500', '166', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('80', '2', '0', '154', '500', '166', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('80', '3', '0', '154', '500', '166', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('80', '4', '0', '154', '500', '166', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('80', '5', '0', '154', '500', '166', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('80', '6', '0', '1184', '0', '1185', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('80', '7', '0', '1264', '0', '1263', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('81', '1', '250', null, '500', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('82', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('82', '2', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('82', '3', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('82', '4', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('82', '5', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('82', '6', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('82', '10', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('83', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('84', '1', '0', null, '0', null, '1000', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('84', '2', '0', null, '0', null, '1000', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('102', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('102', '2', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('102', '3', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('102', '4', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('102', '5', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('102', '6', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('102', '7', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '2', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '3', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '4', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '5', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '6', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '7', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '8', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '9', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '10', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '11', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '12', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '13', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '14', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '15', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '16', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '17', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '18', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '19', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '20', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '21', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '22', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '23', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '24', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '25', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '26', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '27', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '28', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '29', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '30', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '31', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '32', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '33', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '34', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '35', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '36', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '37', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '38', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '39', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '40', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '41', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '42', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '43', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '44', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '45', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '46', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '47', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '48', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '49', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '50', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '51', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '52', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '53', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '54', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '56', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('128', '57', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '1', '0', null, '1500', '498', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '2', '0', null, '1132', '500', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '3', '0', null, '1500', '514', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '4', '0', null, '1500', '567', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '5', '0', null, '1500', '568', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '6', '0', null, '1999', '610', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '7', '0', null, '1132', '632', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '8', '0', null, '1000', '749', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '9', '0', null, '1666', '757', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '14', '0', null, '2000', '779', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '15', '0', null, '2000', '610', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '16', '0', null, '2166', '765', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '17', '0', null, '1000', '1131', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '18', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '19', '0', null, '1500', '514', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '20', '0', null, '1500', '567', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '21', '0', null, '1500', '567', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '22', '0', null, '1000', '632', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '23', '0', null, '2000', '632', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('129', '24', '0', null, '1500', '1479', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '1', '0', null, '1500', '499', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '2', '0', null, '1000', '501', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '3', '0', null, '900', '513', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '4', '0', null, '1500', '566', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '5', '0', null, '1500', '569', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '6', '0', null, '800', '609', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '7', '0', null, '1000', '631', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '8', '0', null, '666', '748', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '9', '0', null, '1333', '753', '1', '2000', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '10', '0', null, '1333', '764', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '14', '0', null, '1500', '778', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '15', '0', null, '800', '609', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '16', '0', null, '800', '1130', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '17', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '18', '0', null, '1500', '513', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '19', '0', null, '3000', '566', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '20', '0', null, '3000', '566', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '21', '0', null, '1000', '631', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '22', '0', null, '1000', '632', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '23', '0', null, '1000', '631', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '24', '0', null, '2000', '631', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('130', '25', '0', null, '1000', '1483', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '1', '1500', '557', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '2', '1333', '558', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '3', '2166', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '4', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '5', '2000', '562', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '6', '2333', '633', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '9', '2166', '767', '0', null, '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '10', '2000', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '11', '2166', '583', '0', null, '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '12', '2000', '633', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '13', '2000', '654', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '14', '1500', '557', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '15', '1500', '627', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '16', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '17', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '18', '500', '1026', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '19', '500', '1027', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '20', '2000', '633', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '21', '2000', '633', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '22', '2000', '633', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '23', '2000', '654', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '24', '2000', '654', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '25', '2000', '654', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '26', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '27', '3000', '653', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '28', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '29', '3000', '653', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '30', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '31', '3000', '653', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '32', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '33', '3000', '653', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '34', '2000', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '35', '2000', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '36', '2000', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '37', '2000', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '38', '2333', '633', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '39', '2333', '654', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '40', '2333', '633', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '41', '2333', '654', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '42', '2333', '633', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '43', '2333', '633', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '44', '2000', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '45', '2000', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '46', '2000', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '47', '1500', '557', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '48', '1500', '627', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '49', '1500', '557', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '50', '1500', '627', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '51', '1500', '557', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '52', '1500', '627', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '53', '1500', '557', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '54', '1500', '627', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '55', '2000', '562', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '56', '2000', '562', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '57', '2000', '562', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '58', '2000', '562', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '59', '1333', '558', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '60', '1333', '634', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '61', '1333', '558', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '62', '1333', '634', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '63', '1333', '558', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '64', '1333', '634', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '65', '1333', '558', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '66', '1333', '634', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '67', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '68', '3000', '653', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '69', '3000', '653', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '70', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '71', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '72', '3000', '653', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '73', '2166', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '74', '2166', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '75', '2166', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '76', '2166', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '77', '2166', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '78', '1966', '729', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '79', '1666', '729', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '80', '1666', '627', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '81', '1666', '627', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '82', '1666', '729', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '83', '1666', '627', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '84', '1666', '729', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '85', '1666', '627', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '86', '1666', '729', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '87', '1666', '627', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '88', '2164', '767', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '89', '2166', '767', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '90', '2166', '767', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '91', '3000', '559', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '92', '0', null, '3996', '653', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '93', '1500', '1481', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '94', '3000', '633', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('134', '95', '2000', '556', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('137', '1', '0', null, '2300', '593', '20', '300000', '0', '1');
INSERT INTO `actionarguments` VALUES ('140', '1', '0', '664', '0', '561', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('140', '2', '0', '664', '0', '561', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('140', '3', '0', '664', '0', '561', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('140', '4', '0', '664', '0', '561', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('140', '5', '0', '664', '0', '561', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('140', '6', '0', '664', '0', '561', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '1', '0', '578', '1333', '560', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '2', '0', '578', '1333', '560', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '3', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '4', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '5', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '6', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '7', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '8', '0', '578', '1333', '560', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '9', '0', '578', '1333', '560', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '10', '0', '578', '1333', '560', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '11', '833', '1319', '2000', '1318', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '12', '0', '578', '1333', '560', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '13', '0', '578', '1333', '560', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '14', '1', '566', '1', '560', '60', '0', '0', '0');
INSERT INTO `actionarguments` VALUES ('141', '15', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '16', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '17', '2000', '1435', '1666', '1025', '80', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '18', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('141', '19', '500', '1516', '1500', '1515', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('142', '1', '0', null, '0', null, '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('146', '1', '0', null, '0', '601', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('147', '1', '0', null, '2000', '572', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('147', '2', '4000', '750', '2200', '767', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('147', '3', '4000', '750', '2200', '767', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('147', '4', '0', null, '2000', '572', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('147', '5', '0', null, '2000', '572', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('147', '6', '0', null, '2000', '572', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '1', '0', '638', '200', '637', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '3', '0', '638', '200', '637', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '4', '0', '638', '200', '637', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '5', '0', '638', '200', '637', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '6', '0', '1398', '1000', '819', '4', '100', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '7', '200', '638', '200', '637', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '8', '200', '638', '200', '637', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '9', '250', '493', '250', '490', '20', '500', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '10', '500', '1487', '0', null, '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '11', '500', '1023', '200', '1508', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '12', '1000', '1023', '2000', '1508', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '13', '0', '638', '200', '637', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '14', '0', '638', '200', '637', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '15', '0', '638', '200', '637', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('149', '16', '0', null, '750', null, '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('150', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('153', '1', '0', null, '0', null, '1000', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '3762', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '3781', '0', null, '20000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '3816', '0', null, '19966', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '3868', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '3897', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '3901', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '3902', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '3981', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '3982', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '4046', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '4047', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '4064', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '4261', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '4313', '0', null, '3500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6021', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6029', '0', null, '1660', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6031', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6032', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6038', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6039', '0', null, '1260', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6040', '0', null, '3000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6043', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6163', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6167', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6170', '0', null, '2750', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6203', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6207', '0', null, '1500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6236', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6239', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6262', '0', null, '2666', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6263', '0', null, '2666', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6332', '0', null, '1500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6338', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6415', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6421', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6441', '0', null, '4666', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6960', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '6961', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7043', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7078', '0', null, '3366', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7079', '0', null, '3366', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7081', '0', null, '3366', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7120', '0', null, '1900', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7233', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7234', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7235', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7236', '0', null, '3000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7253', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7254', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7280', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7336', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7338', '0', null, '4666', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7339', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7340', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7344', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7477', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7482', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7500', '0', null, '1999', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7510', '0', null, '3600', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7528', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7586', '0', null, '3600', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7696', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7722', '0', null, '12000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7747', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '7977', '0', null, '6300', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '9244', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '9284', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '9354', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '9554', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '9616', '0', null, '1999', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '9804', '0', null, '13000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '10164', '0', null, '1666', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '10225', '0', null, '16000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '10230', '0', null, '2500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '10235', '0', null, '6733', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '10310', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '10312', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '10521', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '10700', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '10997', '0', null, '1500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '11065', '0', null, '16000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '11066', '0', null, '16000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '11067', '0', null, '16000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '11068', '0', null, '16000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '13006', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '20359', '0', null, '2999', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '20698', '0', null, '6230', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '20768', '0', null, '2650', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '21499', '0', null, '7200', '1211', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '21795', '0', null, '2666', '1236', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '21796', '0', null, '2666', '1237', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '21797', '0', null, '2666', '1238', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '21798', '0', null, '2666', '1239', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '21799', '0', null, '2666', '1236', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '21800', '0', null, '2666', '1237', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '21801', '0', null, '2666', '1238', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '21802', '0', null, '2666', '1239', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '23125', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '24092', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '24093', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '24883', '0', null, '5999', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '25539', '0', null, '2333', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '25540', '0', null, '2333', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '25541', '0', null, '2333', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '25542', '0', null, '2333', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '25610', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '26022', '0', null, '3000', '1348', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '26023', '0', null, '3000', '1348', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '26024', '0', null, '3000', '1348', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '26025', '0', null, '3000', '1348', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '26582', '0', null, '0', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('155', '26583', '0', null, '1000', '622', '0', '0', '0', '0');
INSERT INTO `actionarguments` VALUES ('155', '30445', '0', null, '1000', '622', '0', '0', '0', '0');
INSERT INTO `actionarguments` VALUES ('158', '1', '500', '1063', '700', '1064', '60', '1500', '0', '1');
INSERT INTO `actionarguments` VALUES ('158', '2', '500', '1063', '700', '1064', '60', '1500', '0', '1');
INSERT INTO `actionarguments` VALUES ('158', '3', '500', '1063', '700', '1064', '60', '1500', '0', '1');
INSERT INTO `actionarguments` VALUES ('158', '4', '500', '1063', '700', '1064', '60', '1500', '0', '1');
INSERT INTO `actionarguments` VALUES ('158', '5', '500', '1063', '700', '1064', '60', '1500', '0', '1');
INSERT INTO `actionarguments` VALUES ('158', '6', '1000', '1520', '1500', '1521', '60', '3000', '0', '1');
INSERT INTO `actionarguments` VALUES ('158', '7', '1000', '1520', '1500', '1521', '60', '3000', '0', '1');
INSERT INTO `actionarguments` VALUES ('159', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('162', '1', '500', '1063', '500', '1064', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('162', '2', '500', '1063', '500', '1064', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('162', '3', '500', '1063', '500', '1064', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('162', '4', '500', '1063', '500', '1064', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('162', '5', '500', '1063', '500', '1064', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('162', '6', '500', '1063', '500', '1064', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('162', '7', '500', '1063', '500', '1064', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('166', '1', '0', null, '3333', '630', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('167', '1', '0', null, '7333', '629', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('168', '1', '3000', '650', '0', null, '0', '3000', '0', '1');
INSERT INTO `actionarguments` VALUES ('169', '1', '3000', '651', '0', null, '0', '3000', '0', '1');
INSERT INTO `actionarguments` VALUES ('171', '1', '1233', '663', '3000', '922', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('171', '2', '1233', '663', '3000', '922', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('171', '3', '1233', '663', '3000', '922', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('171', '4', '1233', '663', '3000', '922', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('171', '5', '1233', '663', '3000', '922', '3', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('171', '6', '1233', '663', '3000', '922', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('172', '1', '4000', '750', '2200', '767', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('172', '2', '0', null, '0', '758', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('172', '3', '4000', '750', '2200', '767', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('172', '4', '4000', '750', '2200', '767', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('172', '5', '4000', '750', '2200', '767', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('172', '11', '4000', '750', '2200', '767', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('172', '12', '4000', '750', '2200', '767', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('172', '168', '4000', '750', '2200', '767', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('172', '169', '4000', '750', '2200', '767', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '1', '0', null, '500', '739', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '2', '0', null, '400', '1205', '5', '10', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '3', '200', '1050', '133', '988', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '4', '200', '1048', '266', '983', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '5', '200', '994', '133', '986', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '6', '200', '1040', '133', '984', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '7', '200', '1046', '133', '985', '5', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '8', '200', '1044', '133', '987', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '9', '0', null, '2833', '858', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '10', '0', null, '1333', '815', '2', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '11', '0', null, '2200', '792', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '12', '1000', '879', '1900', '876', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '13', '0', null, '2000', '859', '2', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '14', '0', null, '2000', '822', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '17', '0', null, '1500', '861', '2', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '18', '0', null, '1233', '841', '2', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '24', '0', null, '2150', '913', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '25', '0', null, '1733', '875', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '26', '0', null, '2133', '899', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '27', '0', null, '1500', '900', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '28', '0', null, '2100', '882', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '29', '0', null, '1733', '998', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '30', '0', null, '1733', '1060', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '31', '0', null, '1200', '1032', '5', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '32', '4', '926', '5', '926', '0', '2', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '33', '333', '1010', '933', '1012', '2', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '34', '0', null, '2633', '734', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '35', '0', null, '1600', '965', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '36', '0', null, '1800', '952', '0', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '37', '0', null, '2666', '1007', '2', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '38', '1000', '1080', '1750', '1082', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '39', '0', null, '1600', '1085', '2', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '44', '0', null, '2100', '656', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '45', '0', null, '1500', '887', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '46', '0', null, '1300', '983', '5', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '47', '0', null, '1070', '986', '2', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '48', '0', null, '1300', '1155', '2', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '49', '0', null, '1600', '85', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '50', '0', null, '1666', '1262', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '51', '0', null, '2000', '1265', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '52', '0', null, '1366', '1320', '3', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '265', '0', null, '1300', '1155', '2', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '266', '200', '1268', '233', '1269', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '282', '0', null, '400', '1205', '5', '10', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '283', '0', null, '400', '1205', '5', '10', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '284', '0', null, '400', '1205', '5', '10', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '285', '0', null, '400', '1205', '5', '10', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '286', '0', null, '400', '1205', '5', '10', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '287', '233', '1207', '133', '1208', '4', '250', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '288', '0', null, '500', '1205', '5', '10', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '289', '0', null, '2000', '1193', '5', '260', '0', '1');
INSERT INTO `actionarguments` VALUES ('174', '290', '1466', '1504', '0', null, '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('175', '1', '2500', '914', '500', '915', '0', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('175', '2', '2500', '914', '500', '915', '0', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('175', '3', '2500', '914', '500', '915', '0', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('175', '4', '2500', '914', '500', '915', '0', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('175', '5', '2500', '914', '500', '915', '0', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('176', '1', '0', null, '2300', '847', '0', '300000', '0', '1');
INSERT INTO `actionarguments` VALUES ('177', '1', '400', '1069', '400', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('177', '2', '400', '1069', '400', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('177', '3', '400', '1069', '400', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('177', '4', '400', '1069', '400', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('177', '5', '400', '1069', '400', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('177', '6', '400', '1069', '400', '1070', '0', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('178', '1', '500', '1069', '666', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('178', '2', '500', '1069', '666', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('178', '3', '500', '1069', '666', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('178', '4', '500', '1069', '666', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('178', '5', '500', '1069', '666', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('178', '6', '500', '1520', '666', '1521', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('179', '1', '0', '1254', '500', '1253', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('179', '2', '0', '1254', '500', '1253', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('179', '3', '0', '1254', '500', '1253', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('179', '4', '0', '1254', '500', '1253', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('179', '5', '0', '1254', '500', '1253', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('179', '6', '0', '1254', '500', '1253', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('179', '7', '0', null, '500', '846', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('179', '8', '0', '1519', '1000', '1524', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('179', '9', '0', '1514', '200', '1190', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('180', '1', '4333', '1139', '4000', '1140', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('180', '2', '4333', '1139', '4000', '1140', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('180', '3', '4333', '1139', '4000', '1140', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('180', '4', '4333', '1139', '4000', '1140', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('181', '1', '0', null, '3533', '823', '40', '8000', '0', '1');
INSERT INTO `actionarguments` VALUES ('181', '2', '0', null, '3533', '823', '40', '8000', '0', '1');
INSERT INTO `actionarguments` VALUES ('181', '3', '3000', '824', '3533', '823', '40', '8000', '0', '1');
INSERT INTO `actionarguments` VALUES ('182', '1', '1500', '814', '3000', '813', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('183', '1', '3000', '826', '1000', '825', '4', '100', '0', '1');
INSERT INTO `actionarguments` VALUES ('183', '2', '2000', '826', '3000', '825', '4', '100', '0', '1');
INSERT INTO `actionarguments` VALUES ('183', '3', '2000', '826', '3000', '825', '4', '100', '0', '1');
INSERT INTO `actionarguments` VALUES ('183', '4', '2000', '826', '3000', '825', '4', '100', '0', '1');
INSERT INTO `actionarguments` VALUES ('184', '1', '0', null, '1000', '612', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('184', '2', '0', null, '1000', '176', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('185', '1', '500', '1248', '700', '1250', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('185', '2', '500', '1248', '700', '1250', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('185', '3', '500', '1248', '700', '1250', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('185', '4', '500', '1248', '700', '1250', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('185', '5', '500', '1248', '700', '1250', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('186', '1', '300', '1420', '300', '916', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('186', '2', '300', '1069', '300', '1070', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('186', '3', '300', '1420', '300', '916', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('186', '4', '300', '1420', '300', '916', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('186', '5', '300', '1069', '300', '1070', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('186', '6', '800', '1517', '800', '1518', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('187', '1', '500', '1083', '500', '1084', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('187', '2', '500', '1083', '500', '1084', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('187', '3', '500', '1083', '500', '1084', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('187', '4', '500', '1083', '500', '1084', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('187', '5', '500', '1083', '500', '1084', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('188', '1', '300', '1069', '300', '1070', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('188', '2', '300', '1069', '300', '1070', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('188', '3', '300', '1069', '300', '1070', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('188', '4', '300', '1069', '300', '1070', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('188', '5', '300', '1069', '300', '1070', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('189', '1', '630', '842', '530', '935', '20', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('189', '2', '630', '842', '530', '935', '20', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('190', '1', '3299', '855', '533', '927', '20', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('191', '1', '2799', '853', '4033', '936', '40', '20000', '0', '1');
INSERT INTO `actionarguments` VALUES ('192', '1', '1000', '854', '5199', '937', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('192', '2', '1000', '854', '5199', '937', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('193', '1', '0', null, '2500', '847', '40', '300000', '0', '1');
INSERT INTO `actionarguments` VALUES ('194', '1', '500', '1063', '700', '1064', '60', '1200', '0', '1');
INSERT INTO `actionarguments` VALUES ('194', '2', '500', '1063', '700', '1064', '60', '1200', '0', '1');
INSERT INTO `actionarguments` VALUES ('194', '3', '500', '1063', '700', '1064', '60', '1200', '0', '1');
INSERT INTO `actionarguments` VALUES ('194', '4', '500', '1063', '700', '1064', '60', '1200', '0', '1');
INSERT INTO `actionarguments` VALUES ('194', '5', '500', '1063', '700', '1064', '60', '1200', '0', '1');
INSERT INTO `actionarguments` VALUES ('194', '6', '0', null, '700', '1064', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('194', '7', '500', '1063', '700', '1064', '60', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('196', '1', '825', '862', '500', '928', '20', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('196', '2', '825', '862', '500', '928', '20', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('197', '1', '666', '1065', '666', '1066', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('197', '2', '666', '1065', '666', '1066', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('197', '3', '666', '1065', '666', '1066', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('197', '4', '666', '1065', '666', '1066', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('197', '5', '666', '1065', '666', '1066', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('197', '6', '666', '1065', '666', '1066', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('198', '1', '0', null, '1500', '750', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('198', '2', '0', null, '1500', '750', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('198', '3', '0', null, '1500', '750', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('198', '4', '0', null, '1500', '750', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('198', '5', '0', null, '1500', '785', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('199', '1', '0', null, '1500', '1124', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('200', '1', '925', '901', '1720', '932', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('200', '2', '925', '901', '1720', '932', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('200', '3', '925', '901', '1720', '932', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('200', '4', '925', '901', '1720', '932', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('200', '5', '925', '901', '1720', '932', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('200', '6', '925', '901', '1720', '932', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('200', '7', '925', '901', '1720', '932', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('201', '1', '0', null, '1500', '924', '5', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('201', '2', '0', null, '1500', '924', '5', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('201', '3', '0', null, '1500', '924', '5', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('201', '4', '0', null, '1500', '924', '5', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('202', '1', '2266', '902', '2066', '933', '10', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('202', '2', '2266', '902', '2066', '933', '10', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('202', '3', '2266', '902', '2066', '933', '10', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('202', '4', '2266', '902', '2066', '933', '10', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('203', '1', '1666', '864', '1333', '931', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('203', '2', '1666', '864', '1333', '931', '40', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('203', '3', '1666', '864', '1333', '931', '40', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('203', '4', '1666', '864', '1333', '931', '40', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('203', '5', '1666', '864', '1333', '931', '40', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('204', '1', '1500', '863', '500', '930', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('204', '2', '500', '955', '700', '946', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('205', '1', '660', '879', '1900', '876', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('205', '2', '660', '879', '1900', '876', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('205', '3', '660', '879', '1900', '876', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('205', '4', '660', '879', '1900', '876', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('207', '1', '1066', '999', '1200', '870', '1', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('208', '1', '600', '884', '4100', '934', '10', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('208', '2', '600', '884', '4100', '934', '10', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('208', '3', '600', '884', '4100', '934', '10', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('208', '4', '600', '884', '4100', '934', '10', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('209', '1', '825', '860', '2140', '925', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('209', '2', '825', '860', '2140', '925', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('209', '3', '825', '860', '2140', '925', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('209', '4', '825', '860', '2140', '925', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('210', '1', '1200', '844', '1233', '845', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('210', '2', '1200', '844', '1233', '845', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('211', '1', '1800', '895', '1150', '923', '2', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('211', '2', '1800', '895', '1150', '923', '2', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('211', '3', '1800', '895', '1150', '923', '2', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('211', '4', '1800', '895', '1150', '923', '2', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('213', '1', '0', '1514', '200', '1190', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('229', '1', '333', '1083', '333', '1084', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('229', '2', '333', '1083', '333', '1084', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('229', '3', '333', '1083', '333', '1084', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('229', '4', '333', '1083', '333', '1084', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('229', '5', '333', '1083', '333', '1084', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('230', '1', '0', null, '1000', '1330', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('230', '2', '0', null, '1000', '1330', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('230', '3', '0', null, '1000', '1330', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('230', '4', '0', null, '1000', '1330', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('230', '5', '0', null, '1000', '1330', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('230', '6', '0', null, '1000', '1330', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('231', '1', '500', '1069', '400', '1070', '20', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('231', '2', '500', '1069', '400', '1070', '20', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('231', '3', '500', '1069', '400', '1070', '20', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('231', '4', '500', '1069', '400', '1070', '20', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('231', '5', '500', '1069', '400', '1070', '20', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('232', '1', '333', '1083', '999', '945', '20', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('232', '2', '333', '1083', '999', '945', '20', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('232', '3', '333', '1083', '999', '945', '20', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('232', '4', '333', '1083', '999', '945', '20', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('232', '5', '333', '1083', '999', '945', '20', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('232', '6', '333', '1083', '999', '945', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('233', '1', '2466', '827', '500', '916', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('233', '2', '2466', '827', '500', '916', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('233', '3', '2466', '827', '500', '916', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('233', '4', '2466', '827', '500', '916', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('233', '5', '2466', '827', '500', '916', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('234', '1', '0', null, '2300', '593', '20', '300000', '0', '1');
INSERT INTO `actionarguments` VALUES ('234', '2', '0', null, '2300', '593', '20', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('235', '1', '166', '938', '966', '943', '1', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('235', '2', '166', '938', '966', '943', '1', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('236', '1', '825', '890', '500', '926', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('237', '1', '925', '898', '1720', '929', '40', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('237', '2', '925', '898', '1720', '929', '40', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('237', '3', '925', '898', '1720', '929', '40', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('237', '4', '925', '898', '1720', '929', '40', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('238', '1', '1060', '911', '1560', '912', '40', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('238', '2', '1060', '911', '1560', '912', '40', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('238', '3', '1060', '911', '1560', '912', '40', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('238', '4', '1060', '911', '1560', '912', '40', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('239', '1', '2499', '955', '1733', '946', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('239', '2', '2499', '955', '1733', '946', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('239', '3', '2499', '955', '1733', '946', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('239', '4', '2499', '955', '1733', '946', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('239', '5', '2499', '955', '1733', '946', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('239', '6', '2499', '955', '1733', '946', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('239', '7', '1066', '1512', '1900', '1191', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('240', '1', '1000', '1248', '966', '1250', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('240', '2', '1000', '1248', '966', '1250', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('240', '3', '1000', '1248', '966', '1250', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('240', '4', '1000', '1248', '966', '1250', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('240', '5', '1000', '1248', '966', '1250', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('242', '1', '2850', '957', '1300', '956', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('242', '2', '2850', '957', '1300', '956', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('242', '3', '2850', '957', '1300', '956', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('242', '4', '2850', '957', '1300', '956', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('242', '5', '2850', '957', '1300', '956', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('242', '6', '2850', '957', '1300', '956', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('243', '1', '1166', '1433', '2800', '958', '50', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('243', '2', '1166', '1433', '2800', '958', '30', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('244', '1', '0', null, '6733', '959', '1000', '12000', '0', '1');
INSERT INTO `actionarguments` VALUES ('244', '2', '0', null, '6733', '959', '1000', '12000', '0', '1');
INSERT INTO `actionarguments` VALUES ('245', '1', '0', null, '0', null, '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('246', '1', '500', '1063', '700', '1064', '20', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('246', '2', '500', '1063', '700', '1064', '20', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('246', '3', '500', '1063', '700', '1064', '20', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('246', '4', '500', '1063', '700', '1064', '20', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('246', '5', '500', '1063', '700', '1064', '20', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('248', '1', '2966', '962', '1166', '963', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('248', '2', '2966', '962', '1166', '963', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('248', '3', '2966', '962', '1166', '963', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('249', '1', '0', '638', '200', '637', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('249', '2', '0', '638', '200', '637', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('249', '3', '0', '638', '200', '637', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('249', '101', '0', '715', '200', '648', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('249', '102', '0', '715', '200', '648', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('249', '103', '0', '715', '200', '648', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('249', '104', '0', '638', '200', '637', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('249', '105', '0', '638', '200', '637', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('249', '106', '0', '638', '200', '637', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('251', '1', '500', '1248', '700', '1250', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('251', '2', '500', '1248', '700', '1250', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('251', '3', '500', '1248', '700', '1250', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('251', '4', '500', '1248', '700', '1250', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('251', '5', '500', '1248', '700', '1250', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('251', '6', '800', '1517', '800', '1518', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('252', '1', '0', null, '2300', '847', '0', '300000', '0', '1');
INSERT INTO `actionarguments` VALUES ('253', '1', '1666', '871', '500', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('253', '2', '1666', '871', '500', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('253', '3', '1666', '871', '500', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('253', '4', '1666', '871', '500', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('253', '5', '1666', '871', '500', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('254', '1', '0', null, '2333', '997', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('255', '1', '1500', '863', '670', '930', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('255', '2', '1500', '863', '670', '930', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('255', '3', '1500', '863', '670', '930', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('255', '4', '1500', '863', '670', '930', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('255', '5', '1500', '863', '670', '930', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('258', '14', '4000', '750', '0', null, '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('259', '1', '500', '1063', '1700', '1064', '60', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('259', '2', '500', '1063', '700', '1064', '60', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('259', '3', '500', '1063', '700', '1064', '60', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('259', '4', '500', '1063', '700', '1064', '60', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('259', '5', '500', '1063', '700', '1064', '60', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('259', '6', '1000', '1248', '1000', '1250', '30', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('260', '1', '0', null, '2300', '847', '40', '300000', '0', '1');
INSERT INTO `actionarguments` VALUES ('262', '1', '900', '871', '766', '945', '20', '60000', '0', '0');
INSERT INTO `actionarguments` VALUES ('262', '2', '900', '871', '766', '945', '20', '60000', '0', '0');
INSERT INTO `actionarguments` VALUES ('262', '3', '900', '871', '766', '945', '20', '60000', '0', '0');
INSERT INTO `actionarguments` VALUES ('262', '4', '900', '871', '766', '945', '20', '60000', '0', '0');
INSERT INTO `actionarguments` VALUES ('262', '5', '900', '871', '766', '945', '20', '60000', '0', '0');
INSERT INTO `actionarguments` VALUES ('263', '1', '7333', '1158', '4333', '1014', '40', '20000', '0', '1');
INSERT INTO `actionarguments` VALUES ('263', '2', '7333', '1158', '4333', '1014', '40', '20000', '0', '1');
INSERT INTO `actionarguments` VALUES ('263', '3', '7333', '1158', '4333', '1014', '40', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('264', '1', '2633', '1203', '1833', '1015', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('264', '2', '2633', '1203', '1833', '1015', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('264', '3', '2633', '1203', '1833', '1015', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('264', '4', '2633', '1203', '1933', '1015', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('264', '5', '2633', '1203', '1933', '1015', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('266', '1', '0', null, '5333', null, '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('267', '1', '1500', '1420', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('267', '2', '1500', '1420', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('267', '3', '1500', '1420', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('267', '4', '1500', '1420', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('267', '5', '1500', '1420', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('268', '1', '0', null, '2000', '1030', '3', '2500', '0', '1');
INSERT INTO `actionarguments` VALUES ('269', '1', '2766', '1001', '2566', '1000', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('269', '2', '2766', '1001', '2566', '1000', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('270', '1', '2733', '1003', '3933', '1002', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('270', '2', '2733', '1003', '3933', '1002', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('271', '1', '825', '862', '500', '928', '40', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('272', '1', '0', null, '3330', '1029', '20', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('273', '1', '1999', '893', '1999', '892', '60', '6000', '0', '1');
INSERT INTO `actionarguments` VALUES ('273', '2', '1999', '893', '1999', '892', '60', '6000', '0', '1');
INSERT INTO `actionarguments` VALUES ('273', '3', '1999', '893', '1999', '892', '60', '6000', '0', '1');
INSERT INTO `actionarguments` VALUES ('273', '4', '1999', '893', '1999', '892', '60', '6000', '0', '1');
INSERT INTO `actionarguments` VALUES ('274', '1', '3600', '1041', '500', '1042', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('274', '2', '3600', '1041', '500', '1042', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('274', '3', '3600', '1041', '500', '1042', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('274', '4', '3600', '1041', '500', '1042', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('275', '1', '1670', '1037', '2000', '1038', '20', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('275', '2', '1670', '1037', '2000', '1038', '30', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('275', '3', '1670', '1037', '2000', '1038', '20', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('275', '4', '1670', '1037', '2000', '1038', '30', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('275', '5', '1670', '1037', '2000', '1038', '0', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('275', '6', '1670', '1037', '2000', '1038', '30', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('276', '1', '2300', '1036', '2350', '1035', '40', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('276', '2', '2300', '1036', '2350', '1035', '40', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('276', '3', '2300', '1036', '2350', '1035', '40', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('276', '4', '2300', '1036', '2350', '1035', '40', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('276', '5', '2300', '1036', '2350', '1035', '40', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('277', '1', '4866', '1135', '1133', '1136', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('277', '2', '4866', '1135', '1133', '1136', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('278', '1', '500', '1344', '700', '1345', '0', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('278', '2', '500', '1344', '700', '1345', '0', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('278', '3', '500', '1344', '700', '1345', '0', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('279', '1', '3000', '1036', '466', '1035', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('280', '1', '1000', '1245', '1000', null, '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('281', '1', '0', null, '2300', '593', '0', '300000', '0', '1');
INSERT INTO `actionarguments` VALUES ('282', '1', '500', '1083', '500', '945', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('282', '2', '500', '1083', '500', '945', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('282', '3', '500', '1083', '500', '945', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('282', '4', '500', '1083', '500', '945', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('282', '5', '500', '1083', '500', '945', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('283', '1', '0', null, '2400', '1059', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('284', '1', '1866', '1024', '2133', '1434', '10', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('284', '2', '1866', '1024', '2133', '1434', '5', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('284', '3', '1866', '1024', '2133', '1434', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('284', '4', '1866', '1024', '2133', '1434', '10', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('285', '1', '0', null, '2333', '1053', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('286', '1', '766', '1294', '1533', '1052', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('287', '1', '0', null, '2999', '1054', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('288', '1', '0', null, '1066', '1296', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('288', '2', '0', null, '1066', '1296', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('288', '3', '0', null, '1066', '1296', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('288', '4', '0', null, '1066', '1296', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('289', '1', '1000', '1055', '1999', '1055', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('292', '1', '1366', '1402', '1033', '1401', '50', '3000', '0', '1');
INSERT INTO `actionarguments` VALUES ('292', '2', '1366', '1402', '1033', '1401', '50', '3000', '0', '1');
INSERT INTO `actionarguments` VALUES ('292', '3', '1366', '1402', '1033', '1401', '50', '3000', '0', '1');
INSERT INTO `actionarguments` VALUES ('292', '4', '1366', '1402', '1033', '1401', '50', '3000', '0', '1');
INSERT INTO `actionarguments` VALUES ('293', '1', '2000', '1435', '1666', '1025', '50', '3000', '0', '1');
INSERT INTO `actionarguments` VALUES ('293', '2', '0', null, '1666', '1025', '80', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('293', '3', '0', null, '1666', '1025', '50', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '1', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '2', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '3', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '4', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '5', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '6', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '7', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '8', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '9', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '10', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '11', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '12', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '13', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '14', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '15', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '16', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '17', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '18', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '19', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '20', '0', null, '2999', '1057', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('294', '21', '2300', '1036', '2350', '1035', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('295', '1', '500', '1063', '666', '1064', '100', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('295', '2', '500', '1063', '666', '1064', '100', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('295', '3', '500', '1063', '666', '1064', '100', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('295', '4', '500', '1063', '666', '1064', '100', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('295', '5', '500', '1063', '666', '1064', '100', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('296', '1', '1000', '894', '200', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('296', '2', '1000', '894', '200', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('296', '3', '1000', '894', '200', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('296', '4', '1000', '894', '200', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('296', '5', '1000', '894', '200', null, '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('297', '1', '1565', '1081', '1430', '1079', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('298', '1', '500', '1248', '500', '1250', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('298', '2', '500', '1248', '500', '1250', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('298', '3', '500', '1248', '500', '1250', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('298', '4', '500', '1248', '500', '1250', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('298', '5', '500', '1248', '500', '1250', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('300', '1', '500', '1086', '2150', '1087', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('301', '1', '500', '871', '500', '945', '20', '20000', '0', '1');
INSERT INTO `actionarguments` VALUES ('301', '2', '500', '871', '500', '945', '20', '20000', '0', '1');
INSERT INTO `actionarguments` VALUES ('301', '3', '500', '871', '500', '945', '20', '20000', '0', '1');
INSERT INTO `actionarguments` VALUES ('301', '4', '500', '871', '500', '945', '20', '20000', '0', '1');
INSERT INTO `actionarguments` VALUES ('301', '5', '500', '871', '500', '945', '20', '20000', '0', '1');
INSERT INTO `actionarguments` VALUES ('302', '1', '1598', '1099', '1398', '1098', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('302', '2', '1598', '1099', '1398', '1098', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('302', '3', '1598', '1099', '1000', '1098', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('302', '4', '1598', '1099', '500', '1098', '50', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('302', '5', '1598', '1099', '500', '1098', '60', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('303', '1', '400', '1063', '400', '1064', '20', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('303', '2', '400', '1063', '400', '1064', '20', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('303', '3', '400', '1063', '400', '1064', '30', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('303', '4', '400', '1063', '400', '1064', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('303', '5', '400', '1063', '400', '1064', '50', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('304', '1', '500', '1063', '700', '1064', '20', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('304', '2', '500', '1063', '700', '1064', '30', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('304', '3', '500', '1063', '700', '1064', '40', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('304', '4', '500', '1063', '700', '1064', '50', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('304', '5', '500', '1063', '700', '1064', '60', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('305', '1', '0', null, '2300', '847', '0', '300000', '0', '1');
INSERT INTO `actionarguments` VALUES ('306', '1', '3000', '1093', '2000', '1094', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('307', '1', '400', '1420', '500', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('307', '2', '400', '1069', '500', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('307', '3', '400', '1420', '500', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('307', '4', '400', '1069', '500', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('307', '5', '400', '1069', '500', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('307', '6', '400', '1520', '500', '1521', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('307', '7', '400', '1069', '500', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('380', '1', '500', '1069', '500', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('380', '2', '500', '1069', '500', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('380', '3', '500', '1069', '500', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('380', '4', '500', '1069', '500', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('380', '5', '500', '1069', '500', '1070', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('380', '6', '500', '1520', '500', '1521', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('381', '1', '500', '1248', '500', '1250', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('381', '2', '500', '1248', '500', '1250', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('381', '3', '500', '1248', '500', '1250', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('381', '4', '500', '1248', '500', '1250', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('381', '5', '500', '1248', '500', '1250', '60', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('383', '1', '500', '1248', '500', '1250', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('383', '2', '500', '1248', '500', '1250', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('383', '3', '500', '1248', '500', '1250', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('383', '4', '500', '1248', '500', '1250', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('383', '5', '500', '1248', '500', '1250', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('384', '1', '666', '1083', '666', '1084', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('384', '2', '666', '1083', '666', '1084', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('384', '3', '666', '1083', '666', '1084', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('384', '4', '666', '1083', '666', '1084', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('384', '5', '666', '1083', '666', '1084', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('385', '1', '500', '1420', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('385', '2', '500', '1420', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('385', '3', '500', '1420', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('385', '4', '500', '1420', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('385', '5', '500', '1420', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('386', '1', '400', '1069', '400', '1070', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('386', '2', '400', '1069', '400', '1070', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('386', '3', '400', '1069', '400', '1070', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('386', '4', '400', '1069', '400', '1070', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('386', '5', '400', '1069', '400', '1070', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('386', '6', '400', '1069', '400', '1070', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('387', '1', '666', '1248', '966', '1250', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('387', '2', '666', '1248', '666', '1250', '100', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('387', '3', '666', '1248', '966', '1250', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('387', '4', '666', '1248', '666', '1250', '100', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('387', '5', '666', '1248', '966', '1250', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('387', '6', '666', '1248', '966', '1250', '60', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('388', '1', '400', '1063', '400', '1064', '60', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('388', '2', '400', '1063', '400', '1064', '60', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('388', '3', '400', '1063', '400', '1064', '60', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('388', '4', '400', '1063', '400', '1064', '60', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('388', '5', '400', '1063', '400', '1064', '60', '45000', '0', '1');
INSERT INTO `actionarguments` VALUES ('389', '1', '2499', '827', '500', '916', '0', '15000', '0', '0');
INSERT INTO `actionarguments` VALUES ('389', '2', '2499', '827', '500', '916', '0', '15000', '0', '0');
INSERT INTO `actionarguments` VALUES ('389', '3', '2499', '827', '500', '916', '0', '15000', '0', '0');
INSERT INTO `actionarguments` VALUES ('389', '4', '2499', '827', '500', '916', '0', '15000', '0', '0');
INSERT INTO `actionarguments` VALUES ('389', '5', '2499', '827', '500', '916', '0', '15000', '0', '0');
INSERT INTO `actionarguments` VALUES ('390', '1', '400', '1420', '400', '916', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('390', '2', '400', '1420', '400', '916', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('390', '3', '400', '1420', '400', '916', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('390', '4', '400', '1420', '400', '916', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('390', '5', '400', '1420', '400', '916', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('392', '1', '2499', '827', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('392', '2', '2499', '827', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('392', '3', '2499', '827', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('392', '4', '2499', '827', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('392', '5', '2499', '827', '466', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('392', '6', '2000', '827', '466', '916', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('392', '7', '1000', '1522', '466', '916', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('393', '1', '500', '1063', '700', '1064', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('393', '2', '500', '1063', '700', '1064', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('393', '3', '500', '1063', '700', '1064', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('393', '4', '500', '1063', '700', '1064', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('393', '5', '500', '1063', '700', '1064', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('395', '1', '700', '1476', '1033', '968', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('395', '2', '700', '1476', '1033', '968', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('395', '3', '700', '1476', '1033', '968', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('395', '4', '700', '1476', '1033', '968', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('395', '5', '700', '1476', '1033', '968', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('395', '6', '700', '1476', '1033', '968', '50', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('396', '1', '0', null, '1499', '1097', '3', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('397', '1', '333', '1102', '950', '1103', '3', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('397', '2', '333', '1102', '950', '1103', '3', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('397', '3', '333', '1102', '950', '1103', '3', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('397', '4', '333', '1102', '950', '1103', '3', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('397', '5', '333', '1102', '950', '1103', '3', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('397', '6', '333', '1102', '950', '1103', '3', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('398', '1', '0', null, '1533', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('398', '2', '0', null, '1533', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('398', '3', '0', null, '1533', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('398', '4', '0', null, '1533', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('398', '5', '0', null, '1533', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('399', '1', '0', null, '250', '490', '50', '750', '0', '1');
INSERT INTO `actionarguments` VALUES ('399', '2', '0', null, '250', '490', '50', '750', '0', '1');
INSERT INTO `actionarguments` VALUES ('399', '3', '0', null, '250', '490', '50', '750', '0', '1');
INSERT INTO `actionarguments` VALUES ('399', '4', '0', null, '250', '490', '50', '750', '0', '1');
INSERT INTO `actionarguments` VALUES ('399', '5', '0', null, '250', '490', '50', '750', '0', '1');
INSERT INTO `actionarguments` VALUES ('399', '6', '0', null, '250', '490', '50', '750', '0', '1');
INSERT INTO `actionarguments` VALUES ('399', '7', '0', null, '250', '490', '50', '750', '0', '1');
INSERT INTO `actionarguments` VALUES ('399', '8', '0', null, '250', '490', '50', '750', '0', '1');
INSERT INTO `actionarguments` VALUES ('400', '1', '3335', '1107', '1335', '1109', '1', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('400', '2', '3335', '1107', '1335', '1109', '1', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('400', '3', '3335', '1107', '1335', '1109', '1', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('400', '4', '3335', '1107', '1335', '1109', '1', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('400', '5', '3335', '1107', '1335', '1109', '5', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('400', '6', '3335', '1107', '1335', '1109', '1', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('401', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('401', '2', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('401', '3', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('401', '4', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('401', '5', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('401', '6', '0', null, '0', null, '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('402', '1', '0', null, '2133', '899', '2', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('402', '2', '0', null, '2133', '899', '2', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('402', '3', '0', null, '2133', '899', '2', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('402', '4', '0', null, '2133', '899', '2', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('402', '5', '0', null, '2133', '899', '2', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('402', '6', '0', null, '2133', '899', '2', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('403', '1', '900', '1009', '766', '1008', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('403', '2', '900', '1009', '766', '1008', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('403', '3', '900', '1009', '766', '1008', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('403', '4', '900', '1009', '766', '1008', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('403', '5', '900', '1009', '766', '1008', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('403', '6', '900', '1009', '766', '1008', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('404', '1', '1000', '1386', '6000', '1385', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('405', '1', '1200', '1137', '2350', '1138', '30', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('405', '2', '1200', '1137', '2350', '1138', '30', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('405', '3', '1200', '1137', '2350', '1138', '30', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('405', '4', '1200', '1137', '2350', '1138', '30', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('407', '1', '0', null, '4000', '1149', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('407', '2', '0', null, '4000', '1149', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('408', '1', '0', null, '0', '1064', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('409', '1', '0', null, '3000', '1154', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('409', '2', '0', null, '3000', '1154', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('409', '3', '0', null, '3000', '1154', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('409', '4', '0', null, '3000', '1154', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('409', '5', '0', null, '3000', '1154', '1', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('410', '1', '5666', '1156', '1533', '1157', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('410', '2', '5666', '1156', '1533', '1157', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('410', '3', '5666', '1156', '1533', '1157', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('411', '1', '0', null, '500', '954', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('412', '1', '566', '1187', '933', '980', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('412', '2', '566', '1187', '933', '980', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('412', '3', '566', '1187', '933', '980', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('412', '4', '566', '1187', '933', '980', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('412', '5', '566', '1187', '933', '980', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('413', '1', '3000', '1154', '0', null, '2', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('417', '1', '0', null, '3000', '1272', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('418', '1', '0', null, '933', '85', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('418', '2', '0', null, '933', '85', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('418', '3', '0', null, '933', '85', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('418', '4', '0', null, '933', '85', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('418', '5', '0', null, '933', '85', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('418', '6', '0', null, '933', '85', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('419', '1', '0', null, '2000', '1246', '0', '5000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '2', '0', null, '2000', '1246', '0', '10000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '3', '0', null, '2000', '1246', '0', '15000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '4', '0', null, '2000', '1246', '0', '5000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '5', '0', null, '2000', '1246', '0', '10000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '6', '0', null, '2000', '1246', '0', '15000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '7', '0', null, '2000', '1246', '0', '5000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '8', '0', null, '2000', '1246', '0', '10000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '9', '0', null, '2000', '1246', '0', '15000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '10', '0', null, '2000', '1246', '0', '5000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '11', '0', null, '2000', '1246', '0', '10000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '12', '0', null, '2000', '1246', '0', '15000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '13', '0', null, '2000', '1246', '0', '5000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '14', '0', null, '2000', '1246', '0', '10000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '15', '0', null, '2000', '1246', '0', '15000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '16', '0', null, '2000', '1246', '0', '5000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '17', '0', null, '2000', '1246', '0', '10000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '18', '0', null, '2000', '1246', '0', '15000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '19', '0', null, '2000', '1246', '0', '5000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '20', '0', null, '2000', '1246', '0', '10000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '21', '0', null, '2000', '1246', '0', '15000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '22', '0', null, '2000', '1246', '0', '5000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '23', '0', null, '2000', '1246', '0', '10000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '24', '0', null, '2000', '1246', '0', '15000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '25', '0', null, '2000', '1246', '0', '5000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '26', '0', null, '2000', '1246', '0', '10000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '27', '0', null, '2000', '1246', '0', '15000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '28', '0', null, '2000', '1246', '0', '5000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '29', '0', null, '2000', '1246', '0', '10000', '1', '1');
INSERT INTO `actionarguments` VALUES ('419', '30', '0', null, '2000', '1246', '0', '15000', '1', '1');
INSERT INTO `actionarguments` VALUES ('420', '1', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '2', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '3', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '4', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '5', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '6', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '7', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '8', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '9', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '10', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '11', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '12', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '13', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '14', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '15', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '16', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '17', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '18', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '19', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '20', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '21', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '22', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '23', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '24', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '25', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '26', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '27', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '28', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '29', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '30', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '31', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '32', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '33', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '34', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '35', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '36', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '37', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '38', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '39', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '40', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '41', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '42', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '43', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '44', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '45', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '46', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '47', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '48', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '49', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('420', '50', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('421', '1', '300', '1083', '300', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('421', '2', '300', '1083', '300', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('421', '3', '300', '1083', '300', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('421', '4', '300', '1083', '300', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('421', '5', '300', '1083', '300', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('421', '6', '300', '1083', '300', '945', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('422', '1', '0', null, '2500', '858', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('424', '1', '0', null, '2066', '1259', '0', '0', '0', '0');
INSERT INTO `actionarguments` VALUES ('424', '2', '0', null, '1133', null, '0', '0', '0', '0');
INSERT INTO `actionarguments` VALUES ('424', '3', '0', null, '2500', null, '0', '0', '0', '0');
INSERT INTO `actionarguments` VALUES ('424', '4', '0', null, '2500', null, '0', '0', '0', '0');
INSERT INTO `actionarguments` VALUES ('425', '1', '0', null, '4000', '1267', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('426', '1', '0', null, '2966', '662', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('426', '2', '0', null, '2966', '662', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('427', '1', '0', null, '1000', '1246', '0', '60000', '1', '1');
INSERT INTO `actionarguments` VALUES ('427', '2', '0', null, '1000', '1246', '0', '60000', '1', '1');
INSERT INTO `actionarguments` VALUES ('427', '3', '0', null, '1000', '1246', '0', '60000', '1', '1');
INSERT INTO `actionarguments` VALUES ('427', '4', '0', null, '1000', '1246', '0', '60000', '1', '1');
INSERT INTO `actionarguments` VALUES ('427', '5', '0', null, '1000', '1246', '0', '60000', '1', '1');
INSERT INTO `actionarguments` VALUES ('427', '6', '0', null, '1000', '1246', '0', '60000', '1', '1');
INSERT INTO `actionarguments` VALUES ('427', '7', '0', null, '1000', '1246', '0', '60000', '1', '1');
INSERT INTO `actionarguments` VALUES ('427', '8', '0', null, '1000', '1246', '0', '60000', '1', '1');
INSERT INTO `actionarguments` VALUES ('427', '9', '0', null, '1000', '1246', '0', '60000', '1', '1');
INSERT INTO `actionarguments` VALUES ('427', '10', '0', null, '1000', '1246', '0', '60000', '1', '1');
INSERT INTO `actionarguments` VALUES ('428', '1', '0', null, '4000', '940', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('428', '2', '0', null, '4000', '940', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('429', '1', '0', null, '3000', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('429', '2', '0', null, '3000', null, '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('430', '1', '500', '1063', '500', '1064', '100', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('430', '2', '500', '1063', '500', '1064', '100', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('430', '3', '500', '1063', '500', '1064', '100', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('430', '4', '500', '1063', '500', '1064', '100', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('430', '5', '500', '1063', '500', '1064', '100', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('431', '1', '0', null, '2500', '896', '2', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('431', '2', '0', null, '2500', '896', '2', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('431', '3', '0', null, '2500', '896', '2', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('431', '4', '0', null, '2500', '896', '2', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('433', '1', '1033', '1274', '1733', '1273', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('433', '2', '1033', '1274', '1733', '1273', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('433', '3', '1033', '1274', '1733', '1273', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('433', '4', '1033', '1274', '1733', '1273', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('433', '5', '1033', '1274', '1733', '1273', '3', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('433', '6', '1033', '1274', '1733', '1273', '3', '4000', '0', '1');
INSERT INTO `actionarguments` VALUES ('433', '7', '1066', '1512', '1900', '1191', '3', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('434', '1', '833', '1275', '1133', '1276', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('435', '1', '4500', '1277', '3000', '1278', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('436', '1', '3600', '1041', '500', '1042', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('436', '2', '3600', '1041', '500', '1042', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('436', '3', '3600', '1041', '500', '1042', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('436', '4', '3600', '1041', '500', '1042', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('437', '1', '0', null, '2150', '913', '5', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('437', '2', '0', null, '2150', '913', '5', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('437', '3', '0', null, '2150', '913', '5', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('437', '4', '0', null, '2150', '913', '5', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('438', '1', '0', null, '5333', '1280', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('438', '2', '0', null, '5333', '1280', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('438', '3', '0', null, '5333', '1280', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('438', '4', '0', null, '5333', '1280', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('439', '1', '0', null, '2400', '900', '3', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('439', '2', '0', null, '2400', '900', '3', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('439', '3', '0', null, '2400', '900', '3', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('439', '4', '0', null, '2400', '900', '3', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('440', '1', '1060', '911', '1560', '912', '40', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('440', '2', '1060', '911', '1560', '912', '40', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('440', '3', '1060', '911', '1560', '912', '40', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('440', '4', '1060', '911', '1560', '912', '40', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('441', '1', '0', null, '3333', '630', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('442', '1', '3666', '1290', '3666', '629', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('443', '1', '0', null, '3000', '1285', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('444', '1', '3000', '1286', '6000', '1287', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('445', '1', '1666', '1289', '5000', '1288', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('446', '1', '400', '1083', '500', '945', '20', '10000', '0', '0');
INSERT INTO `actionarguments` VALUES ('446', '2', '400', '1083', '500', '945', '20', '10000', '0', '0');
INSERT INTO `actionarguments` VALUES ('446', '3', '400', '1083', '500', '945', '20', '10000', '0', '0');
INSERT INTO `actionarguments` VALUES ('446', '4', '400', '1083', '500', '945', '20', '10000', '0', '0');
INSERT INTO `actionarguments` VALUES ('446', '5', '400', '1083', '500', '945', '20', '10000', '0', '0');
INSERT INTO `actionarguments` VALUES ('446', '6', '400', '1083', '500', '945', '20', '10000', '0', '0');
INSERT INTO `actionarguments` VALUES ('447', '1', '2200', '1324', '800', '1323', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('447', '2', '2200', '1324', '800', '1323', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('448', '1', '1666', '864', '1333', '931', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('448', '2', '1666', '864', '1333', '931', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('448', '3', '1666', '864', '1333', '931', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('448', '4', '1666', '864', '1333', '931', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('448', '5', '1666', '864', '1333', '931', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('449', '1', '500', '1342', '700', '1343', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('449', '2', '500', '1342', '700', '1343', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('449', '3', '0', null, '700', '1343', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('450', '1', '500', '1342', '700', '1343', '30', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('450', '2', '500', '1342', '700', '1343', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('450', '3', '1066', '1512', '1900', '1191', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('450', '4', '1000', '1513', '2099', '1192', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '1', '500', '1342', '700', '1343', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '2', '500', '1342', '700', '1343', '50', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '3', '500', '1342', '700', '1343', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '4', '500', '1342', '700', '1343', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '5', '500', '1342', '700', '1343', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '6', '500', '1342', '700', '1343', '40', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '7', '500', '1342', '700', '1343', '50', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '8', '500', '1342', '700', '1343', '50', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '9', '500', '1342', '700', '1343', '50', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '10', '500', '1342', '700', '1343', '50', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('451', '11', '1000', '1513', '2099', '1192', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('452', '1', '500', '1063', '700', '1064', '30', '5', '0', '1');
INSERT INTO `actionarguments` VALUES ('452', '2', '500', '1063', '700', '1064', '40', '5', '0', '1');
INSERT INTO `actionarguments` VALUES ('452', '3', '500', '1063', '700', '1064', '40', '5', '0', '1');
INSERT INTO `actionarguments` VALUES ('452', '4', '500', '1063', '700', '1064', '40', '5', '0', '1');
INSERT INTO `actionarguments` VALUES ('452', '5', '1000', '1513', '2099', '1192', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('453', '1', '500', '1063', '700', '1064', '20', '5', '0', '1');
INSERT INTO `actionarguments` VALUES ('453', '2', '500', '1063', '700', '1064', '30', '5', '0', '1');
INSERT INTO `actionarguments` VALUES ('453', '3', '500', '1063', '700', '1064', '30', '5', '0', '1');
INSERT INTO `actionarguments` VALUES ('453', '4', '500', '1063', '700', '1064', '40', '5', '0', '1');
INSERT INTO `actionarguments` VALUES ('454', '1', '500', '1344', '700', '1345', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('454', '2', '500', '1344', '700', '1345', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('455', '1', '500', '1344', '700', '1345', '0', '20000', '0', '1');
INSERT INTO `actionarguments` VALUES ('455', '2', '500', '1344', '700', '1345', '0', '20000', '0', '1');
INSERT INTO `actionarguments` VALUES ('455', '3', '500', '1344', '700', '1345', '0', '50000', '0', '1');
INSERT INTO `actionarguments` VALUES ('456', '1', '500', '1342', '700', '1343', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('456', '2', '500', '1342', '700', '1343', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('456', '3', '500', '1342', '700', '1343', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('456', '4', '500', '1342', '700', '1343', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('456', '5', '500', '1342', '700', '1343', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('456', '6', '500', '1342', '700', '1343', '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('457', '1', '500', '1342', '700', '1343', '30', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('457', '2', '500', '1342', '700', '1343', '30', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('457', '3', '500', '1342', '700', '1343', '30', '30', '0', '1');
INSERT INTO `actionarguments` VALUES ('458', '1', '500', '1342', '700', '1343', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('458', '2', '500', '1342', '700', '1343', '40', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('458', '3', '1333', '1511', '2800', '1189', '20', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('459', '1', '0', null, '2000', '1246', '0', '300000', '1', '1');
INSERT INTO `actionarguments` VALUES ('459', '2', '0', null, '2000', '1246', '0', '300000', '1', '1');
INSERT INTO `actionarguments` VALUES ('459', '3', '0', null, '2000', '1246', '0', '300000', '1', '1');
INSERT INTO `actionarguments` VALUES ('459', '4', '0', null, '2000', '1246', '0', '300000', '1', '1');
INSERT INTO `actionarguments` VALUES ('459', '5', '0', null, '2000', '1246', '0', '300000', '1', '1');
INSERT INTO `actionarguments` VALUES ('459', '6', '0', null, '2000', '1246', '0', '300000', '1', '1');
INSERT INTO `actionarguments` VALUES ('459', '7', '0', null, '2000', '1246', '0', '300000', '1', '1');
INSERT INTO `actionarguments` VALUES ('459', '8', '0', null, '2000', '1246', '0', '300000', '1', '1');
INSERT INTO `actionarguments` VALUES ('459', '9', '0', null, '2000', '1246', '0', '300000', '1', '1');
INSERT INTO `actionarguments` VALUES ('459', '10', '0', null, '2000', '1246', '0', '300000', '1', '1');
INSERT INTO `actionarguments` VALUES ('460', '1', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '2', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '3', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '4', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '5', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '6', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '7', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '8', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '9', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '10', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '11', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '12', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '13', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '14', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '15', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '16', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '17', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '18', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '19', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('460', '20', '0', null, '766', '945', '0', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('461', '1', '0', null, '766', '945', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('462', '1', '0', null, '766', '945', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '1', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '2', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '3', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '4', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '5', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '6', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '7', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '8', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '9', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '10', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '11', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '12', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '13', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '14', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '15', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '16', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '17', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '18', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '19', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '20', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '21', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '22', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '23', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '24', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '25', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '26', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '27', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '28', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '29', '0', null, '9333', '1306', '0', '0', '0', '0');
INSERT INTO `actionarguments` VALUES ('464', '30', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '31', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '32', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '33', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '34', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '35', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '36', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '37', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '38', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '39', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '40', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '41', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '42', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '43', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '44', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '45', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '46', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '47', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '48', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '49', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '50', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '51', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '52', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '53', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '54', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '55', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '56', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('464', '57', '0', null, '4828', '1430', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('467', '1', '0', null, '2000', '1347', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('467', '2', '0', null, '2000', '1347', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('467', '3', '0', null, '2000', '1347', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('467', '4', '0', null, '2000', '1347', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('467', '5', '0', null, '2000', '1347', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('467', '6', '0', null, '2000', '1347', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('467', '7', '0', null, '2000', '1347', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('467', '8', '0', null, '2000', '1347', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('467', '9', '0', null, '2000', '1347', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('467', '10', '0', null, '2000', '1347', '0', '30000', '0', '1');
INSERT INTO `actionarguments` VALUES ('468', '1', '0', null, '2333', '1053', '20', '1000', '0', '1');
INSERT INTO `actionarguments` VALUES ('470', '1', '466', '1332', '1133', '1333', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('470', '2', '466', '1332', '1133', '1333', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('470', '3', '466', '1332', '1133', '1333', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('470', '4', '466', '1332', '1133', '1333', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('471', '1', '0', null, '1533', '1334', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('471', '2', '0', null, '1533', '1334', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('471', '3', '0', null, '1533', '1334', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('471', '4', '0', null, '1533', '1334', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('472', '1', '466', '1340', '1133', '1340', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('473', '1', '2800', '1292', '2199', '1291', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('474', '63', '3000', '1362', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('474', '65', '3000', '1364', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('474', '66', '2666', '1365', '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('475', '1', '0', null, '1666', '1378', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('475', '2', '0', null, '1666', '1378', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('475', '3', '0', null, '1666', '1378', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('475', '4', '0', null, '1666', '1378', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('476', '1', '600', '1300', '1133', '1301', '20', '12000', '0', '1');
INSERT INTO `actionarguments` VALUES ('476', '2', '600', '1300', '1133', '1301', '20', '12000', '0', '1');
INSERT INTO `actionarguments` VALUES ('476', '3', '600', '1300', '1133', '1301', '20', '12000', '0', '1');
INSERT INTO `actionarguments` VALUES ('476', '4', '600', '1300', '1133', '1301', '20', '12000', '0', '1');
INSERT INTO `actionarguments` VALUES ('477', '1', '1000', '1302', '1400', '1303', '20', '7500', '0', '1');
INSERT INTO `actionarguments` VALUES ('477', '2', '1000', '1302', '1400', '1303', '20', '7500', '0', '1');
INSERT INTO `actionarguments` VALUES ('478', '1', '0', null, '1733', '1379', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('478', '2', '0', null, '1733', '1379', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('478', '3', '0', null, '1733', '1379', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('478', '4', '0', null, '1733', '1379', '4', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('479', '1', '0', null, '1666', '1378', '10', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('479', '2', '0', null, '1666', '1378', '10', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('479', '3', '0', null, '1666', '1378', '10', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('479', '4', '0', null, '1666', '1378', '10', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('480', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('480', '2', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('480', '3', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('480', '4', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('482', '1', '1000', '1386', '2000', '1385', '80', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('482', '2', '1000', '1386', '6000', '1385', '80', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('482', '3', '1000', '1386', '6000', '1385', '80', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('482', '4', '1000', '1386', '6000', '1385', '80', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('482', '5', '1000', '1386', '6000', '1385', '80', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('482', '6', '1000', '1386', '6000', '1385', '80', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('482', '7', '1000', '1386', '6000', '1385', '80', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('482', '8', '1000', '1386', '6000', '1385', '80', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('482', '9', '1000', '1386', '6000', '1385', '80', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('482', '10', '1000', '1386', '6000', '1385', '80', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('482', '11', '0', null, '9333', '1306', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('482', '12', '0', null, '9333', '1306', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('483', '1', '4666', '1370', '0', null, '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('484', '1', '0', '827', '0', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('484', '2', '0', '827', '0', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('484', '3', '0', '827', '0', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('484', '4', '0', '827', '0', '916', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('485', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('486', '1', '0', null, '2000', '1246', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('486', '2', '0', null, '2000', '1246', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('486', '3', '0', null, '2000', '1246', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('486', '4', '0', null, '2000', '1246', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('486', '5', '0', null, '2000', '1246', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('486', '6', '0', null, '2000', '1246', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('486', '7', '0', null, '2000', '1246', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('486', '8', '0', null, '2000', '1246', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('486', '9', '0', null, '2000', '1246', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('486', '10', '0', null, '2000', '1246', '0', '60000', '0', '1');
INSERT INTO `actionarguments` VALUES ('487', '1', '8000', '1399', '1066', '1400', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('487', '2', '8000', '1399', '1066', '1400', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('487', '3', '8000', '1399', '1066', '1400', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('487', '4', '8000', '1399', '1066', '1400', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('487', '5', '8000', '1399', '1066', '1400', '0', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('488', '1', '2100', '1424', '900', '1425', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('488', '2', '2100', '1424', '900', '1425', '20', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('489', '1', '800', '1426', '800', '1428', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('489', '2', '800', '1426', '800', '1428', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('490', '1', '800', '1421', '800', '1422', '5', '0', '1', '1');
INSERT INTO `actionarguments` VALUES ('490', '2', '800', '1421', '800', '1422', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('492', '1', '1100', '1412', '1566', '1411', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('492', '2', '1100', '1412', '1566', '1411', '30', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('493', '1', '1166', '1433', '2800', '958', '60', '12000', '0', '1');
INSERT INTO `actionarguments` VALUES ('494', '1', '0', null, '0', null, '60', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '1', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '2', '0', null, '20000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '3', '0', null, '19966', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '4', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '5', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '6', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '7', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '8', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '9', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '10', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '11', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '12', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '13', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '14', '0', null, '3500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '15', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '16', '0', null, '1660', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '17', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '18', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '19', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '20', '0', null, '1260', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '21', '0', null, '3000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '22', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '23', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '24', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '25', '0', null, '2750', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '26', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '27', '0', null, '1500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '28', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '29', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '30', '0', null, '2666', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '31', '0', null, '2666', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '32', '0', null, '1500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '33', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '34', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '35', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '36', '0', null, '4666', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '37', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '38', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '39', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '40', '0', null, '3366', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '41', '0', null, '3366', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '42', '0', null, '3366', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '43', '0', null, '1900', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '44', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '45', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '46', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '47', '0', null, '3000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '48', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '49', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '50', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '51', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '52', '0', null, '4666', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '53', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '54', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '55', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '56', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '57', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '58', '0', null, '1999', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '59', '0', null, '3600', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '60', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '61', '0', null, '3600', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '62', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '63', '0', null, '12000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '64', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '65', '0', null, '6300', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '66', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '67', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '68', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '69', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '70', '0', null, '1999', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '71', '0', null, '13000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '72', '0', null, '1666', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '73', '0', null, '16000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '74', '0', null, '2500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '75', '0', null, '6733', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '76', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '77', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '78', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '79', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '80', '0', null, '1500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '81', '0', null, '16000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '82', '0', null, '16000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '83', '0', null, '16000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '84', '0', null, '16000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '85', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '86', '0', null, '2999', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '87', '0', null, '6230', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '88', '0', null, '2650', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '89', '0', null, '7200', '1211', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '90', '0', null, '2666', '1236', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '91', '0', null, '2666', '1237', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '92', '0', null, '2666', '1238', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '93', '0', null, '2666', '1239', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '94', '0', null, '2666', '1236', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '95', '0', null, '2666', '1237', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '96', '0', null, '2666', '1238', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '97', '0', null, '2666', '1239', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '98', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '99', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '100', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '101', '0', null, '5999', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '102', '0', null, '2333', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '103', '0', null, '2333', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '104', '0', null, '2333', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '105', '0', null, '2333', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '106', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '107', '0', null, '3000', '1348', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '108', '0', null, '3000', '1348', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '109', '0', null, '3000', '1348', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '110', '0', null, '3000', '1348', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '111', '0', null, '0', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '112', '0', null, '4000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '113', '0', null, '250', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '114', '0', null, '1433', '1455', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '115', '0', null, '1433', '1455', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '116', '0', null, '2000', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '117', '0', null, '2166', '1461', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '118', '0', null, '3000', '1463', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '119', '0', null, '1500', '622', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('495', '120', '0', null, '6327', '1503', '0', '0', '0', '0');
INSERT INTO `actionarguments` VALUES ('496', '1', '1800', '1438', '1166', '1439', '50', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('496', '2', '1800', '1438', '1166', '1439', '50', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('496', '3', '1800', '1438', '1166', '1439', '50', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('496', '4', '1800', '1438', '1166', '1439', '50', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('496', '5', '1800', '1438', '1166', '1439', '50', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('497', '1', '1033', '1274', '1733', '1273', '20', '3500', '0', '1');
INSERT INTO `actionarguments` VALUES ('497', '2', '1033', '1274', '1733', '1273', '20', '3500', '0', '1');
INSERT INTO `actionarguments` VALUES ('497', '3', '1033', '1274', '1733', '1273', '20', '3500', '0', '1');
INSERT INTO `actionarguments` VALUES ('497', '4', '1033', '1274', '1733', '1273', '20', '3500', '0', '1');
INSERT INTO `actionarguments` VALUES ('497', '5', '1033', '1274', '1733', '1273', '20', '3500', '0', '1');
INSERT INTO `actionarguments` VALUES ('498', '1', '1000', '1248', '1000', '1250', '30', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('499', '1', '433', '1450', '533', '1451', '2', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('500', '1', '8000', '1453', '1500', '1454', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('500', '2', '8000', '1453', '1500', '1454', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('501', '1', '0', null, '2000', '1347', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('502', '1', '0', null, '2000', '1347', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('503', '1', '0', null, '2000', '1347', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('503', '2', '0', null, '2000', '1347', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('503', '3', '0', null, '2000', '1347', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('504', '1', '1598', '1099', '1398', '1098', '20', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('506', '1', '0', null, '2000', '1347', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('507', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('507', '2', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('508', '1', '0', null, '9333', '1306', '5', '12', '0', '1');
INSERT INTO `actionarguments` VALUES ('508', '2', '2000', '1485', '5500', '1486', '5', '12', '0', '1');
INSERT INTO `actionarguments` VALUES ('508', '3', '0', null, '9333', '1306', '5', '12', '0', '1');
INSERT INTO `actionarguments` VALUES ('509', '1', '2000', '1485', '3000', '1486', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('509', '2', '2000', '1485', '3000', '1486', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('509', '3', '2000', '1485', '3000', '1486', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('509', '4', '2000', '1485', '3000', '1486', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('509', '5', '2000', '1485', '3000', '1486', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('509', '6', '2000', '1485', '3000', '1486', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('509', '7', '2000', '1485', '3000', '1486', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('509', '8', '2000', '1485', '3000', '1486', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('509', '9', '2000', '1485', '3000', '1486', '80', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('510', '1', '1000', '1386', '2000', '1385', '0', '3600000', '0', '1');
INSERT INTO `actionarguments` VALUES ('511', '1', '1000', '1386', '2000', '1385', '0', '3600000', '0', '1');
INSERT INTO `actionarguments` VALUES ('512', '1', '1000', '901', '2600', '932', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('514', '1', '0', null, '5000', '1494', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('516', '1', '2500', '827', '500', '916', '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('517', '1', '0', null, '0', null, '0', '10000', '0', '1');
INSERT INTO `actionarguments` VALUES ('518', '1', '500', '1501', '1000', '1502', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('519', '1', '650', '1500', '1000', '1499', '10', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('520', '1', '0', null, '1000', '1494', '5', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('521', '1', '5494', '1496', '1000', '1495', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('521', '2', '5494', '1496', '1000', '1495', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('522', '1', '1566', '1498', '2733', '1497', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('522', '2', '1566', '1498', '2733', '1497', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('523', '1', '2000', '1506', '2000', '1505', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('524', '1', '2000', '1104', '2000', '1105', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('525', '1', '0', null, '2000', '1195', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('526', '1', '0', null, '1000', '952', '5', '2000', '0', '1');
INSERT INTO `actionarguments` VALUES ('527', '1', '0', null, '1500', '491', '40', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('527', '2', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('528', '1', '1000', '1248', '1000', '1250', '30', '5000', '0', '1');
INSERT INTO `actionarguments` VALUES ('99999', '9999', '123', '1234', '12345', null, null, null, null, null);
INSERT INTO `actionarguments` VALUES ('10000001', '1', '0', null, '0', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000002', '1', '299', '1281', '1098', '1282', '3', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000004', '1', '1498', '939', '666', '873', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000004', '2', '1498', '939', '666', '873', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000004', '3', '1498', '939', '666', '873', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000004', '4', '1498', '939', '666', '873', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000004', '5', '1498', '939', '666', '873', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000004', '6', '1498', '939', '666', '873', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000004', '7', '1498', '939', '666', '873', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000004', '8', '1498', '939', '666', '873', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000004', '9', '1498', '939', '666', '873', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000004', '10', '1498', '939', '666', '873', '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000005', '1', '400', '1063', '400', '1064', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000005', '2', '400', '1063', '400', '1064', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000005', '3', '400', '1063', '400', '1064', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000005', '4', '400', '1063', '400', '1064', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('10000005', '5', '400', '1063', '400', '1064', '60', '15000', '0', '1');
INSERT INTO `actionarguments` VALUES ('20000001', '1', '0', null, '2600', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('20000001', '2', '0', null, '5000', '932', '0', '1000', '0', '0');
INSERT INTO `actionarguments` VALUES ('20000002', '1', '0', null, '2600', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('20000002', '2', '0', null, '2600', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('20000002', '3', '0', null, '2600', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('20000002', '4', '0', null, '2600', null, '0', '0', '0', '1');
INSERT INTO `actionarguments` VALUES ('20000002', '5', '0', null, '2600', null, '0', '0', '0', '1');
INSERT INTO `actionmodules` VALUES ('2', 'GESTURE');
INSERT INTO `actionmodules` VALUES ('74', 'INSTANT_KILL');
INSERT INTO `actionmodules` VALUES ('80', 'USE_OBJECT');
INSERT INTO `actionmodules` VALUES ('81', 'INVULNERABILITY');
INSERT INTO `actionmodules` VALUES ('83', 'REPAIR');
INSERT INTO `actionmodules` VALUES ('129', 'WEAPON_STOW');
INSERT INTO `actionmodules` VALUES ('130', 'WEAPON_DRAW');
INSERT INTO `actionmodules` VALUES ('134', 'WEAPON_RELOAD');
INSERT INTO `actionmodules` VALUES ('140', 'WEAPON_FLAMETHROWER');
INSERT INTO `actionmodules` VALUES ('149', 'WEAPON_MACHINEGUN');
INSERT INTO `actionmodules` VALUES ('155', 'CREATURE_BIRTH');
INSERT INTO `actionmodules` VALUES ('172', 'TOOL_HARVEST');
INSERT INTO `actionmodules` VALUES ('174', 'WEAPON_MELEE');
INSERT INTO `actionmodules` VALUES ('397', 'CR_THRAX_KICK');
INSERT INTO `characters` VALUES ('50', 'Bootcamper', 'Warfare', '5', '1', '0', '1', '2', '18328', '4286611584', '18271', '4286611584', '18299', '4286611584', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3672', '4280300626', '18384', '4286611584', '18356', '4286611584', '20824', '4286886614', '0', '0', '0', '0', '0', '0', '0', '0', '1220', '184.43', '163.12', '-53.75', '0');
INSERT INTO `characters` VALUES ('49', 'Richard', 'Garriott', '4', '1', '1', '2', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3672', '4280300626', '0', '0', '0', '0', '20824', '4286886614', '0', '0', '0', '0', '0', '0', '0', '0', '1991', '-87.67', '10', '100', '0');
INSERT INTO `characters` VALUES ('48', 'OMG', 'WTF', '1', '1', '0', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3672', '4280300626', '0', '0', '0', '0', '20824', '4286886614', '0', '0', '0', '0', '0', '0', '0', '0', '1220', '784', '295', '380', '0');
INSERT INTO `characters` VALUES ('61', 'Wexx', 'Gronzer', '6', '1', '0', '2', '1', '10908', '4286611584', '7054', '4286611584', '10909', '4286611584', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3812', '4280300626', '7052', '4286611584', '7053', '4286611584', '24743', '4284117347', '0', '0', '0', '0', '0', '0', '0', '0', '1148', '-73.9', '116.9', '730.1', '0');
INSERT INTO `characters` VALUES ('62', 'Anthrax', 'Dennton', '3', '1', '0', '4', '1', '10908', '4286611584', '7054', '4286611584', '10909', '4286611584', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3812', '4280300626', '7052', '4286611584', '7053', '4286611584', '24744', '4280891507', '0', '0', '25336', '4282551464', '0', '0', '0', '0', '1220', '894.9', '307.9', '347.1', '0');
INSERT INTO `characters` VALUES ('63', 'Ololol', 'Name', '9', '1', '0', '1', '1', '10908', '4286611584', '7054', '4286611584', '10909', '4286611584', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '9782', '4280300626', '7052', '4286611584', '7053', '4286611584', '24005', '4286886614', '0', '0', '25336', '4282551464', '25346', '4280300626', '0', '0', '1220', '894.9', '307.9', '347.1', '0');
INSERT INTO `creature_action` VALUES ('1', 'melee attack thrax soldier', '174', '46', '0.5', '3.5', '1300', '500', '0', '5', '12');
INSERT INTO `creature_action` VALUES ('2', 'range attack afs light soldier', '1', '133', '1', '20', '800', '500', '0', '10', '15');
INSERT INTO `creature_action` VALUES ('3', 'thrax kick', '397', '1', '1', '4', '6000', '500', '950', '20', '22');
INSERT INTO `creature_type` VALUES ('1', 'thrax_footsoldiers', '0', '25580', '0', '4', '9', '100', '1', '3', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('2', 'boargar_general_spawn', '0', '6031', '3', '6.2', '9', '100', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('3', 'bane_stalkerspawn_general', '0', '3781', '0', '3.3', '9', '100', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('4', 'forean_spearman_normal', '0', '6043', '1', '5.9', '9', '250', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('5', 'forean_elder_normal', '0', '6163', '1', '4.9', '9', '200', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('6', 'afs_soldier_light_m', '0', '29765', '1', '5.7', '9', '210', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('7', 'afs_soldier_medium_f', '0', '29423', '1', '5.1', '9', '100', '2', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('8', 'thrax_footsoldiers_rifle', '0', '25581', '0', '5.4', '9', '300', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('9', 'afs_mech_standard', '0', '10442', '1', '5.6', '9', '400', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('10', 'bane_hunter_invasion', '0', '10166', '0', '5.3', '9', '275', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('11', 'bane_amoeboid_invasion', '0', '6032', '0', '5.1', '9', '350', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `creature_type` VALUES ('12', 'thrax_footsoldier_invasion', '0', '25580', '0', '5.1', '9', '350', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `mission` VALUES ('1992', '4116', '4148');
INSERT INTO `npc` VALUES ('4116', 'Catherine Jones', '1985', '-221.281', '101.051', '-65.4648', '0', '20972', '0', '0', '18271', '4286611584', '18299', '4286611584', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '12143', '4289039425', '9782', '4289039425', '18384', '4286611584', '18356', '4286611584', '7695', '4286886614', '0', '0', '0', '0', '25349', '4280300626', '0', '0');
INSERT INTO `npc` VALUES ('4148', 'John Adams', '1985', '-206.129', '98.0234', '-48.1094', '0', '20975', '0', '0', '18271', '4286611584', '18299', '4286611584', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '6054', '0', '9355', '4293125348', '18384', '4286611584', '18356', '4286611584', '24010', '4286886614', '20824', '2155905152', '25336', '4282551464', '25349', '4293125348', '0', '0');
INSERT INTO `npc` VALUES ('4119', '13th of 23', '1985', '-205.123', '98.3215', '-48.3427', '0', '3868', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `outdated_spawntype` VALUES ('1', '1', 'thrax_footsoldiers', '25580', '0', '2', '15000', '700', '5.1', '1', '0', '1', '5', '8', '295');
INSERT INTO `outdated_spawntype` VALUES ('2', '1', 'boargar_general_spawn', '6031', '3', '0', '10000', '1000', '6.2', '10', '1', '174', '15', '15', '200');
INSERT INTO `outdated_spawntype` VALUES ('3', '1', 'bane_stalkerspawn_general', '3781', '0', '0', '40000', '2700', '3.3', '108', '0', '1', '37', '100', '1000');
INSERT INTO `outdated_spawntype` VALUES ('4', '1', 'forean_spearman_normal', '6043', '1', '0', '15000', '1800', '5.9', '204', '0', '1', '8', '0', '250');
INSERT INTO `outdated_spawntype` VALUES ('5', '1', 'forean_elder_normal', '6163', '1', '0', '25000', '2000', '4.9', '203', '0', '1', '9', '8', '200');
INSERT INTO `outdated_spawntype` VALUES ('6', '1', 'afs_soldier_light_m', '29765', '1', '0', '25000', '900', '5.7', '67', '0', '1', '5', '7', '210');
INSERT INTO `outdated_spawntype` VALUES ('7', '1', 'afs_soldier_medium_f', '29423', '1', '0', '25000', '1100', '5.1', '67', '0', '1', '7', '8', '310');
INSERT INTO `outdated_spawntype` VALUES ('8', '1', 'thrax_footsoldiers_rifle', '25581', '0', '0', '30000', '1200', '5.4', '79', '0', '1', '10', '11', '300');
INSERT INTO `outdated_spawntype` VALUES ('9', '1', 'afs_mech_standard', '10442', '1', '0', '30000', '2000', '5.6', '293', '0', '1', '10', '11', '400');
INSERT INTO `outdated_spawntype` VALUES ('10', '1', 'bane_hunter_invasion', '10166', '0', '2', '100000', '1500', '5.3', '1', '0', '1', '5', '9', '275');
INSERT INTO `outdated_spawntype` VALUES ('11', '1', 'bane_amoeboid_invasion', '6032', '0', '0', '100000', '1500', '5.1', '6032', '1', '174', '7', '7', '350');
INSERT INTO `outdated_spawntype` VALUES ('12', '1', 'thrax_footsoldier_invasion', '25580', '0', '2', '100000', '700', '5.1', '1', '0', '1', '10', '15', '350');
INSERT INTO `pathnodes` VALUES ('2', '-858993460', '-858993460', '-107374000', '-107374000', '-107374000', '-858993460');
INSERT INTO `pathnodes` VALUES ('3', '-858993460', '-858993460', '-107374000', '-107374000', '-107374000', '-858993460');
INSERT INTO `pathnodes` VALUES ('4', '31', '1220', '0', '-3.40282e38', '0', '22756288');
INSERT INTO `pathnodes` VALUES ('5', '31', '1220', '176.902', '163.381', '-50.293', '1');
INSERT INTO `spawnpool` VALUES ('108', '0', '1', '835.961', '294.027', '385.156', '1220', '1', '1', '1', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null);
INSERT INTO `teleporter` VALUES ('41', '8', '', '807.316', '294.055', '391.301', '0', '0', '0', '0', '0', '57');
INSERT INTO `teleporter` VALUES ('42', '8', '', '508.633', '238.461', '224.934', '0', '0', '0', '0', '0', '49');
INSERT INTO `teleporter` VALUES ('43', '8', '', '153.156', '163.004', '-110.672', '0', '0', '0', '0', '0', '61');
INSERT INTO `teleporter` VALUES ('44', '8', '', '-142.109', '220.27', '-499.461', '0', '0', '0', '0', '0', '51');
