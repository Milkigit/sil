rsync -a --delete build/ prebuilt/
man -Tutf8 ./rb3ptr.3 | col -b > prebuilt/rb3ptr.3.txt
man -Tps ./rb3ptr.3 | col -b | ps2pdf - - > prebuilt/rb3ptr.3.pdf
