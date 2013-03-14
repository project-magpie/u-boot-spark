#ifndef __ARCH_ST40_INCLUDE_ASM_LINKAGE_H
#define __ARCH_ST40_INCLUDE_ASM_LINKAGE_H

#define SYMBOL_NAME(name) name
#define SYMBOL_NAME_LABEL(name) name ## :

#define ALIGN .balign 4

#define ENTRY(name) \
  .globl SYMBOL_NAME(name); \
  ALIGN; \
  SYMBOL_NAME_LABEL(name)

#endif /* __ARCH_ST40_INCLUDE_ASM_LINKAGE_H */
