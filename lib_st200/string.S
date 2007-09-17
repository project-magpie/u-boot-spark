/*
 * arch/st200/lib/string.S
 *
 * Copyright (C) 2002 STMicroelectronics Limited
 *	Author: Thierry Strudel <thierry.strudel@st.com>
 *
 */

#include "linkage.h"

/* void * memcpy(void * dest,const void *src,size_t count)
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to ($r16)
 * @src: Where to copy from ($r17)
 * @count: The size of the area. ($r18)
 *
 */


ENTRY(memcpy)
	cmpgt	$b0 = $r18, 3
	mov	$r19 = $r16 /* test if count >= 4 and copy dest  */
	;;
	brf	$b0, L?_2_memcpy
	;;
L?_1_memcpy:
	cmpgt	$b0 = $r18, 7
	ldb	$r20 = 0[$r17]
	add	$r17 = $r17, 1	/* test is checked after the 4 byte copy (must be >=8)*/
	;;
	ldb	$r21 = 0[$r17]
	add	$r17 = $r17, 1
	;;
	ldb	$r22 = 0[$r17]
	add	$r17 = $r17, 1
	;;
	ldb	$r23 = 0[$r17]
	add	$r17 = $r17, 1
	;;
	stb	0[$r19] = $r20
	add	$r19 = $r19, 1
	;;
	stb	0[$r19] = $r21
	add	$r19 = $r19, 1
	;;
	stb	0[$r19] = $r22
	add	$r19 = $r19, 1
	;;
	br	$b0, L?_1_memcpy
	stb	0[$r19] = $r23
	add	$r19 = $r19, 1
	add	$r18 = $r18, -4	/* if (count > 4) we do it again */
	;;

L?_2_memcpy:
	cmpeq	$b0 = $r18, 0
	;;
	br	$b0, L?_3_memcpy
	;;
	ldb	$r20 = 0[$r17]
	add	$r17 = $r17, 1
	;;
	add	$r18 = $r18, -1
	;;
	stb	0[$r19] = $r20
	add	$r19 = $r19, 1
	;;
	goto L?_2_memcpy	/* we have to loop */
	;;

L?_3_memcpy:
	goto	$r63
	;;

/**
 * memset - Fill a region of memory with the given value
 * @s: Pointer to the start of the area. ($r16)
 * @c: The byte to fill the area with ($r17)
 * @count: The size of the area. ($r18)
 *
 * Do not use memset() to access IO space, use memset_io() instead.
 */
ENTRY(memset)
	/*
	 * We try and make the common case (word aligned, big blocks) the
	 * direct path through this code, hence the odd ordering.
	 */
	cmpeq	$b2 = $r18, $r0		/* $b2 = count==0 */
	and	$r20 = $r16, 3
	and	$r17 = $r17, 0xff
	;;
	cmpeq	$b0 = $r20, $r0		/* $b0 = word aligned */
	shl	$r21 = $r17, 8
	shl	$r22 = $r17, 16
	shl	$r23 = $r17, 24
	;;
	cmpltu	$b3 = $r18, 16		/* $b3 = count<16 */
	mov	$r19 = $r16
	cmpltu	$b1 = $r18, 4		/* $b1 = count<4  */
	;;
	br	$b2, 99f		/* Finished */
	or	$r17 = $r17, $r21
	or	$r22 = $r22, $r23
	;;
	brf	$b0, 10f		/* Misaligned */
	or	$r17 = $r17, $r22
	;;
	br	$b3, 40f		/* Aligned but count<16 */
	;;

	/* Big loop */
30:	stw	0[$r19] = $r17
	cmpltu	$b0 = $r18, 16+16	/* $b0 = count<16 at end of loop */
	cmpltu	$b1 = $r18, 4+16		/* $b1 = count<4 at end of loop */
	cmpeq	$b2 = $r18, 16		/* $b2 = count==0 at end of loop */
	;;
	stw	4[$r19] = $r17
	add	$r18 = $r18, -16
	;;
	stw	8[$r19] = $r17
	;;
	stw	12[$r19] = $r17
	add	$r19 = $r19, 16
	brf	$b0, 30b
	;;

	/* Less than 16 bytes remaining. See if we are finished. */
	brf	$b2, 40f		/* not finished */
	;;
	goto	$r63
	;;

	/* Start not word aligned, byte at a time copy until aligned */
10:	stb	0[$r19] = $r17
	cmpeq	$b0 = $r20, 3		/* $b0 = will be aligned at end of loop */
	cmpltu	$b1 = $r18, 4+1		/* $b1 = count<4 at end of loop */
	cmpeq	$b2 = $r18, 1		/* $b2 = count==0 at end of loop */
	;;
	cmpltu	$b3 = $r18, 16+1		/* $b3 = count<16 at end of loop */
	;;
	add	$r19 = $r19, 1
	add	$r18 = $r18, -1
	;;
	brf	$b0, 10b
	and	$r20 = $r19, 3
	;;

	/* Now word aligned. */
	/* See if we're finished. */
	br	$b2, 99f
	;;
	/* At least 1 byte remaining. If more then 15, use the big loop. */
	brf	$b3, 30b
	;;
	/* Between 1 and 15 bytes remaining. If 4 or more copy word at a time. */
40:	br	$b1, 50f		/* finish byte at a time */
	;;

	/* Finish word at a time */
41:	stw	0[$r19] = $r17
	cmpltu	$b1 = $r18, 4+4		/* $b1 = count<4 at end of loop */
	;;
	cmpeq	$b2 = $r18, 4		/* $b2 = count==0 at end of loop */
	;;
	add	$r19 = $r19, 4
	add	$r18 = $r18, -4
	;;
	brf	$b1, 41b
	;;

	/* Less than 4 bytes remaining. See if we are finished. */
	br	$b2, 99f		/* Finished */
	;;

	/* Finish byte at a time */
50:	stb	0[$r19] = $r17
	cmpeq	$b1 = $r18, 1		/* $b1 = count==0 at end of loop */
	;;
	add	$r19 = $r19, 1
	;;
	add	$r18 = $r18, -1
	;;
	brf	$b1, 50b
	;;

99:	goto	$r63
	;;
