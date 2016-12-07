#!/usr/bin/perl

use strict;

# configure below here

my $world_width    = 30;       # number of cells wide to make the world
my $world_height   = 30;       # number of cells high to make the world
my $world_left     = -15000;   # x-coordinate of the left column of the world
my $world_bottom   = -15000;   # y-coordinate of the bottom row of the world
my $num_continents = 30;       # number of techtonic plates to create
my $min_size       = 3;        # minimum radius of a techtonic plate
my $max_size       = 10;       # maximum radius of a techtonic plate
my $percent_plain  = .70;      # percent of plates that are plain like
my $percent_mud    = .20;      # percent of plates that are muddy  (The rest are rocky)
my $cliff_height   = 500;      # Neighbor cells with this altitude difference have a cliff/wall interface
my $max_objects    = 5;        # maximum number of objects in a cell
my $object_chance_power = 10;  # higher numbers make objects more rare
my $object_file    = "objects_2007";  # name of the object file
my $output_file    = "world.txt"; # name of the output world file
my $seed           = time();   # random number seed
# configure above here

use Getopt::Std;
my %Options;

getopts("?w:h:l:b:n:m:M:p:u:c:o:p:f:F:s:", \%Options);

if($Options{'?'})
{
  print "$0
-w world_width    = 30;       # number of cells wide to make the world
-h world_height   = 30;       # number of cells high to make the world
-l world_left     = -15000;   # x-coordinate of the left column of the world
-b world_bottom   = -15000;   # y-coordinate of the bottom row of the world
-n num_continents = 30;       # number of techtonic plates to create
-m min_size       = 3;        # minimum radius of a techtonic plate
-M max_size       = 10;       # maximum radius of a techtonic plate
-p percent_plain  = .70;      # percent of plates that are plain like
-u percent_mud    = .20;      # percent of plates that are muddy  (The rest are rocky)
-c cliff_height   = 500;      # Neighbor cells with this altitude difference have a cliff/wall interface
-o max_objects    = 5;        # maximum number of objects in a cell
-p object_chance_power = 10;  # higher numbers make objects more rare
-f object_file    = objects_2007;  # name of the object file
-F output_file    = world.txt;# name of the output world file
-s seed           = time();   # random number seed
";
  exit 0;
}
if($Options{'w'}) { $world_width = $Options{'w'}; }
if($Options{'h'}) { $world_height = $Options{'h'}; }
if($Options{'l'}) { $world_left = $Options{'l'}; }
if($Options{'b'}) { $world_bottom = $Options{'b'}; }
if($Options{'n'}) { $num_continents = $Options{'n'}; }
if($Options{'m'}) { $min_size = $Options{'m'}; }
if($Options{'M'}) { $max_size = $Options{'M'}; }
if($Options{'p'}) { $percent_plain = $Options{'p'}; }
if($Options{'u'}) { $percent_mud = $Options{'u'}; }
if($Options{'c'}) { $cliff_height = $Options{'c'}; }
if($Options{'o'}) { $max_objects = $Options{'o'}; }
if($Options{'p'}) { $object_chance_power = $Options{'p'}; }
if($Options{'f'}) { $object_file = $Options{'f'}; }
if($Options{'F'}) { $output_file = $Options{'F'}; }
if($Options{'s'}) { $seed = $Options{'s'}; }


my @objects = &ReadObjects($object_file);

my @world = &MakeWorld($world_height, $world_width);
my $first_id = 1;
srand($seed);

foreach my $count (1..$num_continents)
{
  my $radius    = int (rand() * ($max_size - $min_size + 1)) + $min_size;
  my $elevation = (rand() * 2000) - 1000;
  my $variation = rand() * $cliff_height*9/10;
  my $left      = int(rand() * ($world_width - 2*$radius));
  my $bottom    = int(rand() * ($world_height - 2*$radius));
  my $iface     = rand();
  if($iface < $percent_plain)
    {
      $iface = 'plain';
    }
  elsif($iface < ($percent_plain + $percent_mud))
    {
      $iface = 'mud';
    }
  else
    {
      $iface = 'rocks';
    }
  my @c1 = &Continent($radius, $elevation, $variation, $iface, $first_id);
  $first_id += ( 2*$radius + 1)*( 2*$radius + 1);

  &AddContinent(\@world, \@c1, $left, $bottom, $world_left, $world_bottom);
}

&LinkCells(\@world);
&FindCliffs(\@world, $cliff_height);
&CheckWorld(\@world);
&PlaceObjects(\@world, \@objects, $max_objects, $object_chance_power);

&DisplayCells(\@world, $output_file);

sub ReadObjects
{
    my ($filename) = @_;
    my $fh;
    my @objects;
    my $line;
    
    if(-e $filename)
    {
	open($fh, "< $filename") or die "open: $filename: $!";
	$line = <$fh>; # throw away the header line
	while($line = <$fh>)
	{
	    chomp($line);
	    push(@objects, $line);
	}
	close($fh);
    }
    else
    {
	print "Proceeding without any objects.  Use -f object_file to include objects.\n";
    }
    return @objects;
}


