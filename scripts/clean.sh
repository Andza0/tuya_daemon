if [ -d build ]; then
  rm -rf build
fi

if [ -e /etc/systemd/system/cdaemon.service ]; then
  sudo rm /etc/systemd/system/tuyad.service
fi

if [ -e /usr/local/bin/tuyad ]; then
  sudo rm /usr/local/bin/tuyad
fi
