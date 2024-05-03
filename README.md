# Valor
Valor is a very simple run-time checker of executable
files. It may be used for preventing harmful processes from operating
on Android device.

# Building
For building either `CMakeLists.txt` should be used or docker. For cmake building case p
lease note that by default valord target is also 
included, so you need to have Android NDK installed under `ANDROID_NDK` path for CMakeLists.txt
(by default set to `/opt/android-ndk`)

# Integration
As `valord` scans app files it needs extended access to the system which 
is protected by a set of `neverallow` and `mls` rules in order to integrate
it with you ROM you would need patch your SEPolicy(see patch in `patches/` folder).
Here are commits associated with integration of SEPolicy to LineageOS 20.0-based PolarMod 1.3 ROM:
* [Patch neverallows](https://github.com/LineageOS/android_system_sepolicy/commit/4543fae09219b0cc092d84faf6fbecabaa2badc2)
* [Add SEPolicy](https://github.com/LineageOS/android_system_sepolicy/commit/172a314614f59ebe2b47886147ab3b41500ea799)

## Makefiles
To use `Android.mk` files provided you just can add `valord` target to `PRODUCT_PACKGES` like 
[here](https://github.com/MilkyWayOS/android_vendor_milkyway/blob/1172547a1ad68eff30f5d02626f15abbc2bd4ee3/config/config.mk#L14).

# Database
The database should be created on the host machine. You may use `dbgen` utility for creating it.
## Version
Database has version which is logged during startup of valord. The argument `--increment-version` is used to update 
version.
It is reccomended to keep version 0 for empty database which is commonly should be used for debugging.

## Using database
By default makefiles are setupped to search database in local directory in tree, though if you want provide database
separately you may override `valor.db` module. 
The database itself is currently stored under `/system/etc/valor.db`

# Future plans
* Better alogrithms for threat detection
* Active protection: scan also memory-mapped files
* Integration with AOSP: dynamic database updates, status response