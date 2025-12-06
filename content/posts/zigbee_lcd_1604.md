---
title: "Zigbee LCD 1604"
date: 2022-06-16
draft: false
tags: ["CC2530", "Zigbee", "DIY", "1604"]
---

Hey folks, I've been tinkering with home automation lately, and I came up with this cool little gadget: the Zigbee LCD 1604. It's basically a 16x4 character LCD that connects to your Zigbee network, perfect for displaying sensor data, notifications, or just fun messages around the house.

<div align="center">
<img alt="Zigbee LCD 1604" src="https://github.com/xyzroe/Zigbee-LCD-1604/raw/main/images/front_mini.jpeg">
</div>

What makes it awesome? You can send text to specific lines, control the backlight with motion detection, adjust brightness, and even display Cyrillic characters. It integrates seamlessly with PTVO firmware, so you can hook up all sorts of sensors like temperature, humidity, or light levels.

Building it is straightforward – grab an HD44780-compatible LCD with I2C, an Arduino Nano, and a Zigbee module like CC2530. Add a PIR sensor for auto-backlight, and you're good to go. The enclosure STL files are on Thingiverse, and everything's open-source on GitHub.

If you're into DIY smart home stuff, give this a try. It's been a blast to work on, and seeing it display real-time data is super satisfying. Let me know what you think!

##

[Check it out on GitHub](https://github.com/xyzroe/Zigbee-LCD-1604)
