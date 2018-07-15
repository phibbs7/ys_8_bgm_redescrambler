/*
 * A de / re scrambler for Ys VIII: Lacrimosa of DANA's BGM "protection".
 *
 * Last revision: 14/7/2018
 *
 * Note: Only works on one file at a time. Needs a shell script for doing an entire directory at once.
 * Something like this should work for bash on linux:
 * 	for i in *.ogg; do echo "Descrambling $i..."; ys_8_bgm_redescrambler d $i $i.descrambled.ogg; done;
 *
 * Something like this for Windows: (with %1 and %OUTPUT_DIR% being directory names.)
 *  FOR /F "tokens=1 delims= " %%G IN ('DIR /B %1') DO ECHO "Processing file: %1%%G" && ys_8_bgm_redescrambler d %1%%G %OUTPUT_DIR%%%G.descramble.ogg
 *
 * Shout outs to Kaldaien on Steam Community for (re)posting the reversed eng'd descrambling scheme.
 * Original descrambling post: https://steamcommunity.com/app/579180/discussions/0/3288067088114993308/?ctp=2#c1694920442952076795
 */

 /* Check for Visual C. */
#ifdef _MSC_VER
#include <stdint.h>
#endif /* _MSC_VER */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ERROR_SUCCESS 0
#define ERROR_UNKNOWN -1
#define ERROR_INVALID_ARGUMENT -2
#define ERROR_MEMORY_ALLOCATION -3
#define ERROR_IO -4
#define ERROR_FILE_LENGTH_UNKNOWN -5
#define ERROR_FILE_INACCESSABLE -6

int encrypt(char * pData, const size_t pData_length) {
		/* Init vars. */
		int ret = ERROR_UNKNOWN;
#ifdef __GNUC__
		__uint8_t *data = NULL;
#else
		uint8_t *data = NULL;
#endif /* __GNUC__ */
		size_t i = 0;

		/* Check arguments. */
		if ((pData != NULL) && (pData_length > 0)) {
				/* Cast pointer type. */
#ifdef __GNUC__
				data = (__uint8_t *)pData;
#else
				data = (uint8_t *)pData;
#endif /* __GNUC__ */
				/* "Encrypt" the data. */
				for (i = 0; i < pData_length; i++)
				{
					data [i] = (((data [i] >> 4U) & 0xF) | ((~(data [i]) & 0xF) << 4U));
// Lower bits missing			data [i] = (~(data [i]) & 0xF) << 4U;
// Upper bits missing			data [i] = ((data [i] >> 4U) & 0xF);

// Original Decryption                       data [i] = ( (data [i] << 4U) | ~(data [i] >> 4U) & 0xF ) & 0xFF;
				}

				/* Done. */
				ret = ERROR_SUCCESS;
		} else {
				/* Invalid arguments. */
				ret = ERROR_INVALID_ARGUMENT;
		}

		/* Exit function. */
		return ret;
}

int decrypt(char * pData, const size_t pData_length) {
	/* Init vars. */
	int ret = ERROR_UNKNOWN;
#ifdef __GNUC__
	__uint8_t *data = NULL;
#else
	uint8_t *data = NULL;
#endif /* __GNUC__ */
	size_t i = 0;

	/* Check arguments. */
	if ((pData != NULL) && (pData_length > 0)) {
		/* Cast pointer type. */
#ifdef __GNUC__
		data = (__uint8_t *)pData;
#else
		data = (uint8_t *)pData;
#endif /* __GNUC__ */
		/* "Decrypt" the data. */
		for (i = 0; i < pData_length; i++)
		{
			data [i] = ( (data [i] << 4U) |	~(data [i] >> 4U) & 0xF ) & 0xFF;
		}

		/* Done. */
		ret = ERROR_SUCCESS;
	} else {
		/* Invalid arguments. */
		ret = ERROR_INVALID_ARGUMENT;
	}

	/* Exit function. */
	return ret;
}

