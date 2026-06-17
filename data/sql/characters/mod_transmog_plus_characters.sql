CREATE TABLE IF NOT EXISTS `mod_transmog_plus` (
    `Owner` int(10) unsigned NOT NULL,
    `Slot` tinyint(3) unsigned NOT NULL,
    `FakeEntry` int(10) unsigned NOT NULL,
    PRIMARY KEY (`Owner`, `Slot`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `mod_transmog_plus_appearances` (
    `account_id` int(10) unsigned NOT NULL,
    `item_template_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
    PRIMARY KEY (`account_id`, `item_template_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
