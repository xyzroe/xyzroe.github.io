---
title: "XZG Firmware"
date: 2024-03-30
draft: false
tags: ["ESP32", "Zigbee", "CC2652"]
---

I've been working with Zigbee gateways for a while now, experimenting with different ESP32-based projects and CC2652 coordinators. Each one had its strengths — some excelled at MQTT integration, others at web interfaces or specific hardware support — but switching between them or trying to combine features was a hassle. Too many fragmented efforts meant slower progress for everyone.

That's why I created XZG Firmware. It's a unified solution that brings together the best innovations from various Zigbee gateway projects into one codebase. By consolidating efforts on a single, open-source firmware, we can focus on making it better: more features, better stability, easier setup.

<div align="center">
<video controls width="640">
  <source src="https://private-user-images.githubusercontent.com/6440415/329702792-ae312626-f1d9-41c4-b982-11a1f9ba9ed5.mp4?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NjUwOTcxOTYsIm5iZiI6MTc2NTA5Njg5NiwicGF0aCI6Ii82NDQwNDE1LzMyOTcwMjc5Mi1hZTMxMjYyNi1mMWQ5LTQxYzQtYjk4Mi0xMWExZjliYTllZDUubXA0P1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI1MTIwNyUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNTEyMDdUMDg0MTM2WiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9MmQ0OTM3NzUwOTQxMDNiYTNmMmYwMTEyMGFmZDFhMzE3YWFhYzgzNTIwYWQ0NDY2OWQ1YzdkMWNhMDY2ZjU4NCZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QifQ.NBdK2O-_NN9ryPWfUutDR2nmvUIVYo8QR-PZHOeHMOw" type="video/mp4">
  Your browser does not support video.
</video>
</div>

XZG runs on ESP32 boards and supports Zigbee coordinators like CC2652, CC1352, and more. It includes web-based configuration, MQTT support, OTA updates, and a ton of community-driven enhancements. Whether you're building a smart home hub or just tinkering, it's designed to be flexible and powerful.

The project has grown thanks to contributors who share their ideas and fixes. If you're interested, check out the repo at [github.com/xyzroe/XZG](https://github.com/xyzroe/XZG) or the docs at [xzg.xyzroe.cc](https://xzg.xyzroe.cc). Pull requests are welcome — let's make Zigbee gateways even better together.

It's rewarding to see how this has evolved from a personal project into something the community uses and improves. If it saves you time or sparks new ideas, that's the win.