int read_file(const char * filename, const size_t filename_length, char ** buffer, size_t * buffer_length) {
	/* Init vars. */
	int ret = ERROR_UNKNOWN;
	char * buffer_temp = NULL;
	size_t flength = 0;
	size_t bytes_read = 0;
	FILE * finput = NULL;

	/* Check vars. */
	if ((filename != NULL) && (filename_length > 0) &&
		(buffer != NULL) && (buffer_length != NULL)) {

		/* Open file. */
		finput = fopen(filename, "rb");
		if (finput != NULL) {
			/* Determine size. */
			fseek(finput, 0, SEEK_END);
			flength = ftell(finput);
			if (flength > 0) {
				/* Seek back to beginning. */
				fseek(finput, 0, SEEK_SET);

				/* Allocate memory block. */
				buffer_temp = (char *)malloc(flength);
				if (buffer_temp != NULL) {
					/* Clear memory. */
					memset(buffer_temp, '\0', flength);

					/* Read entire file. */
					bytes_read = fread ((void *)buffer_temp, 1, flength, finput);
					if (bytes_read == flength) {
						/* Copy the pointer and length. */
						*buffer = buffer_temp;
						*buffer_length = flength;

						/* Set success. */
						ret = ERROR_SUCCESS;
					} else {
						/* IO ERROR */
						ret = ERROR_IO;

						/* Clear Memory. */
						memset(buffer_temp, '\0', flength);
					}
				} else {
					/* Memory allocation error. */
					ret = ERROR_MEMORY_ALLOCATION;
				}
			} else {
				/* Could not get file length. */
				ret = ERROR_FILE_LENGTH_UNKNOWN;
			}

			/* Close file. */
			fclose (finput);
		} else {
			/* Could not open file. */
			ret = ERROR_FILE_INACCESSABLE;
		}
	} else {
		/* Invalid argument. */
		ret = ERROR_INVALID_ARGUMENT;
	}

	/* Clean up if needed. */
	if (ret != ERROR_SUCCESS) {
		if (buffer_temp != NULL) {
			free(buffer_temp);
			buffer_temp = NULL;
		}
	}

	/* Exit function. */
	return ret;
}

int write_file(const char * filename, const size_t filename_length, const char * buffer, const size_t buffer_length) {
	/* Init vars. */
	int ret = ERROR_UNKNOWN;
	size_t bytes_written = 0;
	FILE * foutput = NULL;

	/* Check vars. */
	if ((filename != NULL) && (filename_length > 0) &&
				(buffer != NULL) && (buffer_length > 0)) {
		/* Open output file for writing. */
		foutput = fopen(filename, "wb");
		if (foutput != NULL) {
			bytes_written = fwrite((const void *)buffer, 1, buffer_length, foutput);
			if (bytes_written == buffer_length) {
				/* Done. */
				ret = ERROR_SUCCESS;
			} else {
				/* IO ERROR. */
				ret = ERROR_IO;
			}

			/* Close file. */
			fclose(foutput);
		} else {
			/* Could not open file. */
			ret = ERROR_FILE_INACCESSABLE;
		}
	} else {
				/* Invalid argument. */
				ret = ERROR_INVALID_ARGUMENT;
		}

	/* Exit function. */
	return ret;
}

int print_help() {
	/* Print help info. */
	printf("%s", "Usage: <operation> <input_filename> <output_filename>\n");
	printf("%s", "Where <operation> is one of the following:\n\n");
	printf("%s", "\td - decrypt input file\n\n");
	printf("%s", "\te - encrypt input file\n\n");
	printf("%s", "Note: Input and output filenames must be different.\n");

	/* Exit function. */
	return ERROR_SUCCESS;
}

