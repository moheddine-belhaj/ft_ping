# Debian 11 (bullseye) ships inetutils 2.0, the reference for ft_ping.
# Bullseye is EOL, so its packages live on archive.debian.org.
FROM debian:bullseye

RUN printf '%s\n' \
	'deb http://archive.debian.org/debian bullseye main' \
	'deb http://archive.debian.org/debian bullseye-updates main' \
	'deb http://archive.debian.org/debian-security bullseye-security main' \
	> /etc/apt/sources.list \
	&& echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99archive \
	&& apt-get update \
	&& apt-get install -y --no-install-recommends \
		build-essential gdb valgrind tcpdump iproute2 inetutils-ping \
	&& rm -rf /var/lib/apt/lists/*

WORKDIR /src
CMD ["bash"]
