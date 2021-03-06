
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


/****************************************  IMPORTANT NOTE  **********************************

    Comments in this file that start with / * ! or / / ! are being used by Doxygen to
    document the software.  Dashes in these comment blocks are used to create bullet lists.
    The lack of blank lines after a block of dash preceeded comments means that the next
    block of dash preceeded comments is a new, indented bullet list.  I've tried to keep the
    Doxygen formatting to a minimum but there are some other items (like <br> and <pre>)
    that need to be left alone.  If you see a comment that starts with / * ! or / / ! and
    there is something that looks a bit weird it is probably due to some arcane Doxygen
    syntax.  Be very careful modifying blocks of Doxygen comments.

*****************************************  IMPORTANT NOTE  **********************************/



#ifndef PFMLOADER_H
#define PFMLOADER_H

#include "pfmLoaderDef.hpp"
#include "pfmFilter.hpp"
#include "version.hpp"


class pfmLoader : public QObject
{
  Q_OBJECT


public:

  pfmLoader (int32_t argc = 0, char **argv = NULL);
  ~pfmLoader ();


protected:

  int32_t          pfm_file_count, input_file_count;

  uint8_t          cube_available, csf_attr;

  char             cube_name[50], out_str[2048];

  PFM_DEFINITION   pfm_def[MAX_LOAD_FILES], ref_def;

  PFM_GLOBAL       pfm_global;

  FILE_DEFINITION  *input_file_def;

  WKT              wkt_data;

  FILE             *hfp;

  FLAGS            flags;

  char             fname[512];

  QStringList      input_files;


  void usage ();


protected slots:

};

#endif
