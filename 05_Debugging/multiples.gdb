set pagination off
set confirm off

break range_get if I->current % 5 == 0
commands
    silent
    printf "@@@ start=%d stop=%d step=%d current=%d\n", I->start, I->stop, I->step, I->current
    continue
end

run 1 12 > /dev/null
