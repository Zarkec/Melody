/*
 Navicat Premium Data Transfer

 Source Server         : localhost_3306
 Source Server Type    : MySQL
 Source Server Version : 80036
 Source Host           : localhost:3306
 Source Schema         : melody_db

 Target Server Type    : MySQL
 Target Server Version : 80036
 File Encoding         : 65001

 Date: 18/11/2024 13:55:21
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for music_infos
-- ----------------------------
DROP TABLE IF EXISTS `music_infos`;
CREATE TABLE `music_infos`  (
  `music_id` int NOT NULL AUTO_INCREMENT,
  `music_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `music_anthor` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `music_picurl` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `music_url` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  PRIMARY KEY (`music_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of music_infos
-- ----------------------------
INSERT INTO `music_infos` VALUES (1, 'Don’t say “lazy”', '放課後ティータイム', 'D:/cppCode/last/Melody/res/music/Don\'t say lazy.jpg', 'D:/cppCode/last/Melody/res/music/Don\'t say lazy.mp3');
INSERT INTO `music_infos` VALUES (2, 'ギターと孤独と蒼い惑星', '結束バンド', 'D:/cppCode/last/Melody/res/music/ギターと孤独と蒼い惑星 - 結束バンド.jpg', 'D:/cppCode/last/Melody/res/music/ギターと孤独と蒼い惑星 - 結束バンド.mp3');

SET FOREIGN_KEY_CHECKS = 1;
