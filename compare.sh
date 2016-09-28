dd if=$1 bs=4096k | { dd bs=3600 count=1 of=/dev/null; dd bs=4096k of=bench.segy; }
dd if=$2 bs=4096k | { dd bs=3600 count=1 of=/dev/null; dd bs=4096k of=bench2.segy; }

size=$(stat --print="%s" bench.segy)
md5sum bench.segy

split -b $size bench2.segy splitout.
md5sum splitout.*

rm bench.segy bench2.segy splitout.*

md5sum $2
