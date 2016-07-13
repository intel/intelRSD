#!/usr/bin/perl -w
#
# Copyright (c) 2016, 2016 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

use strict;
use warnings qw(all);
use Carp;
use Getopt::Long;
use Pod::Usage;


=pod

=head1 NAME

dep.pl - Generate/update static dependencies in Makefile

=head1 SYNOPSIS

=over 10

=item B<dep.pl>

[B<--man>]
S<[B<--help> | B<-?>]>
[S<B<--include> I<include dir>>]

Parameters can be abbreviated to the smallest nonambigious spelling.

E.g.,

    $ dep.pl -h

=back

=cut


my  $help;
my  $man;
my  $verbose;
my  @include;


=pod

=head1 OPTIONS

=over 4

=item B<--man>

Display this man page

=item S<B<--help> | B<-?>>

Display an abbreviation of this man page

=item S<B<--include> I<directory>>

Include I<directory> when looking for include files that are to have dependency
targets created.  (Used when testing against base modules)

=back

=cut


my  $generated_msg = "### Generated targets ###";
my  @cfiles = qx(ls -1 *.c 2>/dev/null);
my  @hfiles = qx(ls -1 *.h 2>/dev/null);
my  %s_hfiles;
my  %s_cfiles;
my  @makefile;
my  $cmdline = "@ARGV";

# Process the command line, possibly dealing with help requests
GetOptions( 'include=s' => \@include,
            'verbose+' => \$verbose,
            'help|?' => \$help,
            'man' => \$man ) or

# Manage online help if needed
pod2usage( -exitstatus => 0, -verbose => 0 );
pod2usage( -exitstatus => 0, -verbose => 1 ) if $help;
pod2usage( -exitstatus => 0, -verbose => 2 ) if $man;


sub headers;

=pod

=head1 DESCRIPTION

B<dep.pl> is a simple script for refreshing static targets in the Makefile.  It
only need to be run once or occasionally to refresh the Makefile so that it
shows all of the dependencies.  This is slightly more sane then
checking/generating the dependencies every time Make is executed as it usually
leads to rebuilding everything (and missing the point of only updating those
files that change).  Moreover, it generates a static dependency tree for the
header files, so that C files are properly rebuilt if a header file changes.

=head1 RETURNS

=over 4

=item 0 -

Success

=item 1 -

error

=back

=head1 EXAMPLES

    $ dep.pl
    $ dep.pl --include ../..

=head1 BUGS

None written at this time.  I hope not to have plenty at a later date.

=cut

for (my $i = 0; $i < scalar @include; $i++) {
    $include[$i] =~ s#/+$##;
}

open(MAKEFILE, "<Makefile") || die "Unable to open Makefile for reading: $!\n";
@makefile = <MAKEFILE>;
close MAKEFILE;

open(MAKEFILE, ">Makefile") || die "Unable to open Makefile for writing: $!\n";
foreach my $line (@makefile) {
    last if $line =~ /$generated_msg/;
    print MAKEFILE $line;
}

