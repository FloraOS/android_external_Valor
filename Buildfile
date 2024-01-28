target_valord(){
  info "NOTE: The gcc should be compitiable with target device architecture and should be able to access ANDROID_NDK"
  require_command gcc
  require_directory bin
  change_dir src/valord
  exec "gcc -Wall -Wpedantic -llog */*.c *.c -o../../bin/valord"
  leave_dir
  success "Succesfully built valord."
}

target_dbgen(){
  require_command gcc
  require_directory bin
  change_dir src/dbgen
  exec "gcc -Wall -Wpedantic crc32/*.c db/*.c *.c -o../../bin/dbgen"
  leave_dir
  success "Succesfully built dbgen."
}

target_install(){
  require_command adb
  exec adb root  
  exec adb shell mount -o remount,rw /
  exec adb shell rm -rf /system/bin/valord
  exec adb shell rm -rf /system/etc/init/valord.rc
  exec adb shell rm -rf /system/addon.d/50-valord.sh
  exec adb push bin/valord /system/bin
  exec adb push valord-custom.rc /system/etc/init/valord.rc
  exec adb push 50-valord.sh /system/addon.d/
  exec adb shell chmod 755 /system/addon.d/50-valord.sh
  success "Succesfully installed valord to the device."
  success "Please copy now valor.db to /system/etc/"
}

target_all(){
  target_dbgen
  target_valord
  target_install
}
