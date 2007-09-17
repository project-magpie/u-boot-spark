/*
 *  include/asm-st200/bitops.h
 *
 *  Copyright (C) 2002 STMicroelectronics Limited
 *	Author: Stuart Menefy <stuart.menefy@st.com>
 *
 */

#ifndef _ASM_ST200_BITOPS_H
#define _ASM_ST200_BITOPS_H

#include "unimplemented.h"

#include <asm/system.h>
#define save_and_cli(x)  do { local_save_flags(x); local_irq_disable(); } while (0)
#define restore_flags(x) local_irq_restore(x)

/*
 * All bit operations return 0 if the bit was cleared before the
 * operation and != 0 if it was not.
 *
 * bit 0 is the LSB of addr; bit 32 is the LSB of (addr+1).
 */

/**
 * set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This function is atomic and may not be reordered.  See __set_bit()
 * if you do not require the atomic guarantees.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static __inline__ void set_bit (int nr, volatile unsigned long *addr)
{
	int mask;
	volatile unsigned long *a = addr;
	unsigned long flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	save_and_cli (flags);
	*a |= mask;
	restore_flags (flags);
}

/**
 * __set_bit - Set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Unlike set_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static __inline__ void __set_bit (int nr, volatile unsigned long *addr)
{
	int mask;
	volatile unsigned long *a = addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	*a |= mask;
}

/**
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call smp_mb__before_clear_bit() and/or smp_mb__after_clear_bit()
 * in order to ensure changes are visible on other processors.
 */
static __inline__ void clear_bit (int nr, volatile unsigned long *addr)
{
	int mask;
	volatile unsigned long *a = addr;
	unsigned long flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	save_and_cli (flags);
	*a &= ~mask;
	restore_flags (flags);
}

static __inline__ void __clear_bit (int nr, volatile unsigned long *addr)
{
	int mask;
	volatile unsigned long *a = addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	*a &= ~mask;
}

#define smp_mb__before_clear_bit()	barrier()
#define smp_mb__after_clear_bit()	barrier()

/**
 * change_bit - Toggle a bit in memory
 * @nr: Bit to change
 * @addr: Address to start counting from
 *
 * change_bit() is atomic and may not be reordered.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static __inline__ void change_bit (int nr, volatile unsigned long *addr)
{
	int mask;
	volatile unsigned long *a = addr;
	unsigned long flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	save_and_cli (flags);
	*a ^= mask;
	restore_flags (flags);
}

/**
 * __change_bit - Toggle a bit in memory
 * @nr: the bit to change
 * @addr: the address to start counting from
 *
 * Unlike change_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static __inline__ void __change_bit (int nr, volatile unsigned long *addr)
{
	int mask;
	volatile unsigned long *a = addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	*a ^= mask;
}

/**
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static __inline__ int test_and_set_bit (int nr, volatile unsigned long *addr)
{
	int mask, retval;
	volatile unsigned long *a = addr;
	unsigned long flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	save_and_cli (flags);
	retval = (mask & *a) != 0;
	*a |= mask;
	restore_flags (flags);

	return retval;
}

/**
 * __test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static __inline__ int __test_and_set_bit (int nr,
					  volatile unsigned long *addr)
{
	int mask, retval;
	volatile unsigned long *a = addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *a) != 0;
	*a |= mask;

	return retval;
}

/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static __inline__ int test_and_clear_bit (int nr,
					  volatile unsigned long *addr)
{
	int mask, retval;
	volatile unsigned long *a = addr;
	unsigned long flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	save_and_cli (flags);
	retval = (mask & *a) != 0;
	*a &= ~mask;
	restore_flags (flags);

	return retval;
}

/**
 * __test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static __inline__ int __test_and_clear_bit (int nr,
					    volatile unsigned long *addr)
{
	int mask, retval;
	volatile unsigned long *a = addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *a) != 0;
	*a &= ~mask;

	return retval;
}

/**
 * test_and_change_bit - Change a bit and return its new value
 * @nr: Bit to change
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static __inline__ int test_and_change_bit (int nr,
					   volatile unsigned long *addr)
{
	int mask, retval;
	volatile unsigned long *a = addr;
	unsigned long flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	save_and_cli (flags);
	retval = (mask & *a) != 0;
	*a ^= mask;
	restore_flags (flags);

	return retval;
}

/* WARNING: non atomic and it can be reordered! */
static __inline__ int __test_and_change_bit (int nr,
					     volatile unsigned long *addr)
{
	int mask, retval;
	volatile unsigned long *a = addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *a) != 0;
	*a ^= mask;

	return retval;
}

/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static __inline__ int test_bit (int nr, const volatile void *addr)
{
	return 1UL & (((const volatile unsigned int *) addr)[nr >> 5] >>
		      (nr & 31));
}

/**
 * ffz - find first zero in word.
 * @word: The word to search
 *
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
static __inline__ unsigned long ffz (unsigned long word)
{
	int k;

	word = ~word;
	k = 31;
	if (word & 0x0000ffff) {
		k -= 16;
		word <<= 16;
	}
	if (word & 0x00ff0000) {
		k -= 8;
		word <<= 8;
	}
	if (word & 0x0f000000) {
		k -= 4;
		word <<= 4;
	}
	if (word & 0x30000000) {
		k -= 2;
		word <<= 2;
	}
	if (word & 0x40000000) {
		k -= 1;
	}
	return k;
}

/**
 * __ffs - find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
static __inline__ unsigned long __ffs (unsigned long word)
{
	int k;

	k = 31;
	if (word & 0x0000ffff) {
		k -= 16;
		word <<= 16;
	}
	if (word & 0x00ff0000) {
		k -= 8;
		word <<= 8;
	}
	if (word & 0x0f000000) {
		k -= 4;
		word <<= 4;
	}
	if (word & 0x30000000) {
		k -= 2;
		word <<= 2;
	}
	if (word & 0x40000000) {
		k -= 1;
	}
	return k;
}

#define fls(x) generic_fls(x)

/**
 * find_next_zero_bit - find the first zero bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The maximum size to search
 */
