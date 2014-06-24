# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

CFLAGS = -I$(TOPDIR)/include -D__KERNEL__ -O0 -g


CFLAGS += -Wall -Werror -fno-builtin

LIB 	:= $(obj)libencrypt.a

COBJS-y += fulanid.o
COBJS-y += ident.o
COBJS-y += cryptoe2.o
COBJS-y += des.o
COBJS-y += cm_io.o
COBJS-y += cm_burnfuze.o
COBJS-y += cm_data.o
COBJS-y += cm_gpa.o
COBJS-y += cm_i2c.o
COBJS-y += cm_i2cx.o
COBJS-y += cm_low.o
COBJS-y += cm_nopower.o
COBJS-y += cm_password.o
COBJS-y += cm_rdcfg.o
COBJS-y += cm_rdcksum.o
COBJS-y += cm_rdfuze.o
COBJS-y += cm_rduser.o
COBJS-y += cm_read.o
COBJS-y += cm_resetpsw.o
COBJS-y += cm_resetsec.o
COBJS-y += cm_rnd.o
COBJS-y += cm_secure.o
COBJS-y += cm_select.o
COBJS-y += cm_setuser.o
COBJS-y += cm_srduser.o
COBJS-y += cm_swruser.o
COBJS-y += cm_true.o
COBJS-y += cm_wrcfg.o
COBJS-y += cm_wrcksum.o
COBJS-y += cm_write.o
COBJS-y += cm_wruser.o

COBJS	:= $(COBJS-y)
SRCS 	:= $(COBJS:.o=.c)
OBJS 	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

