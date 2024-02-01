droidbuild_module(){
  LOCAL_PATH=$(dirname $BASH_SOURCE)
  info "Updating valord SEPolicy"
  exec cp $LOCAL_PATH $BASEDIR/system/sepolicy/private
  success "Updating valord SEPolicy"
}