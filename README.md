# Compilation with Open Embedded/Yocto Toolchain

The u-boot source for spark7162 can be compiled in this way with the yocto toolchain:

```bash
$ source /opt/poky/1.5.1/environment-setup-sh4-poky-linux
# For  more details on this please read:
# [http://www.denx.de/wiki/view/ELDK-5/FrequentlyAskedQuestionsAndAnswers#Compiling_U_Boot_or_Linux_fails](http://www.denx.de/wiki/view/ELDK-5/FrequentlyAskedQuestionsAndAnswers#Compiling_U_Boot_or_Linux_fails)
$ unset LDFLAGS
$ export CROSS_COMPILE=sh4-poky-linux-
$ make distclean
$ make pdk7105se_config
```

Due to the fact that there is no support for the "-m4-nofpu" on the yocto GCC a patch is needed:

```diff
diff --git a/cpu/sh/config.mk b/cpu/sh/config.mk
index b16c9b5..b94c5e7 100644
--- a/cpu/sh/config.mk
+++ b/cpu/sh/config.mk
@@ -26,7 +26,7 @@

 ### old:  PLATFORM_CPPFLAGS += -m4 -m4-nofpu -U__sh3__
 ### want: PLATFORM_CPPFLAGS += -m4 -mno-implicit-fp
-PLATFORM_CPPFLAGS += -m4-nofpu -U__sh3__ -D__SH4__=1
+PLATFORM_CPPFLAGS += -U__sh3__ -D__SH4__=1

 # all SH (ST40) CPUs will use the same linker script file
 LDSCRIPT := $(SRCTREE)/cpu/$(CPU)/u-boot.lds
```

After appling the patch you can compile the u-boot:
```bash
$ make
```

For testing your new u-boot I recommend my [JTAG adapter](https://github.com/project-magpie/jtag-adapter).
