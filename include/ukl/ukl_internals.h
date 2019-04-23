#ifndef UKL_INTERNALS_H
#define UKL_INTERNALS_H

#define UKL_BAD_FD(fn, d)                                                      \
	printk(KERN_ERR "Trying to call " fn                                   \
			"() with bad file descriptor: %d",                     \
	       d)

#endif /* UKL_INTERNALS_H */
