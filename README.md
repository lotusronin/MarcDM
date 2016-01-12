# MarcDM

MarcDM is my attempt at a very simple login manager for Linux with some customizability.
It is written in C++ and uses Qt5 for rendering its windows.

MarcDM is meant to be a modern and lean display manager so it doesn't support some features that other display managers might. 
It is meant to load quickly and support a signle login session at a time.
Systemd is a requirement for power options to work.

## Requirements

Requirements for building:
* Qt5
* xcb
* pam & pam_misc
* systemd
