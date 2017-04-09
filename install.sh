#!/bin/sh

./.installreq.sh
./.buildurho.sh

git pull
qmake OGTatt.pro
sudo make install
sudo chown -R $USER ~/.local/share/luckey/ogtatt/
sudo chown $USER ~/.local/share/icons/ogtatt.svg
update-icon-caches ~/.local/share/icons/
