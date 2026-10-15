set pagination off
set confirm off
set $index = 0

break range_get
commands
    silent
    set $index = $index + 1
    if $index >= 28 && $index <= 35
        printf "@@@ index=%d start=%d stop=%d step=%d current=%d\n", $index, I->start, I->stop, I->step, I->current
    end
    continue
end

run -100 100 3 > /dev/null
