# Counts and outputs number of positive lines in a file.
@ARGV > 0 or die "Need a file.";
$pos=0;
while(<>){
	chomp;
	if($_ > 0){
		$pos++
	}
}
print $pos;