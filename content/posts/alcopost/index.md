---
title: "Alco Post"
date: 2018-05-15
draft: false
tags: ["Linux", "Python", "Arduino"]
---
{{< load-photoswipe >}}

## Hardware and software complex of the checkpoint

- 1 turnstile any
- 2 controllers ZK MA300
- 2 alcotesters Alcofor S40
- 1 speaker and LED unit with voice prompts and mode indication
- 1 orange pi with python utility for displaying photos and status of passing through the turnstile and writing to the database

order of actions: authorization to ZK - alcohol test - in case of an error again - in case of an error, again record the violation and refusal to work (or notification of dismissal, but with permission to exit) - record a successful passage and farewell.

{{< gallery dir="posts/alcopost/photos" />}}


{{< embed-pdf url="posts/alcopost/alcopost.pdf" hidePaginator="true" >}}

[OrangePi Alco Post utility](OrangePi.zip)
{{<icon icon="download" link="OrangePi.zip" color="#6f0" stroke_width="2" width="32" height="32" >}}  

[Wiegand-V3-Library-for-all-Arduino.zip](Arduino/Wiegand-V3-Library-for-all-Arduino.zip)
{{<icon icon="download" link="Arduino/Wiegand-V3-Library-for-all-Arduino.zip" color="#6f0" stroke_width="2" width="32" height="32" >}}  

{{< codeaccordion lang="c" title="Arduino code" file="/posts/alcopost/Arduino/bio_alco_ctr_v2.ino" >}}
