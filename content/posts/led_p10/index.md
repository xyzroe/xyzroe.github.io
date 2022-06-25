---
title: "Using LED P-10 to display txt file"
date: 2020-05-05
draft: false
tags: ["LED", "RS485", "python", "DIY"]
---

I need to display actual number of tickets in our service support system using big LED display. 🚨  

I found LED P10 modules, they are cheap and nice, different colors available. Modules can be connected one by one to make big screen.💡  

Then I need some hardware to connect to P-10 and I found [VBC-P10 module](https://vkmodule.com.ua/LED_P10/ledVBC_P10_1R.html) from great Ukrainian 🇺🇦 manufacture VK Module. But it has only RS485 and TTL ports, while I need Ethernet so I add [Ethernet - RS485](https://vkmodule.com.ua/Converter/ConverterE485.html) converter. Now I can print numbers on P-10 sending TCP packets to converter IP address. 🚀   

Next step is get current active tickets count. I found out that the one way than our service support system can export number of current active tickets is save txt file with this count, each time it changes. 😕  

So I wrote small Python script that reads this file very 5 seconds and if any changes make command and send it to P-10. 📡  

Runs using PM2. ecosystem.config.js file is very simple. You can find it at the bottom, after the script. 🥳  



{{< codeaccordion lang="python" title="Python code" file="/posts/led_p10/led_p10.py" >}}

{{< codeaccordion lang="js" title="ecosystem.config.js" file="/posts/led_p10/ecosystem.config.js" >}}
