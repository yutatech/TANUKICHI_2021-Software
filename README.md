# TANUKICHI_2021-Software
This software is developed for RoboCup Junior Soccer. The robot has competed as "CAIO Tezukayama" in 2019 and "Leopard" in 2021.

## Related Web sites
- [Yuta's Blog](http://yuta.techblog.jp)
- [<img width="12px" src="https://upload.wikimedia.org/wikipedia/commons/thumb/0/09/YouTube_full-color_icon_%282017%29.svg/318px-YouTube_full-color_icon_%282017%29.svg.png"> Yuta - YouTube Channel](https://www.youtube.com/@yuta9428)
- [<img width="12px" src="https://upload.wikimedia.org/wikipedia/commons/thumb/4/4f/Twitter-logo.svg/300px-Twitter-logo.svg.png"> Yuta@ciao_yuta - Twitter](https://twitter.com/ciao_yuta)

## Related Repositories
- [yutatech/RoboCup2021](https://github.com/yutatech/RoboCup2021)
- [yutatech/TANUKICHI_2021-PCB](https://github.com/yutatech/TANUKICHI_2021-PCB)

## Related Resources
#### TDP
- [Leopard RoboCup 2021 Team Discription Paper](http://yuta.techblog.jp/RoboCupJapanTournament2021/Leopard%20RoboCup%202021%20Team%20Discription%20Paper.pdf)

#### Video
- [Video - Leopard (Soccer Open) - RoboCup 2021 WORLDWIDE](https://www.youtube.com/watch?v=Lgoi_pQrCUY)
	- [This video](https://www.youtube.com/watch?v=PFkYVE7N84A) and [this video](https://www.youtube.com/watch?v=1l03lD6fXiQ) are also the same. Uploaded from TokyoTech.

## Awards
- ロボカップジュニア日本大会2021オンライン (RoboCup Junior Japan Tournament 2021 Online)
	- サッカーオープン 優勝 (SoccerOpen 1st Place)
	- ベストプレゼンビデオ賞 (Best Presentation Video Award)
	- ベストインタビュー賞 (Best Interview Award)
	- ベストビジュアルポスター賞 (Best Visual Poster Award)
- RoboCup 2021 WORLDWIDE
	- SoccerOpen Indvidual 1st Place
	- SoccerOpen SuperTeam 1st Place

## Source Codes
- Mbed
	- TANUKICHI_2021
		- This project is a program that runs on the main MCU developed on Mbed OS 5. The robot's GUI and motion algorithms are described. The MCU is STM32F446RET6.
- Arduino
	- Encoder-attiny84-ID0x07
	- Encoder-attiny84-ID0x08
		- These projects are handles magnetic pulse encoders. Its MCU is ATtiny84 and developed on Arduino IDE.
	- LineSensor-Front
	- LineSensor-Right
	- LineSensor-Left
	- LineSensor-Back
		- These projects handle each line sensor unit. Its MCU is ATtiny828 and developed on Arduino IDE.
	- UpperCentralUnit
		- This project handles deveices mounted on the upper unit, such as ultrasonic sensors, decorative LEDs, and display backlights. Its MCU is ATMEGA328P and developed on Arduino IDE.
- OpenMV
	- This project is a program that detects the passive ball and goals with a camera. Its camera module is OpenMV M7 and developed on OpenMV IDE.