sub PlaceObjects
{
  my ($cells, $objects, $max_objects, $chance_power) = @_;
  my $w       = @{$cells};
  my $h       = @{${$cells}[0]};
  my $x;
  my $y;
  my $obj_id = 1;

  if(@{$objects} <= 0) { return ; } # no objects
  for($x = 0; $x < $w; $x++)
    {
      for($y = 0; $y < $h; $y++)
        {
	    if($cells->[$x]->[$y])
	    {
		my $c1 = $cells->[$x]->[$y];
		my $cell_id = $c1->{id};
		my $chance = rand();
		$chance = $chance ** $chance_power;
		my $num_objects = $chance * $max_objects;
		foreach my $n (1..$num_objects)
		{
		    my $i = int(rand() * @{$objects});
		    my $obj = $objects->[$i];
		    $obj =~ s/OBJ[0-9]+\s+[0-9]+/OBJ${obj_id} ${cell_id}/;
		    $obj =~ s/\s+/ /g;
		    $obj_id ++;
		    push(@{$c1->{oems}}, $obj);
		}
            }
        }
    }
  
}

sub CheckWorld
{
  my ($cells) = @_;
  my $w       = @{$cells};
  my $h       = @{${$cells}[0]};
  my $x;
  my $y;

  for($x = 0; $x < $w; $x++)
    {
      for($y = 0; $y < $h; $y++)
        {
          my @d1 = qw/N S E W/;
          my @d2 = qw/S N W E/;
          my @xn = ($x, $x, $x+1, $x-1);
          my @yn = ($y+1, $y-1, $y, $y);
          my $i;
          for($i = 0; $i < @xn; $i++)
            {
              if($xn[$i] >= 0 && $xn[$i] < $w &&
                 $yn[$i] >= 0 && $yn[$i] < $h &&
                 $cells->[$x]->[$y] &&
                 $cells->[$xn[$i]]->[$yn[$i]])
                {
                  my $c1 = $cells->[$x]->[$y];
                  my $c2 = $cells->[$xn[$i]]->[$yn[$i]];
		  if($c1->{$d1[$i].'i'} != $c2->{$d2[$i].'i'})
		  {
		      printf STDERR ("ERROR %s %s\n",
				     $c1->{$d1[$i].'i'},
				     $c2->{$d2[$i].'i'}
				     );
		  }
                }
            }
        }
    }
  
}


sub DisplayCells
{
  my ($world, $filename) = @_;

  my $crow;
  my $cell;

  my $fh;


  open($fh, "> $filename") || die "open: $filename: $!";
  
  print $fh '
#          title damage energy traversable is_fall
Interface: plain      0      0           1       0
Interface: rocks      1      0           1       0
Interface: mud        0      1           1       0
Interface: wall      10      1           0       0
Interface: cliff      0      0           1       1
';

  my $base = 0;
  my $goal = 0;
  foreach $crow (@{$world})
    {
      foreach $cell (@{$crow})
        {
          if($cell)
            {
              if(($base == 0) ||
                 (rand() < .01))
                {
                  $base = $cell->{id};
                }
              if(($goal == 0) ||
                 (rand() < .001))
                {
                  $goal = $cell->{id};
                }
              printf $fh ("Cell: %4d %9.2f %9.2f %9.2f %4d %4d %4d %4d %6s %6s %6s %6s\n",
                          $cell->{id},
                          $cell->{x},
                          $cell->{y},
                          $cell->{z},
                          $cell->{N},
                          $cell->{S},
                          $cell->{E},
                          $cell->{W},
                          $cell->{Ni},
                          $cell->{Si},
                          $cell->{Ei},
                          $cell->{Wi},
                          );
	      foreach my $obj (@{$cell->{oems}})
                {
                  print $fh $obj, "\n";
                }
            }
        }
    }

  print $fh "Base: $base\n";
  print $fh "Goal: $goal\n";
  
  close($fh);
}


sub AddContinent
{
  my ($world, $continent, $x_orig, $y_orig, $world_left, $world_bottom) = @_;
  
  my $w       = @{$continent};
  my $h       = @{${$continent}[0]};
  my $x;
  my $y;

  for($x = 0; $x < $w; $x++)
    {
      for($y = 0; $y < $h; $y++)
        {
          my $wx = $x_orig + $x;
          my $wy = $y_orig + $y;
          if($continent->[$x]->[$y] != 0)
            {
              if($world->[$wx]->[$wy] != 0)
                { # merge cells
                  $world->[$wx]->[$wy]->{z} += $continent->[$x]->[$y]->{z};
                  $world->[$wx]->[$wy]->{z} /= 1.5;

                  my @d1 = qw/N S E W/;
                  foreach my $d (@d1)
                    {
                      if(rand() < .5)
                        {
                          my $key = $d.'i';
                          $world->[$wx]->[$wy]->{$key} = $continent->[$x]->[$y]->{$key};
                        }
                    }
                  if($world->[$wx]->[$wy]->{z} > 1000)
                    {
                      $world->[$wx]->[$wy]->{z} = 1000;
                    }
                }
              else
                { # set new cell
                  $world->[$wx]->[$wy] = $continent->[$x]->[$y];
                }
              if($world->[$wx]->[$wy] != 0)
                {
                  $world->[$wx]->[$wy]->{x} = $wx * 1000 + $world_left;
                  $world->[$wx]->[$wy]->{y} = $wy * 1000 + $world_bottom;
                }
            }
        }
    }
}

