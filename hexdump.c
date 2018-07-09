#include "hexdump.h"

void show_help_messages(void)
{
	printf("HexDump trebuie folosit in modul urmator:\n");
	printf("hd.exe <file> <offset>\n");
	printf("unde:\n");
	printf("\thd.exe este numele programului\n");
	printf("\tfile este numele fisierului\n");
	printf("\toffset este numarul de bytes pentru fiecare rand. Offsetul trebuie sa fie <= %d\n", OFFSET_MAX_VALUE);
	printf("Exemplu: hd.exe a.txt 10");
}

// Functia va returna dimensiunea in bytes a unui fisier
// Functia este definita ca fiind statica deoarece este folosita doar in acest fisier
static int64_t get_file_size(FILE *fp)
{
	fseek(fp, 0, SEEK_END); // merge la ultimul byte
	int64_t fs = ftell(fp); // retinem ce byte este acesta
	fseek(fp, 0, SEEK_SET); // mergem la inceput
	return fs;
}

void show_header(int offset)
{
	// header-ul va avea 3 componente: 
	//  a) offset = sectiunea unde afisam offsetul curent
	//  b) raw = sectiunea unde afisam byte-ul curent in hexazecimal
	//  c) text = caracterului corespunzator byte-ului curent sau '.'

	// De asemenea, atunci cand afisam aceste lucruri trebuie sa tinem cont
	// si de offset.
	printf("OFFSET");

	// La sectiunea raw vom incerca sa aliniem textul la mijloc
	// In formula sa tinut cont ca fiecare byte afisat in aceasta sectiune
	// are 2 caractere + ' '
	int raw_size = 3 * offset;
	int raw_size_half = raw_size / 2;
	for(int i = 0; i < raw_size_half - 1; i++)
	{
		printf(" ");
	}
	printf("RAW");
	for(int i = raw_size_half + 1; i < raw_size; i++)
	{
		printf(" ");
	}

	// la fel si la sectiunea text
	int offset_half = offset / 2;
	for(int i = 0; i < offset_half - 2; i++)
	{
		printf(" ");
	}
	printf("TEXT");
	for(int i = offset_half + 2; i < offset; i++)
	{
		printf(" ");
	}
	printf("\n");

	// De asemenea, vom afisa si '-' pentru a desparti header-ul
	// de restul output-ului
	for(int i = 0; i < OFFSET_WIDTH; i++)
	{
		printf("-");
	}
	printf(" ");

	for(int i = 0; i < raw_size; i++)
	{
		printf("-");
	}
	printf(" ");
	for(int i = 0; i < offset; i++)
	{
		printf("-");
	}
	printf("\n");
}

void dump(char *path, int offset)
{
	// Incercam sa deschidem fisierul
	FILE *file = fopen(path, "r");
	if(file != NULL)
	{
		// Afisam header-ul.
		show_header(offset);

		// In variabila current_byte vom retine la de la al catelea
		// byte se face citirea pe un rand
		int64_t current_byte = 0;

		int64_t fs = get_file_size(file);

		// De asemenea, vom avea nevoie si de un vector de bytes(chars)
		// in care sa retinem valorile citite pentru a putea fi afisate
		// mai tarziu

		// NOTA: Trebuie sa folosim 'unsigned char' deoarece tipul 'char' este signed si daca
		//       un byte citit este mai mare decat > 127 atunci va face un overflow si vom printa
		//       chestii de genul: "FFFFFA" in loc de "C5" de exemplu
		unsigned char buffer[offset];

		// Parcurgem fisierul in block-uri de 'offset' bytes
		while(!feof(file) && !ferror(file) && current_byte < fs)
		{
			printf("%0*lld: ", OFFSET_WIDTH, current_byte);
			fread(buffer, sizeof(char), offset, file);

			// afisam datele citite din fisier in forma hexazecimala(hex)
			for(int i = 0; i < offset; i++)
			{
				printf("%02X ", buffer[i]);
			}

			// afisam ceea ce am citit din fisier(partea text)
			for(int i = 0; i < offset; i++)
			{
				// daca nu e un caracter "printable" atunci afiseaza un "."
				if(!isprint(buffer[i]))
				{
					printf(".");
				}
				else
				{
					printf("%c", buffer[i]);
				}
			}
			printf("\n");
			current_byte += offset;
		}

		fclose(file);
	}
	else
	{
		printf("EROARE: Nu s-a putut deschide fisierul <%s>\n", path);
	}
}

void handle_user(char *argv[], int argc)
{
	// Trebuie sa tinem cont de faptul ca primul argument din
	// argv va fi numele programului insusi
	if(argc != 3)
	{
		show_help_messages();
		exit(EXIT_FAILURE);
	}
	// Trebuie sa verificam ca user-ul nu a introdus altceva, adica:
	// o litera in loc de o valoare numerica pentru offset etc.
	
	// Astfel, vom verifica daca offset-ul este corect si i-l si
	// convertim(offsetul) in int
	int is_offset_good = 1;
	int offset = 0;
	char *current_letter = argv[2];
	for(; *current_letter != '\0' && is_offset_good; current_letter++)
	{
		if(!isdigit(*current_letter))
		{
			is_offset_good = 0;
		}
		else
		{
			offset = offset * 10 + (*current_letter - '0');
		}
	}

	if(!is_offset_good)
	{
		printf("offset-ul trebuie sa fie o valoare numerica\n");
		exit(EXIT_FAILURE);
	}
	
	// De asemenea, trebuie sa verificam ca offsetul este mai mic decat o valoare prestabilita
	if(offset > OFFSET_MAX_VALUE)
	{
		printf("offset-ul [%d] este prea mare.\n", offset);
		exit(EXIT_FAILURE);
	}

	dump(argv[1], offset);
}
