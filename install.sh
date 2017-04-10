#!/bin/sh

./.installreq.sh
./.buildurho.sh

git pull
cd ..
mkdir OGTatt-build
cd OGTatt-build
qmake ../OGTatt/OGTatt.pro
sudo make install
sudo chown -R $USER ~/.local/share/luckey/ogtatt/
sudo chown $USER ~/.local/share/icons/ogtatt.svg
update-icon-caches ~/.local/share/icons/
cd ..
rm -rf OGTatt-build