sub MakeWorld
{
  my ($w, $h) = @_;
  my @cells;

  my ($x, $y);
  
  for($x = 0; $x <= $w; $x ++)
    {
      $cells[$x] = [];
      
      for($y = 0; $y <= $h; $y ++)
        {
          $cells[$x]->[$y] = 0;
        }
    }

  return @cells;
}


sub FindCliffs
{
  my ($cells, $cliff_height) = @_;
  my $w       = @{$cells};
  my $h       = @{${$cells}[0]};
  my $x;
  my $y;

  for($x = 0; $x < $w; $x++)
    {
      for($y = 0; $y < $h; $y++)
        {
          my @d1 = qw/N S E W/;
          my @d2 = qw/S N W E/;
          my @xn = ($x, $x, $x+1, $x-1);
          my @yn = ($y+1, $y-1, $y, $y);
          my $i;
          for($i = 0; $i < @xn; $i++)
            {
              if($xn[$i] >= 0 && $xn[$i] < $w &&
                 $yn[$i] >= 0 && $yn[$i] < $h &&
                 $cells->[$x]->[$y] &&
                 $cells->[$xn[$i]]->[$yn[$i]])
                {
                  my $c1 = $cells->[$x]->[$y];
                  my $c2 = $cells->[$xn[$i]]->[$yn[$i]];
                  if(abs($c1->{z} - $c2->{z}) > $cliff_height)
                    {
                      if($c1->{z} > $c2->{z})
                        {
                          $c1->{$d1[$i].'i'} = 'cliff';
                          $c2->{$d2[$i].'i'} = 'wall';
                        }
                      else
                        {
                          $c1->{$d1[$i].'i'} = 'wall';
                          $c2->{$d2[$i].'i'} = 'cliff';
                        }
                    }
                }
              if(($xn[$i] < 0 || $xn[$i] >= $w ||
                  $yn[$i] < 0 || $yn[$i] >= $h ||
                  $cells->[$xn[$i]]->[$yn[$i]] == 0) &&
                 $cells->[$x]->[$y])
                { # exterior wall
                  my $c1 = $cells->[$x]->[$y];
                  $c1->{$d1[$i].'i'} = 'wall';
                }
            }
        }
    }
  
}
sub LinkCells
{
  my ($cells) = @_;
  my $w       = @{$cells};
  my $h       = @{${$cells}[0]};
  my $x;
  my $y;

  for($x = 0; $x < $w; $x++)
    {
      for($y = 0; $y < $h; $y++)
        {
          my @d1 = qw/N S E W/;
          my @d2 = qw/S N W E/;
          my @xn = ($x, $x, $x+1, $x-1);
          my @yn = ($y+1, $y-1, $y, $y);
          my $i;
          for($i = 0; $i < @xn; $i++)
            {
              if($xn[$i] >= 0 && $xn[$i] < $w &&
                 $yn[$i] >= 0 && $yn[$i] < $h &&
                 $cells->[$x]->[$y] &&
                 $cells->[$xn[$i]]->[$yn[$i]])
                {
                  my $c1 = $cells->[$x]->[$y];
                  my $c2 = $cells->[$xn[$i]]->[$yn[$i]];
                  $c1->{$d1[$i]} = $c2->{id};
                  $c2->{$d2[$i]} = $c1->{id};
		  
                  $c2->{$d2[$i].'i'} = $c1->{$d1[$i].'i'};
                }
            }
        }
    }
  
}

sub Continent
{
  my ($radius, $base, $hills, $iface, $cell_id) = @_;

  my ($x, $y, $z);
  my ($x_count, $y_count, $z);
  my @cells;
  
  $z = $base;
  
  for($x = -$radius, $x_count = 0; $x <= $radius; $x ++, $x_count ++)
    {
      $cells[$x_count] = [];
      
      for($y = -$radius, $y_count = 0; $y <= $radius; $y ++, $y_count ++)
        {
          my $r = sqrt($x*$x + $y*$y);
          my $d = $r/$radius;
          if(($d <= 1.) || (rand() > (2.0 - $d)))
            { # make a cell
              my $c = { "id" => $cell_id,
                        "x" => $x*1000,
                        "y" => $y*1000,
                        "z" => $z + rand() * $hills,
                        "N" => 0,
                        "S" => 0,
                        "E" => 0,
                        "W" => 0,
                        "Ni" => $iface,
                        "Si" => $iface,
                        "Ei" => $iface,
                        "Wi" => $iface,
			"oems" => [],
                      };
              $cells[$x_count]->[$y_count] = $c;
              $cell_id ++;
            }
          else
            {
              $cells[$x_count]->[$y_count] = 0;
            }
          
        }
    }
  return @cells;
}
