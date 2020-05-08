#!/bin/bash
# set -e

source config/common.sh

if [ -z $1 ]; then
  BUILD_PATH=./build-rt1051-Debug
  echo "copying from default: $BUILD_PATH"
  echo -e "usage: $0 <build-folder>\n"
else
  BUILD_PATH=$1
fi

check_target_rt1051 "$BUILD_PATH"

PURE_DEV=/dev/disk/by-id/usb-NXP_SEMI_NXP_MASS_STORAGE_0123456789ABCDEF-0:0
PURE_DISK=`readlink -f $PURE_DEV`
# check if already mouted

MOUNT_ENTRY=$(grep "$PURE_DISK" /etc/mtab)
if [ $? -ne 0 ]; then
	>&2 echo "PurePhone not mounted. Mount or retry with Ozone. https://github.com/muditacom/PurePhone/blob/master/doc/running_on_phone.md#eMMC_upload"
	exit 2
fi
PURE_PATH=$(echo $MOUNT_ENTRY | awk -F " " '{print $2}')
# unespace
PURE_PATH=$(echo -e $PURE_PATH)


echo "PurePhone remove all files"
rm "$PURE_PATH"/* -r &>/dev/null || echo "PurePhone disk alread empty"
rm -f $PURE_PATH/.boot.ini*
# sudo sync $PURE_DEV

echo "PurePhone copy build files"
cp -v $BUILD_PATH/boot.bin "$PURE_PATH"/  # | sed 's/'-\>'/'→'/g'

for file in $IMAGE_FILES; do
	echo Copying $file
	cp -vr $BUILD_PATH/$file "$PURE_PATH"/
done

# sudo sync $PURE_DEV # https://unix.stackexchange.com/a/345950
echo -e "PurePhone copied\n"

PURE_PARTITION=$(lsblk -nlp $PURE_DISK | tail +2 | awk '{print $1}')

if [ -z $PURE_PARTITION ]; then
       PURE_PARTITION=$PURE_DISK # it is formatted like so apparently
fi

# unmount the partition (sdX1), but eject the whole disk (sdX). then the PurePhone will detect it's been removed (=ejected)
if $(udisksctl unmount -b "$PURE_PARTITION" > /dev/null ) || $(umount "$PURE_PARTITION" > /dev/null ); then
	echo "PurePhone unmouted"
	timeout --signal=SIGINT 1 udisksctl power-off -b $PURE_DISK # timeout because there is known error, that this command hangs up.
	echo "PurePhone ejected"
	echo "Done. You can reset PurePhone now"
else
	>&2 echo "Eject manually"
fi
