#include <unistd.h>
#include <linux/syscalls.h>
#include <linux/console.h>

static DEFINE_RAW_SPINLOCK(console_owner_lock);
static struct task_struct       *console_owner;
static bool                     console_waiter;
#ifdef CONFIG_LOCKDEP
static struct lockdep_map       console_owner_dep_map = {
    .name = "console_owner"
};
#endif /* CONFIG_LOCKDEP */

static void console_lock_spinning_enable(void)
{
    /* Lock, save the current thread as the console owner, and unlock */
    raw_spin_lock(&console_owner_lock);
    console_owner = current;
    raw_spin_unlock(&console_owner_lock);

    /* Take the console lock */
    spin_acquire(&console_owner_dep_map, 0, 0, _THIS_IP_);
}

static int console_lock_spinning_disable_and_check(void)
{
    int         waiter;

    /* Lock, atomically set the waiter, reset the console owner, and unlock */
    raw_spin_lock(&console_owner_lock);
    waiter = READ_ONCE(console_waiter);
    console_owner = NULL;
    raw_spin_unlock(&console_owner_lock);

    /* No waiters, release the spin lock */
    if (!waiter)
    {
        spin_release(&console_owner_dep_map, 1, _THIS_IP_);
        return 0;
    }

    /* The waiter is now free to continue */
    WRITE_ONCE(console_waiter, false);

    spin_release(&console_owner_dep_map, 1, _THIS_IP_);

    /*
     * Hand off console_lock to waiter. The waiter will perform
     * the up(). After this, the waiter is the console_lock owner.
     */
    mutex_release(&console_lock_dep_map, 1, _THIS_IP_);

    return 1;
}

static void write_to_console(const char *buf, size_t count)
{
    struct console *con;

    /* No console drives to speak of */
    if (!console_drivers)
        return;

    /* For each known console */
    for_each_console(con)
    {
        /* Console is not enabled */
        if (!(con->flags & CON_ENABLED))
            continue;

        /* Cannot write to this console */
        if (!con->write)
            continue;

        /* CPU is not ready */
        if (!cpu_online(smp_processor_id()) &&
                !(con->flags & CON_ANYTIME))
            continue;
        else
            con->write(con, buf, count);
    }
}

ssize_t write(int fd, const void *buf, size_t count)
{
    /* We don't support pipe writing in KSpace */
    if (fd == 0)
    {
        printk(KERN_WARNING "Attempt to write to fd 0!\n");
        return 0;
    }

    /**
     * Writing to stdout or stderr, that don't exist in KSpace,
     * so write them directly to the console.
     */
    if (fd == 1 || fd == 2)
    {
        /* Take the console lock, write and release the lock */
        console_lock_spinning_enable();
        write_to_console(buf, count);
        console_lock_spinning_disable_and_check();
    }

    /**
     * Little trick used to mock stdin, stdout and stderr for UKL
     * Don't make any sense in kernel space, since a fd could very well
     * be 0.
     */
    return ksys_write(fd - 3, buf, count);
}
