    #Delete files ending with .model, .data, .predn and .train
	use Cwd;
	my $dir = $ARGV[0] ? $ARGV[0] : getcwd;
    opendir(DIR, $dir) or die $!;
    while (my $file = readdir(DIR)) {
        # We only want files
        next unless (-f "$dir/$file");
        next unless ($file =~ m/\.model$/) or ($file =~ m/\.data$/) or ($file =~ m/\.predn$/) or ($file =~ m/\.train$/);
        unlink $file;
    }
    closedir(DIR);
    exit 0;