const struct logo {
	unsigned char action;
	unsigned int x;
	unsigned int y;
}	logo_pix [] = {
//{0x1,0xED,0xC},
//{0x2,0xB9,0x22},
//{0x2,0x8C,0x3F},
//{0x2,0x68,0x61},
//{0x2,0x51,0x87},
//{0x2,0x3B,0xE6},
//{0x2,0x20,0x18B},
//{0x2,0xA,0x23A},
//{0x2,0x0,0x2B4},
//{0x2,0x3,0x2CD},
//{0x2,0xA,0x2E6},
//{0x2,0x15,0x2FD},
//{0x2,0x24,0x312},
//{0x2,0x36,0x325},
//{0x2,0x4B,0x336},
//{0x2,0x63,0x343},
//{0x2,0x7D,0x34D},
//{0x2,0x137,0x358},
//{0x2,0x292,0x35B},
//{0x2,0x3ED,0x357},
//{0x2,0x4A9,0x34B},
//{0x2,0x4DD,0x337},
//{0x2,0x50E,0x319},
//{0x2,0x534,0x2F7},
//{0x2,0x54B,0x2D4},
//{0x2,0x55F,0x26C},
//{0x2,0x57E,0x1A3},
//{0x2,0x591,0x10E},
//{0x2,0x599,0xB6},
//{0x2,0x597,0x86},
//{0x2,0x58C,0x65},
//{0x2,0x57B,0x46},
//{0x2,0x567,0x2E},
//{0x2,0x548,0x1D},
//{0x2,0x517,0x11},
//{0x2,0x464,0x6},
//{0x2,0x317,0x2},
//{0x2,0x1A0,0x4},
//{0x2,0xED,0xC},
//{0x1,0x455,0xF1},
//{0x2,0x462,0x102},
//{0x2,0x467,0x11B},
//{0x2,0x465,0x147},
//{0x2,0x45D,0x192},
//{0x2,0x44D,0x1F9},
//{0x2,0x43F,0x234},
//{0x2,0x432,0x244},
//{0x2,0x41E,0x251},
//{0x2,0x401,0x25B},
//{0x2,0x3D8,0x262},
//{0x2,0x35B,0x269},
//{0x2,0x293,0x268},
//{0x2,0x215,0x266},
//{0x2,0x1B8,0x263},
//{0x2,0x178,0x25C},
//{0x2,0x14F,0x24E},
//{0x2,0x142,0x244},
//{0x2,0x139,0x237},
//{0x2,0x134,0x227},
//{0x2,0x133,0x214},
//{0x2,0x136,0x1E1},
//{0x2,0x13F,0x19D},
//{0x2,0x14F,0x144},
//{0x2,0x161,0x10B},
//{0x2,0x16D,0xF8},
//{0x2,0x17B,0xEB},
//{0x2,0x18C,0xE1},
//{0x2,0x19F,0xDC},
//{0x2,0x212,0xD8},
//{0x2,0x2FD,0xD8},
//{0x2,0x3A6,0xD9},
//{0x2,0x40B,0xDD},
//{0x2,0x43E,0xE4},
//{0x2,0x455,0xF1},
//{0x1,0x13D6,0xC},
//{0x2,0x13A8,0x1F},
//{0x2,0x137E,0x37},
//{0x2,0x135C,0x54},
//{0x2,0x1343,0x75},
//{0x2,0x132D,0xB8},
//{0x2,0x1315,0x130},
//{0x2,0x12FF,0x1C8},
//{0x2,0x12EE,0x26B},
//{0x2,0x12EA,0x2B0},
//{0x2,0x12EB,0x2D9},
//{0x2,0x12F5,0x2F4},
//{0x2,0x130A,0x310},
//{0x2,0x1335,0x335},
//{0x2,0x1365,0x34D},
//{0x2,0x1412,0x357},
//{0x2,0x1581,0x35B},
//{0x2,0x167A,0x35A},
//{0x2,0x1718,0x357},
//{0x2,0x1775,0x350},
//{0x2,0x17A7,0x346},
//{0x2,0x17CD,0x337},
//{0x2,0x17EC,0x325},
//{0x2,0x1806,0x310},
//{0x2,0x181B,0x2F8},
//{0x2,0x182C,0x2DA},
//{0x2,0x183A,0x2B7},
//{0x2,0x1845,0x28E},
//{0x2,0x184D,0x25D},
//{0x2,0x1859,0x208},
//{0x2,0x17C9,0x208},
//{0x2,0x1778,0x209},
//{0x2,0x174C,0x20D},
//{0x2,0x1736,0x217},
//{0x2,0x172A,0x22A},
//{0x2,0x171E,0x23E},
//{0x2,0x170B,0x24D},
//{0x2,0x16F0,0x259},
//{0x2,0x16C8,0x261},
//{0x2,0x164A,0x269},
//{0x2,0x157B,0x268},
//{0x2,0x14FD,0x266},
//{0x2,0x14A0,0x263},
//{0x2,0x1460,0x25C},
//{0x2,0x1437,0x24F},
//{0x2,0x142A,0x244},
//{0x2,0x1421,0x237},
//{0x2,0x141C,0x227},
//{0x2,0x141B,0x214},
//{0x2,0x141E,0x1E2},
//{0x2,0x1427,0x19F},
//{0x2,0x1438,0x13A},
//{0x2,0x1444,0x108},
//{0x2,0x1450,0xF9},
//{0x2,0x1464,0xED},
//{0x2,0x1481,0xE3},
//{0x2,0x14A8,0xDC},
//{0x2,0x151E,0xD4},
//{0x2,0x15D7,0xD2},
//{0x2,0x1698,0xD4},
//{0x2,0x1708,0xDE},
//{0x2,0x1729,0xE6},
//{0x2,0x173E,0xF2},
//{0x2,0x1749,0x102},
//{0x2,0x174F,0x116},
//{0x2,0x1750,0x11F},
//{0x2,0x1754,0x127},
//{0x2,0x175B,0x12C},
//{0x2,0x1767,0x130},
//{0x2,0x1794,0x134},
//{0x2,0x17E6,0x135},
//{0x2,0x1877,0x135},
//{0x2,0x1881,0xE1},
//{0x2,0x1884,0xBC},
//{0x2,0x1883,0x9A},
//{0x2,0x187E,0x7D},
//{0x2,0x1874,0x63},
//{0x2,0x1866,0x4D},
//{0x2,0x1854,0x39},
//{0x2,0x183D,0x28},
//{0x2,0x1821,0x19},
//{0x2,0x17F3,0xB},
//{0x2,0x17A2,0x3},
//{0x2,0x1704,0x0},
//{0x2,0x15F1,0x0},
//{0x2,0x1484,0x3},
//{0x2,0x13D6,0xC},
//{0x1,0x199C,0x13},
//{0x2,0x1969,0x2D},
//{0x2,0x193D,0x50},
//{0x2,0x191B,0x7A},
//{0x2,0x1907,0xA8},
//{0x2,0x18DD,0x1AC},
//{0x2,0x18C0,0x2B3},
//{0x2,0x18C3,0x2CC},
//{0x2,0x18CA,0x2E5},
//{0x2,0x18D5,0x2FC},
//{0x2,0x18E4,0x312},
//{0x2,0x18F6,0x325},
//{0x2,0x190B,0x336},
//{0x2,0x1923,0x343},
//{0x2,0x193C,0x34D},
//{0x2,0x19F9,0x357},
//{0x2,0x1B58,0x35B},
//{0x2,0x1CB7,0x357},
//{0x2,0x1D72,0x34D},
//{0x2,0x1D9E,0x33B},
//{0x2,0x1DCB,0x31E},
//{0x2,0x1DF1,0x2FD},
//{0x2,0x1E0B,0x2DC},
//{0x2,0x1E1F,0x279},
//{0x2,0x1E3E,0x1AD},
//{0x2,0x1E52,0x111},
//{0x2,0x1E5A,0xB7},
//{0x2,0x1E58,0x87},
//{0x2,0x1E4C,0x65},
//{0x2,0x1E3E,0x4E},
//{0x2,0x1E2C,0x3B},
//{0x2,0x1E15,0x29},
//{0x2,0x1DF9,0x1A},
//{0x2,0x1DCB,0xB},
//{0x2,0x1D7A,0x3},
//{0x2,0x1CDC,0x0},
//{0x2,0x1BC8,0x0},
//{0x2,0x1AD3,0x1},
//{0x2,0x1A30,0x4},
//{0x2,0x19CE,0xA},
//{0x2,0x199C,0x13},
//{0x1,0x1CFF,0xE4},
//{0x2,0x1D13,0xF1},
//{0x2,0x1D1D,0x103},
//{0x2,0x1D20,0x125},
//{0x2,0x1D1F,0x160},
//{0x2,0x1D1C,0x1B8},
//{0x2,0x1D14,0x1FB},
//{0x2,0x1D0D,0x214},
//{0x2,0x1D03,0x22A},
//{0x2,0x1CF5,0x23C},
//{0x2,0x1CE3,0x24A},
//{0x2,0x1CCD,0x255},
//{0x2,0x1CB1,0x25D},
//{0x2,0x1C8F,0x263},
//{0x2,0x1C66,0x267},
//{0x2,0x1BFE,0x26B},
//{0x2,0x1B74,0x26B},
//{0x2,0x1AA5,0x269},
//{0x2,0x1A35,0x25E},
//{0x2,0x1A17,0x252},
//{0x2,0x1A06,0x242},
//{0x2,0x19FD,0x22C},
//{0x2,0x19F9,0x20F},
//{0x2,0x19FC,0x1C8},
//{0x2,0x1A08,0x16D},
//{0x2,0x1A11,0x13A},
//{0x2,0x1A1E,0x114},
//{0x2,0x1A27,0x105},
//{0x2,0x1A32,0xF8},
//{0x2,0x1A40,0xEE},
//{0x2,0x1A52,0xE6},
//{0x2,0x1A83,0xDA},
//{0x2,0x1ACA,0xD5},
//{0x2,0x1B2B,0xD2},
//{0x2,0x1BAC,0xD2},
//{0x2,0x1C3A,0xD3},
//{0x2,0x1C9E,0xD6},
//{0x2,0x1CDE,0xDC},
//{0x2,0x1CFF,0xE4},
//{0x1,0x634,0x39},
//{0x2,0x5FC,0x1D2},
//{0x2,0x5CA,0x350},
//{0x2,0x5F5,0x358},
//{0x2,0x65E,0x35B},
//{0x2,0x695,0x35B},
//{0x2,0x6BD,0x35A},
//{0x2,0x6D9,0x357},
//{0x2,0x6EC,0x34F},
//{0x2,0x6F8,0x342},
//{0x2,0x6FF,0x32D},
//{0x2,0x703,0x30F},
//{0x2,0x708,0x2E6},
//{0x2,0x710,0x295},
//{0x2,0x8DF,0x295},
//{0x2,0x9E6,0x295},
//{0x2,0xA77,0x291},
//{0x2,0xAA1,0x28D},
//{0x2,0xABE,0x287},
//{0x2,0xAD5,0x27F},
//{0x2,0xAEA,0x275},
//{0x2,0xB0A,0x264},
//{0x2,0xB23,0x252},
//{0x2,0xB38,0x23D},
//{0x2,0xB48,0x225},
//{0x2,0xB55,0x208},
//{0x2,0xB60,0x1E4},
//{0x2,0xB69,0x1B7},
//{0x2,0xB72,0x181},
//{0x2,0xB7D,0x12B},
//{0x2,0xB83,0xE5},
//{0x2,0xB84,0xAE},
//{0x2,0xB80,0x84},
//{0x2,0xB75,0x63},
//{0x2,0xB64,0x49},
//{0x2,0xB4B,0x34},
//{0x2,0xB2B,0x23},
//{0x2,0xAFE,0x17},
//{0x2,0xA9B,0x10},
//{0x2,0x9DD,0xD},
//{0x2,0x89B,0xC},
//{0x2,0x63C,0xC},
//{0x2,0x634,0x39},
//{0x1,0xA47,0xE8},
//{0x2,0xA5B,0xF6},
//{0x2,0xA68,0x109},
//{0x2,0xA6F,0x122},
//{0x2,0xA70,0x13C},
//{0x2,0xA6B,0x157},
//{0x2,0xA5F,0x171},
//{0x2,0xA4F,0x186},
//{0x2,0xA39,0x195},
//{0x2,0xA12,0x19C},
//{0x2,0x9C3,0x1A1},
//{0x2,0x94A,0x1A4},
//{0x2,0x8A5,0x1A6},
//{0x2,0x735,0x1A6},
//{0x2,0x73F,0x15B},
//{0x2,0x744,0x120},
//{0x2,0x747,0xF8},
//{0x2,0x749,0xF0},
//{0x2,0x751,0xEB},
//{0x2,0x762,0xE6},
//{0x2,0x780,0xE4},
//{0x2,0x7EF,0xE1},
//{0x2,0x8BC,0xE0},
//{0x2,0x9CA,0xE3},
//{0x2,0xA47,0xE8},
//{0x1,0xBFF,0x24},
//{0x2,0xBF8,0x50},
//{0x2,0xBEF,0x95},
//{0x2,0xBE6,0xEE},
//{0x2,0xCDE,0xEE},
//{0x2,0xD65,0xEF},
//{0x2,0xDAF,0xF2},
//{0x2,0xDC4,0xF5},
//{0x2,0xDCF,0xF9},
//{0x2,0xDD5,0xFF},
//{0x2,0xDD6,0x106},
//{0x2,0xDCA,0x16C},
//{0x2,0xDAD,0x23E},
//{0x2,0xD87,0x35B},
//{0x2,0xE17,0x35B},
//{0x2,0xEA7,0x35B},
//{0x2,0xEB0,0x32E},
//{0x2,0xEBE,0x2C9},
//{0x2,0xED4,0x216},
//{0x2,0xEEB,0x169},
//{0x2,0xEF9,0x10D},
//{0x2,0xEFB,0x103},
//{0x2,0xF00,0xFB},
//{0x2,0xF0B,0xF6},
//{0x2,0xF1E,0xF2},
//{0x2,0xF67,0xEF},
//{0x2,0xFF4,0xEE},
//{0x2,0x10EA,0xEE},
//{0x2,0x10F1,0xC0},
//{0x2,0x10F9,0x8C},
//{0x2,0x1103,0x4F},
//{0x2,0x110B,0xC},
//{0x2,0xE89,0xC},
//{0x2,0xD28,0xD},
//{0x2,0xC66,0x10},
//{0x2,0xC32,0x13},
//{0x2,0xC13,0x17},
//{0x2,0xC04,0x1D},
//{0x2,0xBFF,0x24},
//{0x1,0x118F,0x1D},
//{0x2,0x1182,0x6E},
//{0x2,0x116B,0x118},
//{0x2,0x1160,0x176},
//{0x2,0x1155,0x1C9},
//{0x2,0x114D,0x209},
//{0x2,0x1148,0x22C},
//{0x2,0x1130,0x2E2},
//{0x2,0x1125,0x347},
//{0x2,0x112B,0x351},
//{0x2,0x1141,0x357},
//{0x2,0x116C,0x35A},
//{0x2,0x11B2,0x35B},
//{0x2,0x1201,0x35A},
//{0x2,0x122C,0x357},
//{0x2,0x1237,0x353},
//{0x2,0x123E,0x34D},
//{0x2,0x1242,0x346},
//{0x2,0x1245,0x33C},
//{0x2,0x1255,0x2D2},
//{0x2,0x1272,0x201},
//{0x2,0x128F,0x11F},
//{0x2,0x12A3,0x8B},
//{0x2,0x12AB,0x45},
//{0x2,0x12B0,0x1D},
//{0x2,0x12A8,0x15},
//{0x2,0x1290,0x10},
//{0x2,0x1264,0xD},
//{0x2,0x1223,0xC},
//{0x2,0x11E1,0xD},
//{0x2,0x11B4,0x11},
//{0x2,0x1199,0x16},
//{0x2,0x118F,0x1D},
//{0x1,0x1EED,0x18},
//{0x2,0x1EBB,0x19A},
//{0x2,0x1E83,0x333},
//{0x2,0x1E7D,0x35B},
//{0x2,0x1F0F,0x35B},
//{0x2,0x1FA0,0x35B},
//{0x2,0x1FB1,0x2DD},
//{0x2,0x1FBF,0x27C},
//{0x2,0x1FC7,0x245},
//{0x2,0x1FC9,0x233},
//{0x2,0x1FCD,0x215},
//{0x2,0x1FD1,0x1EE},
//{0x2,0x1FD5,0x1C5},
//{0x2,0x1FE1,0x17D},
//{0x2,0x1FED,0x15F},
//{0x2,0x2047,0x1AA},
//{0x2,0x210C,0x25D},
//{0x2,0x221C,0x35B},
//{0x2,0x22C6,0x35B},
//{0x2,0x236F,0x35B},
//{0x2,0x2377,0x32C},
//{0x2,0x2389,0x2AB},
//{0x2,0x23AA,0x1B4},
//{0x2,0x23CB,0xBC},
//{0x2,0x23DD,0x39},
//{0x2,0x23E5,0xC},
//{0x2,0x2353,0xC},
//{0x2,0x22C0,0xC},
//{0x2,0x229E,0x10D},
//{0x2,0x2286,0x1C5},
//{0x2,0x2279,0x215},
//{0x2,0x2223,0x1CB},
//{0x2,0x2157,0x113},
//{0x2,0x2038,0xC},
//{0x2,0x1F93,0xC},
//{0x2,0x1F1E,0x10},
//{0x2,0x1EED,0x18},
};