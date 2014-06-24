release_dir=release

if [ -d $release_dir ]; then
	rm $release_dir -rf
fi

mkdir $release_dir

source distcleanmake_overclock_on.sh
cp u-boot release/u-boot.overclock
cp u-boot.bin release/u-boot.bin.overclock
cp u-boot.bak.bin release/u-boot.bak.bin.overclock

source distcleanmake_overclock_off.sh
cp u-boot release/
cp u-boot.bin release/
cp u-boot.bak.bin release/

mv release/u-boot.bin.overclock release/u-boot.overclock.bin
mv release/u-boot.bak.bin.overclock release/u-boot.overclock.bak.bin

describe=`git describe --dirty --always --tags`
touch $release_dir/$describe

git glog --oneline > release/history.txt
touch release/spark7167
