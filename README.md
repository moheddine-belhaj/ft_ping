# ft_ping
42 Heilbronn Mastery project where I recreate the `ping` command, which allows you to test the accessibility of another machine through the IP network.

# SUBJECT

This project is about recoding the ping command. 

(Subject full PDF: [subject_fr_ping.pdf](docs/en.subject_fr_ping.pdf) )

## Introduction

Ping is the name of a command that allows you to test the accessibility of another machine through the IP network. The command also measures the time taken to receive a response, called the round-trip time (RTT).

## General Instructions

- The project must be realized in a virtual machine running on Debian (>= 7.0).
- The virtual machine must have all the necessary software to complete the project, configured and installed.
- You must be able to use your virtual machine from a cluster computer.
- The project is corrected by humans only. You may organise and name your files as you see fit, but you must follow these rules:
  - You must use **C** and submit a **Makefile**.
  - The Makefile must compile the project and contain the usual rules. It must recompile and re-link the program only if necessary.
  - Errors must be handled carefully. The program must never quit in an unexpected manner (segmentation fault, bus error, double free, etc.).
  - You are authorised to use the libc functions to complete this project.

> ⚠️ Program in C, all libC is authorised. Using the system `ping` or the sources of a standard ping in any way is **forbidden**.

## Mandatory Part

- The executable must be named `ft_ping`.
- The reference is the ping implementation from **inetutils-2.0** (`ping -V`).
- You have to manage the `-v` and `-?` options.

> ⚠️ The `-v` option also allows us to see the results in case of a problem or an error linked to the packets, which logically shouldn't force the program to stop (modifying the TTL value can help to force an error).

- You have to manage a simple IPv4 (address/hostname) as parameter.
- You have to manage FQDN without doing the DNS resolution in the packet return.

> 💡 You are allowed to use all the functions of the `printf` family.

> ⚠️ Obviously you are **NOT** allowed to call a real `ping`.

## Bonus Part

A few ideas of interesting bonuses:

- Additional `-f -l -n -w -W -p -r -s -T --ttl --ip-timestamp` flags...

> 💡 The flags `-V`, `--usage`, `--echo` are not considered as bonus.
>
> 💡 Two flags corresponding to the same feature (e.g. `-t` and `--type`) are not considered as two bonuses.

> 💡 A delay of +/- 30ms is tolerated on the reception of a packet.
