# In-circuit DRAM tester for 4164 and 4262 ICs

This is a basic RAM IC tester which writes both ones and zeroes to a 4x64 dram IC and reads back the results. 

It's written in C, without the Arduino library, which makes it much faster than Arduino based testers, 
which I found has a higher chance of finding marginal ICs.

I generally use this tester on an Arduino Pro Mini (16mhz / 5v), which I connect in circuit after making sure 
that all data and address ICs are not driven. For example, by powering a memory expansion board externally or by
keeping the computer continuously in reset.

It's very important to verify that all lines are inactive and are not being held low / high by other ICs.
If lines are being held low / high, it will at best give bad results and at worst damage other components!
