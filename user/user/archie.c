#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
 	printf(1, "Number of processes running in the system: %d\n", getprocs());
	exit();
}
