#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>


unsigned long long int atou(char *num_str)
{
	size_t i = 0;
	unsigned long long int num = 0;

	while (num_str[i])
		num = num * 10 + num_str[i] - 48, i += 1;
	return (num);
}

/**
 * print_factor - prints two factors of num in the followgin format
 * n=p*q, where n is the number and p&q are factors
 * p&q are not necessarly primes
 * @num: the number whose factors are to be printed
 * @fs: File stream to print to
 *Return: the number of characters printed
 */
size_t print_factor(unsigned long long int num, FILE *fs)
{
	unsigned long long int i, ans;
	size_t printed = 0;

	for (i = 2; i < num; i++)
		if (num % i == 0)
		{
			ans = num / i;
			printed += fprintf(fs, "%llu=%llu*%llu\n", num, ans, i);
			return (printed);
		}
	/*incase the number is prime*/
	printed += fprintf(fs, "%llu=%llu*%llu\n", num, num, (unsigned long long int)1);
	return (printed);
}



/**
 *exit_fail-exits with EXIT_FAILURE after printing error
 */
void exit_fail(void)
{
	perror(NULL);
	exit(EXIT_FAILURE);
}
/**
 * reader - reads a text from a file descrriptor
 * until a new line is found and returns when found
 * deallocating memory is caller's responsibility
 * @fd: file descriptor
 * Return: returns a single line of string fromm the open file discreptor
 */
char *reader(int fd)
{
	char *line = malloc(15 * sizeof(char));
	int read_p = 0, stat, size = 15;
	char *_line = line;

	if (!line)
		exit_fail();
	while ((stat = read(fd, _line + read_p, 1)) != -1)
	{
		/*incase nothing is read, using  read_p==0 isn't a good idea in the case
		 * where the line iss exactly 10 chars long, after which read_p is set
		 * back to 0 but som char is read*/
		if (line == _line && stat == 0)
		{
			free(line);
			return (NULL);

		}
		if (_line[read_p] == '\n')
		{
			_line[read_p] = '\0';
			return (line);
		}

		read_p++;
		if (read_p == 15)
		{
			size += 15;
			line = realloc(line, size);
			_line = line + read_p;
			read_p = 0;
		}
	}
	return (line);
}

int main(int argc, char **argv)
{
	int fd, stat;
	unsigned long long int num = 0;
	size_t printed = 0;
	char *read_line = NULL;
	FILE *tmp_file = tmpfile();

	tmp_file = fopen("tmp_file.txt", "w+");

	if (argc != 2)
	{
		dprintf(2, "Usage: %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*if tmporary file couldn't be opened*/
	if (!tmp_file)
		exit_fail();

	/*open the file*/
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		exit_fail();

	while ((read_line = reader(fd)))
	{
		num = atou(read_line);
		printed += print_factor(num, tmp_file);
		free(read_line);
	}
	read_line = malloc(sizeof(char) * printed + 1);

	rewind(tmp_file);
	stat = fread(read_line, sizeof(char), printed, tmp_file);
	if (stat == -1)
		exit_fail();

	printf("%s", read_line);

	free(read_line);
	close(fd);
	exit(EXIT_SUCCESS);
}
