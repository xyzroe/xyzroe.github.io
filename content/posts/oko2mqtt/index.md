---
title: "oko2mqtt"
date: 2021-01-10
draft: false
tags: ["oko.ukr", "MQTT", "Security", "Python", "Socket"]
---
{{< load-photoswipe >}}

During the New Year holidays, we got around to finishing and posting on GitHub the code that allows you to raise your TCP server for OKO devices with state transfer and the ability to control via MQTT. 💡

The MQTT AutoDiscovery mechanism has been implemented, which allows you to automatically add all devices / inputs / outputs / sensors to HomeAssistant. Supports work simultaneously with multiple devices, multithreading.

Currently, support for expansion cards and radio sockets is limited.
Work with the rest of the inputs, outputs, groups, sensors, counters is fully implemented.

Detailed documentation is available on GitHub.
[GitHub: xyzroe/oko2mqtt](https://github.com/xyzroe/oko2mqtt/blob/main/doc/BEGIN.md)

There is a ready-made Docker image, but it can be launched as a separate application / service.
[dockerhub: xyzroe/oko2mqtt](https://hub.docker.com/r/xyzroe/oko2mqtt)


If suddenly it is useful to someone, use it. :)
If someone contributes their improvements / fixes - welcome to the PR on GitHub. ;)

{{< readfile file="projects/oko2mqtt/README.md" markdown="true" >}}


![MQTT Explorer](https://github.com/xyzroe/oko2mqtt/raw/master/doc/img/mqtt_oko.png?raw=true)


{{< figure src="https://github.com/xyzroe/oko2mqtt/raw/master/doc/img/ha_lovelace_oko.png?raw=true" caption="HA interface" caption-position="bottom" >}}
