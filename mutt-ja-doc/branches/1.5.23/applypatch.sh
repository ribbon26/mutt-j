!# /bin/sh
PATCHDIR=../trunk/patches
for i in $PATCHDIR/patch-* ; do \
  patch <$i ;
done;
