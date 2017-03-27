#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>


// defines boolean values
typedef enum {FALSE, TRUE} bool;

// creates type BUFFER of size 1 BYTE
typedef uint8_t BUFFER;

int main(int argc, char *argv[])
{
    clock_t begin = clock();
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover file.raw\n");
        return 1;
    }
    
    // get file name from command line
    char *infile = argv[1];
    
    // checks error
    FILE *card = fopen(infile, "r");
    if (card == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    //declares variable for name of image
    char image[8];
    FILE *img;
    
    //checks size of all files on memory card
    fseek(card, 0, SEEK_END);
    int card_size = ftell(card);
    rewind(card);

    bool first_jpg = FALSE; // tracks whether or not we have found our first jpg file
    int counter = 0; // counts jpg found for naming
    BUFFER b[512]; // allocates space to read buffer
    
    for(int i = 0; i * sizeof(b) < card_size; i++)
    {
        bool start_jpg = FALSE; // begins the loop assuming we have are not on the start of a jpeg
        fread(&b, 512, 1, card); // reads buffer

        // checks if we are at the start of a jpeg
        if (start_jpg == FALSE)
        {
            if (b[0] == 0xff &&
                b[1] == 0xd8 &&
                b[2] == 0xff &&
                (b[3] & 0xf0) == 0xe0)
                {
                start_jpg = TRUE;
                }
        }
        
        // if we are at the beginning of a jpeg
        if(start_jpg == TRUE)
        {
            // checks if we have not previously found our first jpeg
            if (first_jpg == FALSE)
            {
                first_jpg = TRUE;
                sprintf(image, "%03i.jpg", counter);
                img = fopen(image, "w");
                fwrite(&b, 512, 1, img);
                counter++;
            }
            
            // closes current jpeg if we have already found one
            else
            {
                fclose(img);
                sprintf(image, "%03i.jpg", counter);
                img = fopen(image, "w");
                fwrite(&b, 512, 1, img);
                counter++;
            }
        }
        // writes buffer if we are in the middle of a jpeg
        if (start_jpg == FALSE && first_jpg == TRUE)
        {
            fwrite(&b, 512, 1, img);
        }
        // continues through main loop if we are still looking for the first jpeg
        if (start_jpg == FALSE && first_jpg == FALSE)
        {
            i++;
        }
        
    }
    // closes image
    fclose(img);
    clock_t end = clock();
    double time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
    printf("%f", time_spent);
    return 0;
}
