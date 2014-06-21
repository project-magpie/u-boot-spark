# Compilation with Yocto/OE Toolchain

The u-boot source can be compiled in this way with the yocto toolchain:

```bash
$ source /opt/poky/1.5.1/environment-setup-sh4-poky-linux
# For  more details on this please read:
# http://www.denx.de/wiki/view/ELDK-5/FrequentlyAskedQuestionsAndAnswers#Compiling_U_Boot_or_Linux_fails
$ unset LDFLAGS
$ export CROSS_COMPILE=sh4-poky-linux-
$ make clean
$ make ARCH=st40 mb618_config
```

Due to the fact that there is no support for the "-m4-nofpu" on the yocto GCC a patch is needed:

```diff
diff --git a/arch/sh/cpu/sh4/config.mk b/arch/sh/cpu/sh4/config.mk
index b3feb2a..a6e3649 100644
--- a/arch/sh/cpu/sh4/config.mk
+++ b/arch/sh/cpu/sh4/config.mk
@@ -24,5 +24,5 @@
 # MA 02111-1307 USA
 #
 #
-PLATFORM_CPPFLAGS += -m4-nofpu
+#PLATFORM_CPPFLAGS += -m4-nofpu
 PLATFORM_RELFLAGS += -ffixed-r13
diff --git a/arch/st40/cpu/config.mk b/arch/st40/cpu/config.mk
index c780793..a6f99a7 100644
--- a/arch/st40/cpu/config.mk
+++ b/arch/st40/cpu/config.mk
@@ -26,7 +26,7 @@

 ### old:  PLATFORM_CPPFLAGS += -m4 -m4-nofpu -U__sh3__
 ### want: PLATFORM_CPPFLAGS += -m4 -mno-implicit-fp
-PLATFORM_CPPFLAGS += -m4-nofpu -U__sh3__ -D__SH4__=1
+PLATFORM_CPPFLAGS += -U__sh3__ -D__SH4__=1

 # all SH (ST40) CPUs will use the same linker script file
 LDSCRIPT := $(SRCTREE)/$(CPUDIR)/u-boot.lds
```

After appling the patch you can compile the u-boot:
```bash
$ make ARCH=st40
```

For testing your new u-boot I recommend my [JTAG adapter](https://github.com/project-magpie/jtag-adapter).