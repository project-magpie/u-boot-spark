#ifndef	__CM_IO_H
#define	__CM_IO_H

#define	 ClockOut()	SET_PIO_PIN(PIO_PORT(3), 6, STPIO_OUT)
#define Clocklow(void)	STPIO_SET_PIN(PIO_PORT(3), 6, 0)
#define ClockHigh(void)	STPIO_SET_PIN(PIO_PORT(3), 6, 1)
#define DataOut(void)	SET_PIO_PIN(PIO_PORT(3), 7, STPIO_OUT)
#define DataIn(void)	SET_PIO_PIN(PIO_PORT(3), 7, STPIO_IN)
#define Datalow(void)	STPIO_SET_PIN(PIO_PORT(3), 7, 0)
#define DataHigh(void)	STPIO_SET_PIN(PIO_PORT(3), 7, 1)
#define DataRd(void)	STPIO_GET_PIN(PIO_PORT(3), 7)

#endif