static __inline__ int find_next_zero_bit (unsigned long *addr, int size,
					  int offset)
{
	unsigned long *p = ((unsigned long *) addr) + (offset >> 5);
	unsigned long result = offset & ~31UL;
	unsigned long tmp;

	if (offset >= size)
		return size;
	size -= result;
	offset &= 31UL;
	if (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (32 - offset);
		if (size < 32)
			goto found_first;
		if (~tmp)
			goto found_middle;
		size -= 32;
		result += 32;
	}
	while (size & ~31UL) {
		if (~(tmp = *(p++)))
			goto found_middle;
		result += 32;
		size -= 32;
	}
	if (!size)
		return result;
	tmp = *p;

      found_first:
	tmp |= ~0UL << size;
      found_middle:
	return result + ffz (tmp);
}

/**
 * find_next_bit - find the first set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The maximum size to search
 */
static __inline__ int find_next_bit (unsigned long *addr, int size,
				     int offset)
{
	unsigned int *p = ((unsigned int *) addr) + (offset >> 5);
	unsigned int result = offset & ~31UL;
	unsigned int tmp;

	if (offset >= size)
		return size;
	size -= result;
	offset &= 31UL;
	if (offset) {
		tmp = *p++;
		tmp &= ~0UL << offset;
		if (size < 32)
			goto found_first;
		if (tmp)
			goto found_middle;
		size -= 32;
		result += 32;
	}
	while (size >= 32) {
		if ((tmp = *p++) != 0)
			goto found_middle;
		result += 32;
		size -= 32;
	}
	if (!size)
		return result;
	tmp = *p;

      found_first:
	tmp &= ~0UL >> (32 - size);
	if (tmp == 0UL)		/* Are any bits set? */
		return result + size;	/* Nope. */
      found_middle:
	return result + __ffs (tmp);
}

/**
 * find_first_zero_bit - find the first zero bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit-number of the first zero bit, not the number of the byte
 * containing a bit.
 */
static __inline__ int find_first_zero_bit (unsigned long *addr, unsigned size)
{
	return find_next_zero_bit (addr, size, 0);
}

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit-number of the first set bit, not the number of the byte
 * containing a bit.
 */
static __inline__ int find_first_bit (unsigned long *addr, unsigned size)
{
	return find_next_bit (addr, size, 0);
}

/*
 * Every architecture must define this function. It's the fastest
 * way of searching a 140-bit bitmap where the first 100 bits are
 * unlikely to be set. It's guaranteed that at least one of the 140
 * bits is cleared.
 */
/*
static inline int sched_find_first_bit(unsigned long *b)
{
	if (unlikely(b[0]))
		return __ffs(b[0]);
	if (unlikely(b[1]))
		return __ffs(b[1]) + 32;
	if (unlikely(b[2]))
		return __ffs(b[2]) + 64;
	if (b[3])
		return __ffs(b[3]) + 96;
	return __ffs(b[4]) + 128;
}
*/
/**
 * ffs - find first bit set
 * @x: the word to search
 *
 * This is defined the same way as
 * the libc and compiler builtin ffs routines, therefore
 * differs in spirit from the above ffz (man ffs).
 */

#define ffs(x) generic_ffs(x)

/**
 * hweightN - returns the hamming weight of a N-bit word
 * @x: the word to weigh
 *
 * The Hamming Weight of a number is the total number of bits set in it.
 */

#define hweight32(x) generic_hweight32(x)
#define hweight16(x) generic_hweight16(x)
#define hweight8(x) generic_hweight8(x)


#define ext2_set_bit(nr,addr) \
	__test_and_set_bit((nr),(unsigned long*)addr)
#define ext2_set_bit_atomic(lock,nr,addr) \
	test_and_set_bit((nr),(unsigned long*)addr)
#define ext2_clear_bit(nr, addr) \
	__test_and_clear_bit((nr),(unsigned long*)addr)
#define ext2_clear_bit_atomic(lock,nr, addr) \
		test_and_clear_bit((nr),(unsigned long*)addr)
#define ext2_test_bit(nr, addr)      test_bit((nr),(unsigned long*)addr)
#define ext2_find_first_zero_bit(addr, size) \
	find_first_zero_bit((unsigned long*)addr, size)
#define ext2_find_next_zero_bit(addr, size, off) \
	find_next_zero_bit((unsigned long*)addr, size, off)

/* Bitmap functions for the minix filesystem.  */
#define minix_test_and_set_bit(nr,addr) __test_and_set_bit(nr,(void*)addr)
#define minix_set_bit(nr,addr) __set_bit(nr,(void*)addr)
#define minix_test_and_clear_bit(nr,addr) __test_and_clear_bit(nr,(void*)addr)
#define minix_test_bit(nr,addr) test_bit(nr,(void*)addr)
#define minix_find_first_zero_bit(addr,size) \
	find_first_zero_bit((void*)addr,size)

#undef save_and_cli
#undef restore_flags

#endif /* _ASM_ST200_BITOPS_H */
