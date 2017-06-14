rsync -a --delete build/ prebuilt/
man -Tps man3/rb3ptr.3 | col -b | ps2pdf - - > prebuilt/rb3ptr.3.pdf
