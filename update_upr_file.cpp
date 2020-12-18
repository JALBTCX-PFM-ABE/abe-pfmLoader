
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.

*********************************************************************************************/

#include "pfmLoaderDef.hpp"

uint8_t update_upr_file (QString parameter_file, FILE_DEFINITION *input_file_def, int32_t input_file_count)
{
  char file[1024];
  strcpy (file, parameter_file.toLatin1 ());
  FILE *fp;

  if ((fp = fopen (file, "r+")) == NULL)
    {
      perror (file);
      fflush (stderr);
      return (NVFalse);
    }


  char string[1024];
  int32_t pos = 0;
  QString qstring;


  while (fgets (string, sizeof (string), fp) != NULL)
    {
      qstring = string;


      //  Look for the end of the .upr input files section.

      if (qstring.contains ("End Update Parameter File Input Files"))
        {
          //  Move the file pointer to just before the end sentinel.

          fseek (fp, pos, SEEK_SET);


          //  Now add all of the new files to the list.

          for (int32_t i = 0 ; i < input_file_count ; i++)
            {
              if (input_file_def[i].status)
                {
                  strcpy (file, input_file_def[i].name.toLatin1 ());

                  fprintf (fp, "%s\n", file);
                }
            }

          fprintf (fp, "**********  End Update Parameter File Input Files  **********\n");
        }

      pos = ftell (fp);
    }

  fclose (fp);

  return (NVTrue);
}
