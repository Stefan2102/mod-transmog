SET
@Entry = 190012,
@Name = "Warpweaver";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `CreatureImmunitiesId`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, "Transmogrifier", NULL, 0, 80, 80, 2, 35, 1, 0, 0, 2000, 0, 1, 0, 7, 138936390, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, 'npc_transmogrifier');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, 19646, 1, 1, 0);

DELETE FROM `creature_template_locale` WHERE `entry` = @Entry;
INSERT INTO `creature_template_locale` (`entry`, `locale`, `Name`, `Title`) VALUES
(@Entry, 'koKR', @Name, "변형기"),
(@Entry, 'frFR', @Name, "Transmogrificateur"),
(@Entry, 'deDE', @Name, "Transmogrifier"),
(@Entry, 'zhCN', @Name, "变形者"),
(@Entry, 'zhTW', @Name, "幻化大師"),
(@Entry, 'esES', @Name, "Transfigurador"),
(@Entry, 'esMX', @Name, "Transfigurador"),
(@Entry, 'ruRU', @Name, "Трансмогрификатор");

SET @GREETING := 601084;
DELETE FROM `npc_text` WHERE `ID` = @GREETING;
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(@GREETING, 'Greetings, $N. Do you wish to alter the appearance of your equipment?');

DELETE FROM `module_string` WHERE `module` = 'mod-transmog-plus';
INSERT INTO `module_string` (`module`, `id`, `string`) VALUES
('mod-transmog-plus', 1, 'Slot transmogrified.'),
('mod-transmog-plus', 2, 'Invalid source item.'),
('mod-transmog-plus', 3, 'All slot transmogs removed.'),
('mod-transmog-plus', 4, 'Slot hidden.'),
('mod-transmog-plus', 5, 'Slot transmog removed.'),
('mod-transmog-plus', 6, 'You do not have enough money.'),
('mod-transmog-plus', 7, 'Remove all transmogs'),
('mod-transmog-plus', 8, 'Remove all slot transmogs?'),
('mod-transmog-plus', 9, 'Back...'),
('mod-transmog-plus', 10, 'Hide slot'),
('mod-transmog-plus', 11, 'Remove transmog from slot'),
('mod-transmog-plus', 12, 'Next Page'),
('mod-transmog-plus', 13, 'Previous Page'),
('mod-transmog-plus', 14, 'No valid appearances found for this slot.'),
('mod-transmog-plus', 15, 'This item\'s appearance has been added to your collection.'),
('mod-transmog-plus', 16, 'Free');
