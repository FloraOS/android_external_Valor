# Valor
Valor is a very simple run-time checker of executable
files. It may be used for preventing harmful processes from operating
on Android device. 
 
In future it is planned to extend it possibilities also to
blocking IP addresses with help of modified netd or directly by
interacting with `iptables`.

# Installation
A [build.sh](https://github.com/Andrewerr/build.sh) is a available for installing
it with help of `adb root`, though it is not recommend as the daemon if over-priviliged 
then. It is better to integrate the daemon with your custom ROM.

# Integration
As `valord` scans app files it needs extended access to the system which 
is protected by a set of `neverallow` and `mls` rules in order to integrate
it with you ROM you would need patch your SEPolicy(see patch in `patches/` folder).
Here are commits associated with integration of SEPolicy to LineageOS 20.0-based PolarMod 1.3 ROM:
* [Patch neverallows](https://github.com/LineageOS/android_system_sepolicy/commit/4543fae09219b0cc092d84faf6fbecabaa2badc2)
* [Add SEPolicy](https://github.com/LineageOS/android_system_sepolicy/commit/172a314614f59ebe2b47886147ab3b41500ea799)

# Creating database_t
The CMakeLists.txt are provided to generate threat database_t. Currently it is not inteded to be a big one.
In order to create your database_t build `dbgen` utility and then use it to create database_t:
```bash
./dbgen --db valor.db --name mythreat --filename threat-executable-file
```

You may continue adding threats with this command. 

## Using database_t
By default the database_t is searched in local directory(see `Android.mk`)
though you may override `valor.db` module and copy from you own place.

# Future plans
* New algorithm of hashsum checking
* By-name process database_t
* Firewall set-up