print MAKEFILE $generated_msg, "\n";
print MAKEFILE "# Generated via $0", ($cmdline ? " $cmdline" : ""), "\n";
print MAKEFILE "# Object file nested dependencies\n";
foreach my $file (@cfiles) {
    my  $target;
    my  @deps;
    my  $depstr;

    chomp $file;
    $target = $file;
    $target =~ s/\.c$/\.o/;
    open(FILE, "<$file") || die "Unable to open $file: $!\n";

    foreach my $line (<FILE>) {
        my  $header;

        chomp $line;
        if (($header) = ($line =~ /#include\s+"(.*)"/)) {
            if (-e $header) {
                my  ($adir,$afile) = ($header =~ m!^(.*/)?([^/]*)$!);
                $adir ||= "";
                $adir =~ s#//#/#g;
                if ($header =~ /c$/) {
                    push @deps, $header;
                    $s_cfiles{$header}++;
                }
                else {
                    push @deps, ($header =~ /c$/ ? $header : "${adir}.dep/${afile}o");
                }
            }
            foreach my $dir (@include) {
                if (-e "$dir/$header") {
                    my  ($adir,$afile) = ($header =~ m!^(.*/)?([^/]*)$!);
                    $adir ||= "";
                    $adir = "$dir/$adir";
                    $adir =~ s#//#/#g;
                    push @deps, "${adir}.dep/${afile}o";
                    $s_hfiles{"$dir/$header"} = "${adir}.dep/${afile}o";
                    headers "$dir/$header";
                }
            }
        }
    }

    close FILE;
    $depstr = "$target :: $file @deps";
    $depstr =~ s/\s+$//;

    print MAKEFILE "$depstr\n";
}


foreach my $file (keys %s_cfiles) {
    my  $target;
    my  @deps;
    my  $depstr;

    chomp $file;
    $target = $file;
    open(FILE, "<$file") || die "Unable to open $file: $!\n";

    foreach my $line (<FILE>) {
        my  $header;

        chomp $line;
        if (($header) = ($line =~ /#include\s+"(.*)"/)) {
            if (-e $header) {
                my  ($adir,$afile) = ($header =~ m!^(.*/)?([^/]*)$!);
                $adir ||= "";
                $adir =~ s#//#/#g;
                push @deps, ($header =~ /c$/ ? $header : "${adir}.dep/${afile}o");
            }
            foreach my $dir (@include) {
                if (-e "$dir/$header") {
                    my  ($adir,$afile) = ($header =~ m!^(.*/)?([^/]*)$!);
                    $adir ||= "";
                    $adir = "$dir/$adir";
                    $adir =~ s#//#/#g;
                    push @deps, "${adir}.dep/${afile}o";
                    $s_hfiles{"$dir/$header"} = "${adir}.dep/${afile}o";
                    headers "$dir/$header";
                }
            }
        }
    }

    close FILE;
    if (scalar(@deps)) {
        $depstr = "$target :: @deps";
        $depstr =~ s/\s+$//;

        print MAKEFILE "$depstr\n";
    }
}


print MAKEFILE "\n";
print MAKEFILE "# Include file nested dependencies\n";
foreach my $file (@hfiles) {
    my  $target;
    my  @deps;
    my  $depstr;

    chomp $file;
    $target = ".dep/${file}o";
    open(FILE, "<$file") || die "Unable to open $file: $!\n";

    foreach my $line (<FILE>) {
        my  $header;

        chomp $line;
        if (($header) = ($line =~ /#include\s+"(.*)"/)) {
            if (-e $header) {
                my  ($adir,$afile) = ($header =~ m!^(.*/)?([^/]*)$!);
                $adir ||= "";
                $adir =~ s#//#/#g;
                push @deps, "${adir}.dep/${afile}o";
            }
            foreach my $dir (@include) {
                if (-e "$dir/$header") {
                    my  ($adir,$afile) = ($header =~ m!^(.*/)?([^/]*)$!);
                    $adir ||= "";
                    $adir = "$dir/$adir";
                    $adir =~ s#//#/#g;
                    push @deps, "${adir}.dep/${afile}o";
                    $s_hfiles{"$dir/$header"} = "${adir}.dep/${afile}o";
                    headers "$dir/$header";
                }
            }
        }
    }

    close FILE;
    $depstr = "$target :: $file @deps";
    $depstr =~ s/\s+$//;

    print MAKEFILE "$depstr\n";
}

foreach my $file (keys %s_hfiles) {
    my  $target;
    my  @deps;
    my  $depstr;
    my  $target_dir;

    chomp $file;
    $target = $s_hfiles{$file};
    open(FILE, "<$file") || die "Unable to open $file: $!\n";

    foreach my $line (<FILE>) {
        my  $header;

        chomp $line;
        if (($header) = ($line =~ /#include\s+"(.*)"/)) {
            foreach my $dir (@include) {
                if (-e "$dir/$header") {
                    my  ($adir,$afile) = ($header =~ m!^(.*/)?([^/]*)$!);
                    $adir ||= "";
                    $adir = "$dir/$adir";
                    $adir =~ s#//#/#g;
                    push @deps, "${adir}.dep/${afile}o";
                }
            }
        }
    }

    close FILE;
    $depstr = "$target :: $file @deps";
    $depstr =~ s/\s+$//;

    print MAKEFILE "\n$depstr\n";
    ($target_dir) = ($target =~ m!^(.*/)!);
    printf MAKEFILE "\t\@if [ ! -d $target_dir ]; then mkdir -p $target_dir; fi\n";
    printf MAKEFILE "\t\@touch \$\@\n";
}

close MAKEFILE;

sub headers {
    my  ($file) = @_;
    my  $target;
    my  @deps;
    my  $depstr;

    chomp $file;
    $target = ".dep/${file}o";
    open(FILE, "<$file") || die "Unable to open $file: $!\n";

    foreach my $line (<FILE>) {
        my  $header;

        chomp $line;
        if (($header) = ($line =~ /#include\s+"(.*)"/)) {
            foreach my $dir (@include) {
                if (-e "$dir/$header") {
                    my  ($adir,$afile) = ($header =~ m!^(.*/)?([^/]*)$!);
                    $adir ||= "";
                    $adir = "$dir/$adir";
                    $adir =~ s#//#/#g;
                    push @deps, "${adir}.dep/${afile}o";
                    $s_hfiles{"$dir/$header"} = "${adir}.dep/${afile}o";
                }
            }
        }
    }

    close FILE;
}
