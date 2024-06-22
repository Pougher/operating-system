global klock

klock:
    cli
_lock_forever:
    hlt
    jmp _lock_forever
