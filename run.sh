#!/bin/bash
run() {
    printf "\ninput: %s\n" "$1"
    ./cps --encode < <(printf "%s" "$1") | tee out
    decoded="$(./cps --decode < out)"
    printf "\ndecoded: %s\n" "$decoded"
}
run "acekolswq" 
run "abcdefghijklmnopq" 
run "abcdefghijklmnopqrstuvwxyz" 
run "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" 
run "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
rm out 2>/dev/null
