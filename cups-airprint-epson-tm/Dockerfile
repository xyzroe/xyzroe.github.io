FROM ubuntu:xenial

# Install the packages we need. Avahi will be included
RUN apt-get update && apt-get install -y \
	cups \
	cups-pdf \
	inotify-tools \
	python-cups \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/*

# This will use port 631
EXPOSE 631

# We want a mount for these
VOLUME /config
VOLUME /services

# Add scripts
ADD root /

# Add rastertozj filter and Epson tm t-20II drivers (2 configs)
RUN cd /epson \
	&& cp rastertozj /usr/lib/cups/filter/ \
	&& chmod +x  /usr/lib/cups/filter/rastertozj \
	&& mkdir -p /usr/share/cups/model/Epson \
	&& cp tm-t20-NP.ppd /usr/share/cups/model/Epson/ \
	&& cp tm-t20-TG.ppd /usr/share/cups/model/Epson/ \
	&& cp tm-t20-orig.ppd /usr/share/cups/model/Epson/
CMD ["/root/run_cups.sh"]

RUN chmod +x /root/*
CMD ["/root/run_cups.sh"]

# Baked-in config file changes
RUN sed -i 's/Listen localhost:631/Listen 0.0.0.0:631/' /etc/cups/cupsd.conf && \
	sed -i 's/Browsing Off/Browsing On/' /etc/cups/cupsd.conf && \
	sed -i 's/<Location \/>/<Location \/>\n  Allow All/' /etc/cups/cupsd.conf && \
	sed -i 's/<Location \/admin>/<Location \/admin>\n  Allow All\n  Require user @SYSTEM/' /etc/cups/cupsd.conf && \
	sed -i 's/<Location \/admin\/conf>/<Location \/admin\/conf>\n  Allow All/' /etc/cups/cupsd.conf && \
	echo "ServerAlias *" >> /etc/cups/cupsd.conf && \
	echo "DefaultEncryption Never" >> /etc/cups/cupsd.conf