int main(int argc, char *argv[]) {
	/* Init vars. */
	char op = '\0';
	int retFromCall = ERROR_UNKNOWN;
	char * buffer = NULL;
	const char * input_filename = NULL;
	const char * output_filename = NULL;
	size_t buffer_length = 0;
	size_t input_filename_length = 0;
	size_t output_filename_length = 0;

	/* Check args. */
	if (argc == 4) {
		/* Get operation. */
		op = argv[1][0];
		switch (op) {
			case 'd':
				/* Get filename args. */
				input_filename = argv[2];
				input_filename_length = strlen(input_filename);
				output_filename = argv[3];
				output_filename_length = strlen(output_filename);

				/* Check input and output filename lengths. */
				if ((input_filename_length > 0) && (output_filename_length > 0)) {
					/* Check for input and output filenames match. */
					if ((input_filename_length != output_filename_length) || (memcmp(input_filename, output_filename, input_filename_length) != 0)) {
						/* Call read file. */
						retFromCall = read_file(input_filename, input_filename_length, &buffer, &buffer_length);
						if ((retFromCall == ERROR_SUCCESS) && (buffer != NULL) && (buffer_length > 0)) {
							/* Call decrypt. */
							retFromCall = decrypt(buffer, buffer_length);
							if (retFromCall == ERROR_SUCCESS) {
								/* Call write file. */
								retFromCall = write_file(output_filename, output_filename_length, buffer, buffer_length);
								if (retFromCall == ERROR_SUCCESS) {
									/* File "decrypted" successfully. */
									printf("%s -> %s \"DECRYPTED\" OK\n", input_filename, output_filename);
								} else {
									/* Error from write file. */
									printf("Error writing file: %i\n", retFromCall);
								}
							} else {
								/* Error from decrypt. */
								printf("Error decrypting data: %i\n", retFromCall);
							}
						} else {
							/* Error from read_file. */
							printf("Error reading file: %i\n", retFromCall);
						}

						/* Deallocate buffer if needed. */
						if (buffer != NULL) {
							free(buffer);
							buffer = NULL;
						}
					} else {
						/* Input filename and output filenames are identical. */
						retFromCall = ERROR_INVALID_ARGUMENT;
						printf("%s", "Input and output filenames are identical, will not overwrite input file, aborting.\n");
						print_help();
					}
				} else {
					/* Empty filename. */
					retFromCall = ERROR_INVALID_ARGUMENT;
					printf("%s", "No filename(s) given, aborting.\n");
					print_help();
				}
				break;

			case 'e':
				/* Get filename args. */
								input_filename = argv[2];
				input_filename_length = strlen(input_filename);
								output_filename = argv[3];
								output_filename_length = strlen(output_filename);

								/* Check input and output filename lengths. */
								if ((input_filename_length > 0) && (output_filename_length > 0)) {
										/* Check for input and output filenames match. */
										if ((input_filename_length != output_filename_length) || (memcmp(input_filename, output_filename, input_filename_length) != 0)) {
												/* Call read file. */
												retFromCall = read_file(input_filename, input_filename_length, &buffer, &buffer_length);
												if ((retFromCall == ERROR_SUCCESS) && (buffer != NULL) && (buffer_length > 0)) {
							/* Call encrypt. */
														retFromCall = encrypt(buffer, buffer_length);
														if (retFromCall == ERROR_SUCCESS) {
																/* Call write file. */
																retFromCall = write_file(output_filename, output_filename_length, buffer, buffer_length);
																if (retFromCall == ERROR_SUCCESS) {
																		/* File "encrypted" successfully. */
																		printf("%s -> %s \"ENCRYPTED\" OK\n", input_filename, output_filename);
																} else {
																		/* Error from write file. */
																		printf("Error writing file: %i\n", retFromCall);
																}
														} else {
																/* Error from decrypt. */
																printf("Error decrypting data: %i\n", retFromCall);
														}
												} else {
														/* Error from read_file. */
														printf("Error reading file: %i\n", retFromCall);
												}

												/* Deallocate buffer if needed. */
												if (buffer != NULL) {
														free(buffer);
														buffer = NULL;
												}
										} else {
												/* Input filename and output filenames are identical. */
												retFromCall = ERROR_INVALID_ARGUMENT;
												printf("%s", "Input and output filenames are identical, will not overwrite input file, aborting.\n");
												print_help();
										}
								} else {
										/* Empty filename. */
										retFromCall = ERROR_INVALID_ARGUMENT;
										printf("%s", "No filename(s) given, aborting.\n");
										print_help();
								}
								break;
			default:
				/* Invalid operation. */
				retFromCall = ERROR_INVALID_ARGUMENT;
				printf("%s", "Invalid operation given, aborting.\n");
				print_help();
				break;
		}
	} else {
		/* No args, or improper args given. */
		retFromCall = ERROR_INVALID_ARGUMENT;
		printf("%s", "No arguments / Incorrect number of arguments  given, aborting.\n");
		print_help();
	}

	/* Exit program. */
	return retFromCall;
}
