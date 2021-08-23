
#pragma once
#include "Femto"

namespace Graphics {

    inline constexpr u16 generalPalette[] = {
        colorFromRGB(0, 0, 0),
        colorFromRGB(1, 1, 1),
        colorFromRGB(2, 2, 2),
        colorFromRGB(3, 3, 3),
        colorFromRGB(4, 4, 4),
        colorFromRGB(5, 5, 5),
        colorFromRGB(6, 6, 6),
        colorFromRGB(7, 7, 7),
        colorFromRGB(8, 8, 8),
        colorFromRGB(9, 9, 9),
        colorFromRGB(10, 10, 10),
        colorFromRGB(11, 11, 11),
        colorFromRGB(12, 12, 12),
        colorFromRGB(13, 13, 13),
        colorFromRGB(14, 14, 14),
        colorFromRGB(15, 15, 15),
        colorFromRGB(16, 16, 16),
        colorFromRGB(17, 17, 17),
        colorFromRGB(18, 18, 18),
        colorFromRGB(19, 19, 19),
        colorFromRGB(20, 20, 20),
        colorFromRGB(21, 21, 21),
        colorFromRGB(22, 22, 22),
        colorFromRGB(23, 23, 23),
        colorFromRGB(24, 24, 24),
        colorFromRGB(25, 25, 25),
        colorFromRGB(26, 26, 26),
        colorFromRGB(27, 27, 27),
        colorFromRGB(28, 28, 28),
        colorFromRGB(29, 29, 29),
        colorFromRGB(30, 30, 30),
        colorFromRGB(31, 31, 31),
        colorFromRGB(32, 32, 32),
        colorFromRGB(33, 33, 33),
        colorFromRGB(34, 34, 34),
        colorFromRGB(35, 35, 35),
        colorFromRGB(36, 36, 36),
        colorFromRGB(37, 37, 37),
        colorFromRGB(38, 38, 38),
        colorFromRGB(39, 39, 39),
        colorFromRGB(40, 40, 40),
        colorFromRGB(41, 41, 41),
        colorFromRGB(42, 42, 42),
        colorFromRGB(43, 43, 43),
        colorFromRGB(44, 44, 44),
        colorFromRGB(45, 45, 45),
        colorFromRGB(46, 46, 46),
        colorFromRGB(47, 47, 47),
        colorFromRGB(48, 48, 48),
        colorFromRGB(49, 49, 49),
        colorFromRGB(50, 50, 50),
        colorFromRGB(51, 51, 51),
        colorFromRGB(52, 52, 52),
        colorFromRGB(53, 53, 53),
        colorFromRGB(54, 54, 54),
        colorFromRGB(55, 55, 55),
        colorFromRGB(56, 56, 56),
        colorFromRGB(57, 57, 57),
        colorFromRGB(58, 58, 58),
        colorFromRGB(59, 59, 59),
        colorFromRGB(60, 60, 60),
        colorFromRGB(61, 61, 61),
        colorFromRGB(62, 62, 62),
        colorFromRGB(63, 63, 63),
        colorFromRGB(64, 64, 64),
        colorFromRGB(65, 65, 65),
        colorFromRGB(66, 66, 66),
        colorFromRGB(67, 67, 67),
        colorFromRGB(68, 68, 68),
        colorFromRGB(69, 69, 69),
        colorFromRGB(70, 70, 70),
        colorFromRGB(71, 71, 71),
        colorFromRGB(72, 72, 72),
        colorFromRGB(73, 73, 73),
        colorFromRGB(74, 74, 74),
        colorFromRGB(75, 75, 75),
        colorFromRGB(76, 76, 76),
        colorFromRGB(77, 77, 77),
        colorFromRGB(78, 78, 78),
        colorFromRGB(79, 79, 79),
        colorFromRGB(80, 80, 80),
        colorFromRGB(81, 81, 81),
        colorFromRGB(82, 82, 82),
        colorFromRGB(83, 83, 83),
        colorFromRGB(84, 84, 84),
        colorFromRGB(85, 85, 85),
        colorFromRGB(86, 86, 86),
        colorFromRGB(87, 87, 87),
        colorFromRGB(88, 88, 88),
        colorFromRGB(89, 89, 89),
        colorFromRGB(90, 90, 90),
        colorFromRGB(91, 91, 91),
        colorFromRGB(92, 92, 92),
        colorFromRGB(93, 93, 93),
        colorFromRGB(94, 94, 94),
        colorFromRGB(95, 95, 95),
        colorFromRGB(96, 96, 96),
        colorFromRGB(97, 97, 97),
        colorFromRGB(98, 98, 98),
        colorFromRGB(99, 99, 99),
        colorFromRGB(100, 100, 100),
        colorFromRGB(101, 101, 101),
        colorFromRGB(102, 102, 102),
        colorFromRGB(103, 103, 103),
        colorFromRGB(104, 104, 104),
        colorFromRGB(105, 105, 105),
        colorFromRGB(106, 106, 106),
        colorFromRGB(107, 107, 107),
        colorFromRGB(108, 108, 108),
        colorFromRGB(109, 109, 109),
        colorFromRGB(110, 110, 110),
        colorFromRGB(111, 111, 111),
        colorFromRGB(112, 112, 112),
        colorFromRGB(113, 113, 113),
        colorFromRGB(114, 114, 114),
        colorFromRGB(115, 115, 115),
        colorFromRGB(116, 116, 116),
        colorFromRGB(117, 117, 117),
        colorFromRGB(118, 118, 118),
        colorFromRGB(119, 119, 119),
        colorFromRGB(120, 120, 120),
        colorFromRGB(121, 121, 121),
        colorFromRGB(122, 122, 122),
        colorFromRGB(123, 123, 123),
        colorFromRGB(124, 124, 124),
        colorFromRGB(125, 125, 125),
        colorFromRGB(126, 126, 126),
        colorFromRGB(127, 127, 127),
        colorFromRGB(128, 128, 128),
        colorFromRGB(129, 129, 129),
        colorFromRGB(130, 130, 130),
        colorFromRGB(131, 131, 131),
        colorFromRGB(132, 132, 132),
        colorFromRGB(133, 133, 133),
        colorFromRGB(134, 134, 134),
        colorFromRGB(135, 135, 135),
        colorFromRGB(136, 136, 136),
        colorFromRGB(137, 137, 137),
        colorFromRGB(138, 138, 138),
        colorFromRGB(139, 139, 139),
        colorFromRGB(140, 140, 140),
        colorFromRGB(141, 141, 141),
        colorFromRGB(142, 142, 142),
        colorFromRGB(143, 143, 143),
        colorFromRGB(144, 144, 144),
        colorFromRGB(145, 145, 145),
        colorFromRGB(146, 146, 146),
        colorFromRGB(147, 147, 147),
        colorFromRGB(148, 148, 148),
        colorFromRGB(149, 149, 149),
        colorFromRGB(150, 150, 150),
        colorFromRGB(151, 151, 151),
        colorFromRGB(152, 152, 152),
        colorFromRGB(153, 153, 153),
        colorFromRGB(154, 154, 154),
        colorFromRGB(155, 155, 155),
        colorFromRGB(156, 156, 156),
        colorFromRGB(157, 157, 157),
        colorFromRGB(158, 158, 158),
        colorFromRGB(159, 159, 159),
        colorFromRGB(160, 160, 160),
        colorFromRGB(161, 161, 161),
        colorFromRGB(162, 162, 162),
        colorFromRGB(163, 163, 163),
        colorFromRGB(164, 164, 164),
        colorFromRGB(165, 165, 165),
        colorFromRGB(166, 166, 166),
        colorFromRGB(167, 167, 167),
        colorFromRGB(168, 168, 168),
        colorFromRGB(169, 169, 169),
        colorFromRGB(170, 170, 170),
        colorFromRGB(171, 171, 171),
        colorFromRGB(172, 172, 172),
        colorFromRGB(173, 173, 173),
        colorFromRGB(174, 174, 174),
        colorFromRGB(175, 175, 175),
        colorFromRGB(176, 176, 176),
        colorFromRGB(177, 177, 177),
        colorFromRGB(178, 178, 178),
        colorFromRGB(179, 179, 179),
        colorFromRGB(180, 180, 180),
        colorFromRGB(181, 181, 181),
        colorFromRGB(182, 182, 182),
        colorFromRGB(183, 183, 183),
        colorFromRGB(184, 184, 184),
        colorFromRGB(185, 185, 185),
        colorFromRGB(186, 186, 186),
        colorFromRGB(187, 187, 187),
        colorFromRGB(188, 188, 188),
        colorFromRGB(189, 189, 189),
        colorFromRGB(190, 190, 190),
        colorFromRGB(191, 191, 191),
        colorFromRGB(192, 192, 192),
        colorFromRGB(193, 193, 193),
        colorFromRGB(194, 194, 194),
        colorFromRGB(195, 195, 195),
        colorFromRGB(196, 196, 196),
        colorFromRGB(197, 197, 197),
        colorFromRGB(198, 198, 198),
        colorFromRGB(199, 199, 199),
        colorFromRGB(200, 200, 200),
        colorFromRGB(201, 201, 201),
        colorFromRGB(202, 202, 202),
        colorFromRGB(203, 203, 203),
        colorFromRGB(204, 204, 204),
        colorFromRGB(205, 205, 205),
        colorFromRGB(206, 206, 206),
        colorFromRGB(207, 207, 207),
        colorFromRGB(208, 208, 208),
        colorFromRGB(209, 209, 209),
        colorFromRGB(210, 210, 210),
        colorFromRGB(211, 211, 211),
        colorFromRGB(212, 212, 212),
        colorFromRGB(213, 213, 213),
        colorFromRGB(214, 214, 214),
        colorFromRGB(215, 215, 215),
        colorFromRGB(216, 216, 216),
        colorFromRGB(217, 217, 217),
        colorFromRGB(218, 218, 218),
        colorFromRGB(219, 219, 219),
        colorFromRGB(220, 220, 220),
        colorFromRGB(221, 221, 221),
        colorFromRGB(222, 222, 222),
        colorFromRGB(223, 223, 223),
        colorFromRGB(224, 224, 224),
        colorFromRGB(225, 225, 225),
        colorFromRGB(226, 226, 226),
        colorFromRGB(227, 227, 227),
        colorFromRGB(228, 228, 228),
        colorFromRGB(229, 229, 229),
        colorFromRGB(230, 230, 230),
        colorFromRGB(231, 231, 231),
        colorFromRGB(232, 232, 232),
        colorFromRGB(233, 233, 233),
        colorFromRGB(234, 234, 234),
        colorFromRGB(235, 235, 235),
        colorFromRGB(236, 236, 236),
        colorFromRGB(237, 237, 237),
        colorFromRGB(238, 238, 238),
        colorFromRGB(239, 239, 239),
        colorFromRGB(240, 240, 240),
        colorFromRGB(241, 241, 241),
        colorFromRGB(242, 242, 242),
        colorFromRGB(243, 243, 243),
        colorFromRGB(244, 244, 244),
        colorFromRGB(245, 245, 245),
        colorFromRGB(246, 246, 246),
        colorFromRGB(247, 247, 247),
        colorFromRGB(248, 248, 248),
        colorFromRGB(249, 249, 249),
        colorFromRGB(250, 250, 250),
        colorFromRGB(251, 251, 251),
        colorFromRGB(252, 252, 252),
        colorFromRGB(253, 253, 253),
        colorFromRGB(254, 254, 254),
        colorFromRGB(255, 255, 255)
    };

}

