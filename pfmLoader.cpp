
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



#include "pfmLoader.hpp"
#include "version.hpp"


/***************************************************************************\
*                                                                           *
*   Module Name:        pfmLoader                                           *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       September 21, 2010                                  *
*                                                                           *
*   Purpose:            Command line version of pfmLoad Qt Wizard.  This    *
*                       allows you to read and process a pfmLoad parameter  *
*                       file (*.prm or .upr) without having to use the      *
*                       pfmLoad GUI.                                        *
*                                                                           *
\***************************************************************************/

int32_t main (int32_t argc, char **argv)
{
  //  Starting an application just so I can use the LAS_WKT QDialog if needed.

  QApplication a (argc, argv);

  QTranslator translator;
  QString translatorName = QString ("pfmLoader_") + QLocale::system ().name ();
  //QString translatorName = "pfmLoader_de_DE.UTF-8";
  translator.load (translatorName, ":/translations");
  a.installTranslator (&translator);

  new pfmLoader (argc, argv);
}


void pfmLoader::usage ()
{
  strcpy (out_str, pfmLoader::tr ("\nUsage: pfmLoader PFM_LOAD_PARAMETERS_FILE\n\n").toUtf8 ());
  fprintf (stderr, "%s", out_str);
  fflush (stderr);
  exit (-1);
}


#ifdef NVLinux


static int32_t substitute_cnt;
static char substitute_path[10][3][1024];


static void sub_in ()
{
  char        varin[1024], info[1024];
  FILE        *fp;


  substitute_cnt = 0;


  //  If the startup file was found...
    
  if ((fp = find_startup (".pfm_cfg")) != NULL)
    {
      //  Read each entry.
        
      while (ngets (varin, sizeof (varin), fp) != NULL)
        {
	  if (varin[0] != '#')
	    {
	      //  Check input for matching strings and load values if found.
            
	      if (strstr (varin, "[SUBSTITUTE PATH]") != NULL && substitute_cnt < 10)
		{
		  //  Put everything to the right of the equals sign in 'info'.
            
		  get_string (varin, info);


		  /*  Throw out malformed substitute paths.  */

		  if (strchr (info, ','))
		    {
		      /*  Check for more than 1 UNIX substitute path.  */

		      if (strchr (info, ',') == strrchr (info, ','))
			{
			  strcpy (substitute_path[substitute_cnt][0], strtok (info, ","));
			  strcpy (substitute_path[substitute_cnt][1], strtok (NULL, ","));
			  substitute_path[substitute_cnt][2][0] = 0;
			}
		      else
			{
			  strcpy (substitute_path[substitute_cnt][0], strtok (info, ","));
			  strcpy (substitute_path[substitute_cnt][1], strtok (NULL, ","));
			  strcpy (substitute_path[substitute_cnt][2], strtok (NULL, ","));
			}

		      substitute_cnt++;
		    }
		}
	    }
	}

      fclose (fp);
    }
}


static void inv_sub (char *path)
{
  char           string[1024];
  int16_t        i;
  uint8_t        hit;


  for (i = 0 ; i < substitute_cnt ; i++)
    {
      //  Set the hit flag so we can break out if we do the substitution.

      hit = NVFalse;


      for (int32_t k = 1 ; k < 3 ; k++)
	{
	  //  Make sure that we had two UNIX substitutes, otherwise it will substitute for a blank string
	  //  (this only works when k is 2).

	  if (substitute_path[i][k][0] && strstr (path, substitute_path[i][k]))
	    {
	      strcpy (string, substitute_path[i][0]);

	      int32_t j = strlen (substitute_path[i][k]);
	      strcat (string, &path[j]);
	      strcpy (path, string);

	      for (int32_t j = 0 ; j < (int32_t) strlen (path) ; j++)
		{
		  if (path[j] == '/') path[j] = '\\';
		}

	      hit = NVTrue;

	      break;
	    }
	}


      //  Break out if we did a substitution.

      if (hit) break;
    }
}

#endif



pfmLoader::pfmLoader (int32_t argc, char **argv)
{
  int32_t percent = 0, old_percent = -1;
  uint8_t laz_warning = NVFalse;


  uint8_t readParameterFile (QString parameter_file, QStringList *input_files, PFM_DEFINITION *pfm_def, PFM_GLOBAL *pfm_global, FLAGS *flags,
                             uint8_t *csf_attr);
  uint8_t writeParameterFile (QString parameter_file, PFM_GLOBAL pfm_global, FLAGS flags);
  int32_t get_file_type (char *);
  uint8_t update_upr_file (QString parameter_file, FILE_DEFINITION *input_file_def, int32_t input_file_count);


  //  Check for command line parameter file name argument.

  if (argc < 2) usage ();


  /*  Override the HDF5 version check so that we can read BAGs created with an older version of HDF5.  */

  putenv ((char *) "HDF5_DISABLE_VERSION_CHECK=2");


  // Set defaults so that if keys don't exist the parameters are defined

  pfm_global.horizontal_error = 1.5;
  pfm_global.vertical_error = 0.25;


  csf_attr = NVFalse;


  wkt_data.input_wkt = NVFalse;


  pfm_global.appending = NVFalse;
  pfm_global.attribute_count = 0;


  pfm_global.max_files = NINT (pow (2.0, (double) PFM_FILE_BITS)) - 1;
  pfm_global.max_lines = NINT (pow (2.0, (double) PFM_LINE_BITS)) - 1;
  pfm_global.max_pings = NINT (pow (2.0, (double) PFM_PING_BITS)) - 1;
  pfm_global.max_beams = NINT (pow (2.0, (double) PFM_BEAM_BITS)) - 1;

  pfm_global.cache_mem = 400000000;


  /************************************************** IMPORTANT NOTE *************************************************\

      If you need to add to, subtract from, or change the available attributes, the functions are in the utility
      library (Libraries/utility) in attributes.cpp and attributes.hpp.

  \************************************************** IMPORTANT NOTE *************************************************/

  for (int32_t i = 0 ; i < GSF_ATTRIBUTES ; i++) pfm_global.gsf_attribute_num[i] = 0;
  for (int32_t i = 0 ; i < HOF_ATTRIBUTES ; i++) pfm_global.hof_attribute_num[i] = 0;
  for (int32_t i = 0 ; i < TOF_ATTRIBUTES ; i++) pfm_global.tof_attribute_num[i] = 0;
  for (int32_t i = 0 ; i < CZMIL_ATTRIBUTES ; i++) pfm_global.czmil_attribute_num[i] = 0;
  for (int32_t i = 0 ; i < LAS_ATTRIBUTES ; i++) pfm_global.las_attribute_num[i] = 0;
  for (int32_t i = 0 ; i < BAG_ATTRIBUTES ; i++) pfm_global.bag_attribute_num[i] = 0;


  flags.old = NVFalse;
  flags.hof = NVFalse;
  flags.sub = NVFalse;
  flags.lnd = NVFalse;
  flags.tof = NVFalse;
  flags.lid = NVFalse;
  flags.nom = NVFalse;
  flags.sec = NVFalse;
  flags.srtmb = NVFalse;
  flags.srtm1 = NVFalse;
  flags.srtm3 = NVFalse;
  flags.srtm30 = NVFalse;
  flags.srtmr = NVFalse;
  flags.srtme = NVTrue;
  flags.attr = NVFalse;
  flags.czmil = NVFalse;
  flags.czmil_land = NVTrue;
  flags.czmil_water = NVTrue;
  flags.czmil_water_surface = NVTrue;
  flags.ref_water_surface = NVTrue;
  flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_1] = NVTrue;
  flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_2] = NVTrue;
  flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_3] = NVTrue;
  flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_4] = NVTrue;
  flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_5] = NVTrue;
  flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_6] = NVTrue;
  flags.czmil_chan[CZMIL_SHALLOW_CHANNEL_7] = NVTrue;
  flags.czmil_chan[CZMIL_IR_CHANNEL] = NVFalse;
  flags.czmil_chan[CZMIL_DEEP_CHANNEL] = NVTrue;


  ref_def.mbin_size = 2.0;
  ref_def.gbin_size = 0.0;
  ref_def.min_depth = -500.0;
  ref_def.max_depth = 1000.0;
  ref_def.precision = 0.01;
  ref_def.apply_area_filter = NVFalse;
  ref_def.deep_filter_only = NVTrue;
  ref_def.cellstd = 2.4;
  ref_def.radius = 20.0;


  //  Set the timezone to GMT, we'll use this later.

#ifdef NVWIN3X
 #ifdef __MINGW64__
  putenv((char *) "TZ=GMT");
  tzset();
 #else
  _putenv((char *) "TZ=GMT");
  _tzset();
#endif

  strcpy (cube_name, "cube_pfm.exe");
#else
  putenv((char *) "TZ=GMT");
  tzset();

  strcpy (cube_name, "cube_pfm");
#endif


  cube_available = NVFalse;
  if (find_startup_name (cube_name) != NULL) cube_available = NVTrue;


  setTimeAttributes (&pfm_global.time_attribute);
  setGSFAttributes (pfm_global.gsf_attribute);
  setHOFAttributes (pfm_global.hof_attribute);
  setTOFAttributes (pfm_global.tof_attribute);
  setCZMILAttributes (pfm_global.czmil_attribute, pfm_global.czmil_flag_name);
  setLASAttributes (pfm_global.las_attribute);
  setBAGAttributes (pfm_global.bag_attribute);

  countTimeAttributes (pfm_global.time_attribute_num, &pfm_global.attribute_count);
  countGSFAttributes (pfm_global.gsf_attribute_num, &pfm_global.attribute_count);
  countHOFAttributes (pfm_global.hof_attribute_num, &pfm_global.attribute_count);
  countTOFAttributes (pfm_global.tof_attribute_num, &pfm_global.attribute_count);
  countCZMILAttributes (pfm_global.czmil_attribute_num, &pfm_global.attribute_count);
  countLASAttributes (pfm_global.las_attribute_num, &pfm_global.attribute_count);
  countBAGAttributes (pfm_global.bag_attribute_num, &pfm_global.attribute_count);


  for (int32_t i = 0 ; i < MAX_LOAD_FILES ; i++)
    {
      pfm_def[i].name = "";
      pfm_def[i].polygon_count = 0;
      pfm_def[i].index = i;
      pfm_def[i].mbin_size = ref_def.mbin_size;
      pfm_def[i].gbin_size = ref_def.gbin_size;
      pfm_def[i].min_depth = ref_def.min_depth;
      pfm_def[i].max_depth = ref_def.max_depth;
      pfm_def[i].precision = ref_def.precision;
      pfm_def[i].mosaic = "NONE";
      pfm_def[i].feature = "NONE";
      pfm_def[i].apply_area_filter = ref_def.apply_area_filter;
      pfm_def[i].deep_filter_only = ref_def.deep_filter_only;
      pfm_def[i].cellstd = ref_def.cellstd;
      pfm_def[i].radius = ref_def.radius;
      pfm_def[i].existing = NVFalse;
      pfm_def[i].rebuild_flag = NVFalse;
      memset (&pfm_def[i].open_args, 0, sizeof (PFM_OPEN_ARGS));
    }


  input_files.clear ();

  pfm_file_count = 0;
  QString parameter_file = QString (argv[1]);
  if (!readParameterFile (parameter_file, &input_files, pfm_def, &pfm_global, &flags, &csf_attr)) usage ();

  set_cache_size_max (pfm_global.cache_mem);


  QStringList sort_files;

  for (int32_t i = 0 ; i < input_files.size () ; i++) sort_files += input_files.at (i);


  //  Sort so we can remove dupes.

  sort_files.sort ();


  //  Remove dupes and place into input_files.

  QString name, prev_name = "";
  input_files.clear ();

  for (int32_t i = 0 ; i < sort_files.size () ; i++)
    {
      name = sort_files.at (i);

      if (name != prev_name)
        {
          input_files.append (name);
          prev_name = name;
        }
    }


  char string[1024];
  struct stat fstat;


  /*  Code to check your input file list.  Only for debugging.

  for (int32_t i = 0 ; i < input_files.size () ; i++)
    {
      strcpy (string, input_files.at (i).toLatin1 ());
      fprintf (stderr, "%s\n", string);
      fflush (stderr);
    }
  exit (-1);

  */


#ifdef NVLinux
  if (flags.sub) sub_in ();
#endif


  input_file_count = input_files.size ();

  input_file_def = new FILE_DEFINITION[input_file_count];


  //  Pre-check the input files.  If we have a problem we want out now.

  for (int32_t j = 0 ; j < input_file_count ; j++)
    {
      input_file_def[j].name = input_files.at (j);


      //  Check for LAZ files.

      if (input_file_def[j].name.endsWith (".laz")) laz_warning = NVTrue;


      char name[1024], short_name[512];
      strcpy (name, input_file_def[j].name.toLatin1 ());
      strcpy (short_name, QFileInfo (input_file_def[j].name).fileName ().toLatin1 ());


      input_file_def[j].type = get_file_type (name);

      input_file_def[j].status = NVTrue;


      //  If the input parameter file was a .upr file we want to automatically exclude any files in the 
      //  **  Update Parameter File Input Files  ** section.  There is no point in checking them since
      //  they have already been beaten against the PFM files.

      if (pfm_global.upr_flag)
        {
          for (int32_t k = 0 ; k < pfm_global.upr_input_files.size () ; k++)
            {
              if (input_file_def[j].name == pfm_global.upr_input_files.at (k)) input_file_def[j].status = NVFalse;
            }
        }

      switch (input_file_def[j].type)
        {
          //  File that has been added from a PFM list and was marked as "deleted".

        case -1:
          input_file_def[j].status = NVFalse;
          break;


          //  Unable to open the file.

        case -2:
          strcpy (out_str, pfmLoader::tr ("Unable to open file %1\nReason %2\n").arg (name).arg (strerror (errno)).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          input_file_def[j].status = NVFalse;
          break;

        case PFM_UNDEFINED_DATA: 
          strcpy (out_str, pfmLoader::tr ("Unable to determine file type for %1\n").arg (name).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          input_file_def[j].status = NVFalse;
          break;

        case PFM_GSF_DATA:
          if (input_file_def[j].status) input_file_def[j].status = check_gsf_file (name, &wkt_data);
          break;

        case PFM_CHARTS_HOF_DATA:
          flags.lid = NVTrue;
          if (input_file_def[j].status) input_file_def[j].status = check_hof_file (name);
          break;

        case PFM_SHOALS_TOF_DATA:
          if (input_file_def[j].status) input_file_def[j].status = check_tof_file (name);
          break;

        case PFM_NAVO_LLZ_DATA:
          if (input_file_def[j].status) input_file_def[j].status = check_llz_file (name);
          break;

        case PFM_CZMIL_DATA:
          flags.czmil = NVTrue;
          if (input_file_def[j].status) input_file_def[j].status = check_czmil_file (name, csf_attr, &wkt_data);
          break;

        case PFM_LAS_DATA:
          if (input_file_def[j].status) input_file_def[j].status = check_las_file (name);
          break;

        case PFM_BAG_DATA:
          if (input_file_def[j].status) input_file_def[j].status = check_bag_file (name);
          break;

        case PFM_DTED_DATA:
          if (input_file_def[j].status) input_file_def[j].status = check_dted_file (name);
          break;

        case PFM_CHRTR_DATA:
          if (input_file_def[j].status) input_file_def[j].status = check_chrtr_file (name);
          break;
        }

      strcpy (out_str, pfmLoader::tr ("Checking file %1 of %2 - %3                                   \n").arg (j + 1).arg (input_file_count).arg (short_name).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
    }


  strcpy (out_str,
          pfmLoader::tr ("Preliminary file check complete                                                                                                \n\n").toUtf8 ());
  fprintf (stderr, "%s", out_str);
  fflush (stderr);


  //  Get the file count

  pfm_file_count = 0;
  for (int32_t i = 0 ; i < MAX_LOAD_FILES ; i++) 
    {
      if (pfm_def[i].name == "")
        {
          pfm_file_count = i;
          break;
        }
    }


  //  Check for pre-existing index file attributes.

  for (int32_t i = 0 ; i < pfm_file_count ; i++)
    {
      //  First, just check to see if it exists.

      FILE *fp;
      char fname[1024];

      strcpy (fname, pfm_def[i].name.toLatin1 ());


      //  Try to open the file.

      pfm_def[i].existing = NVFalse;
      if ((fp = fopen (fname, "r")) != NULL)
        {
          fclose (fp);
          pfm_def[i].existing = NVTrue;

          pfm_global.appending = NVTrue;
          pfm_global.attribute_count = 0;

          int32_t temp_handle;
          PFM_OPEN_ARGS open_args;

          strcpy (open_args.list_path, pfm_def[i].name.toLatin1 ());

          open_args.checkpoint = 0;

          if ((temp_handle = open_existing_pfm_file (&open_args)) >= 0)
            {
              for (int32_t j = 0 ; j < open_args.head.num_ndx_attr ; j++)
                {
                  if (QString (open_args.head.ndx_attr_name[j]).contains (pfm_global.time_attribute.name))
                    {
                      pfm_global.attribute_count++;
                      pfm_global.time_attribute_num = pfm_global.attribute_count;
                    }

                  for (int32_t k = 0 ; k < GSF_ATTRIBUTES ; k++)
                    {
                      if (QString (open_args.head.ndx_attr_name[j]).contains (pfm_global.gsf_attribute[k].name))
                        {
                          pfm_global.attribute_count++;
                          pfm_global.gsf_attribute_num[k] = pfm_global.attribute_count;
                          break;
                        }
                    }

                  for (int32_t k = 0 ; k < HOF_ATTRIBUTES ; k++)
                    {
                      if (QString (open_args.head.ndx_attr_name[j]).contains (pfm_global.hof_attribute[k].name))
                        {
                          pfm_global.attribute_count++;
                          pfm_global.hof_attribute_num[k] = pfm_global.attribute_count;
                          break;
                        }
                    }

                  for (int32_t k = 0 ; k < TOF_ATTRIBUTES ; k++)
                    {
                      if (QString (open_args.head.ndx_attr_name[j]).contains (pfm_global.tof_attribute[k].name))
                        {
                          pfm_global.attribute_count++;
                          pfm_global.tof_attribute_num[k] = pfm_global.attribute_count;
                          break;
                        }
                    }

                  for (int32_t k = 0 ; k < CZMIL_ATTRIBUTES ; k++)
                    {
                      if (QString (open_args.head.ndx_attr_name[j]).contains (pfm_global.czmil_attribute[k].name))
                        {
                          pfm_global.attribute_count++;
                          pfm_global.czmil_attribute_num[k] = pfm_global.attribute_count;


                          //  Test for CSF attributes.

                          if (i > CZMIL_CSF) csf_attr = NVTrue;

                          break;
                        }
                    }

                  for (int32_t k = 0 ; k < LAS_ATTRIBUTES ; k++)
                    {
                      if (QString (open_args.head.ndx_attr_name[j]).contains (pfm_global.las_attribute[k].name))
                        {
                          pfm_global.attribute_count++;
                          pfm_global.las_attribute_num[k] = pfm_global.attribute_count;
                          break;
                        }
                    }

                  for (int32_t k = 0 ; k < BAG_ATTRIBUTES ; k++)
                    {
                      if (QString (open_args.head.ndx_attr_name[j]).contains (pfm_global.bag_attribute[k].name))
                        {
                          pfm_global.attribute_count++;
                          pfm_global.bag_attribute_num[k] = pfm_global.attribute_count;
                          break;
                        }
                    }
                }

              close_pfm_file (temp_handle);
            }
        }
      else
        {
          //  Check for CSF attributes in new (not pre-existing) PFMs

          for (int32_t k = 18 ; k < CZMIL_ATTRIBUTES ; k++)
            {
              if (pfm_global.czmil_attribute_num[k])
                {
                  csf_attr = NVTrue;
                  break;
                }
            }
        }
    }


  /*
      Check all input files against input files in any pre-existing PFM structures so we don't double load.

      IMPORTANT NOTE:  If we're using a .upr (update parameter file) then any file found in ANY PFM has already
      been beaten against ALL of the PFMs in the parameter file.  We don't want to keep checking and re-checking
      all of the files in a repeating load from one or more directories.  The files that have been loaded are 
      saved to the .upr file and checked against the new input files in the prliminary check section above.
  */

  for (int32_t i = 0 ; i < pfm_file_count ; i++)
    {
      if (pfm_def[i].existing)
        {
          int32_t temp_handle;
          PFM_OPEN_ARGS open_args;

          strcpy (open_args.list_path, pfm_def[i].name.toLatin1 ());

          open_args.checkpoint = 0;

          if ((temp_handle = open_existing_pfm_file (&open_args)) >= 0)
            {
              int16_t k = 0, type;
              char rfile[1024];
              while (!read_list_file (temp_handle, k, rfile, &type))
                {
                  for (int32_t j = 0 ; j < input_file_count ; j++)
                    {
                      if (input_file_def[j].status && !strcmp (rfile, input_file_def[j].name.toLatin1 ())) input_file_def[j].status = NVFalse;
                    }

                  k++;
                }
              close_pfm_file (temp_handle);
            }


          //  Check for existing WKT file when appending (this is only relevant for LAS files).

          char wkt_file[1024];
          sprintf (wkt_file, "%s.data%1c%s.wkt", open_args.list_path, SEPARATOR, pfm_basename (open_args.list_path));

          FILE *wfp;

          if ((wfp = fopen (wkt_file, "r")) != NULL)
            {
              char dta[8192];

              if (fread (dta, 1, 8192, wfp))
                {
                  strcpy (wkt_data.wkt, dta);
                  wkt_data.input_wkt = NVTrue;
                }
              fclose (wfp);
            }
        }
    }


  //  Check to see if we have any files left after all of the file checks above.

  uint8_t hit = NVFalse;

  for (int32_t i = 0 ; i < input_file_count ; i++)
    {
      if (input_file_def[i].status)
        {
          hit = NVTrue;
          break;
        }
    }

  if (!hit)
    {
      strcpy (out_str, pfmLoader::tr ("\n\nNo input files need to be loaded.  Terminating.\n\n").toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
      exit (0);
    }


  //  If we are using a .upr parameter file, update the .upr file with the final file list.

  if (pfm_global.upr_flag)
    {
      if (!update_upr_file (parameter_file, input_file_def, input_file_count))
        {
          strcpy (out_str, pfmLoader::tr ("\nUnable to update the parameter file!\n\n").toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
          exit (-1);
        }
    }


  PFM_LOAD_PARAMETERS load_parms[MAX_LOAD_FILES];


  //  Put everything into load_parms so we can pass it easily.

  for (int32_t i = 0 ; i < pfm_file_count ; i++) 
    {
      load_parms[i].flags = flags;
      load_parms[i].pfm_global = pfm_global;
    }


  //  Open the PFM files.

  for (int32_t i = 0 ; i < pfm_file_count ; i++)
    {
      char name[1024], short_name[512];
      strcpy (name, pfm_def[i].name.toLatin1 ());
      strcpy (short_name, QFileInfo (pfm_def[i].name).fileName ().toLatin1 ());


      //  If this is 0.0 then we're defining bin sizes in minutes of lat/lon

      if (pfm_def[i].mbin_size == 0.0)
        {
          pfm_def[i].open_args.head.y_bin_size_degrees = pfm_def[i].gbin_size / 60.0;
          pfm_def[i].open_args.head.x_bin_size_degrees = pfm_def[i].open_args.head.y_bin_size_degrees;


          /*  We've changed our collective minds.  After actually editing some data north of 64N we have found
              that there is no distortion due to elongated bins.  Therefore, Paul Marin has decided (and I agree)
              that we don't need to change the X bin size to match the Y bin size in distance.  I'm leaving the
              code here for reference (it didn't work though because we should have set a computed XY bin size).
              JCD  01/10/12

          //  We're going to use approximately spatially equivalent geographic bin sizes north or 64N and south of 64S.
          //  Otherwise we're going to use equal lat and lon bin sizes.

          if (pfm_def[i].open_args.head.mbr.min_y >= 64.0 || pfm_def[i].open_args.head.mbr.max_y <= -64.0)
            {
              double dist, az, y, x;
              if (pfm_def[i].open_args.head.mbr.min_y <= -64.0)
                {
                  invgp (NV_A0, NV_B0, pfm_def[i].open_args.head.mbr.max_y, pfm_def[i].open_args.head.mbr.min_x, 
                         pfm_def[i].open_args.head.mbr.max_y - (pfm_def[i].gbin_size / 60.0),
                         pfm_def[i].open_args.head.mbr.min_x, &dist, &az);
                }
              else
                {
                  invgp (NV_A0, NV_B0, pfm_def[i].open_args.head.mbr.min_y, pfm_def[i].open_args.head.mbr.min_x, 
                         pfm_def[i].open_args.head.mbr.min_y + (pfm_def[i].gbin_size / 60.0),
                         pfm_def[i].open_args.head.mbr.min_x, &dist, &az);
                }

              newgp (pfm_def[i].open_args.head.mbr.min_y, pfm_def[i].open_args.head.mbr.min_x, 90.0, dist, &y, &x);

              pfm_def[i].open_args.head.x_bin_size_degrees = x - pfm_def[i].open_args.head.mbr.min_x;
            }
          */
        }
      else
        {
          pfm_def[i].open_args.head.bin_size_xy = pfm_def[i].mbin_size;
        }


      if (!pfm_def[i].existing)
        {
          //  If I don't know what the CRS is, I'm going to put "Unknown" in the wkt and set the
          //  projection flag to zero to indicate that it is geodetic.

          if (strlen (wkt_data.pfm_wkt) > 9)
            {
              strcpy (pfm_def[i].open_args.head.proj_data.wkt, wkt_data.pfm_wkt);
            }
          else
            {
              strcpy (pfm_def[i].open_args.head.proj_data.wkt, "Unknown");
              pfm_def[i].open_args.head.proj_data.projection = 0;
            }


          strcpy (pfm_def[i].open_args.head.creation_software, VERSION);

          strcpy (out_str,
                  pfmLoader::tr ("Initializing PFM structure %1 of %2 - %3                                                 \n").arg (i + 1).arg 
                  (pfm_file_count).arg (short_name).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);

          get_version (pfm_def[i].open_args.head.version);
          strcpy (pfm_def[i].open_args.head.classification, "UNCLASSIFIED");

          time_t systemtime = time (&systemtime);
          strcpy (pfm_def[i].open_args.head.date, asctime (localtime (&systemtime)));

          pfm_def[i].open_args.head.date[strlen (pfm_def[i].open_args.head.date) - 1] = 0;


          //  Make the MBR from the input polygon points.

          pfm_def[i].open_args.head.mbr.min_y = pfm_def[i].open_args.head.mbr.min_x = 999.0;
          pfm_def[i].open_args.head.mbr.max_y = pfm_def[i].open_args.head.mbr.max_x = -999.0;

          pfm_def[i].open_args.head.polygon_count = pfm_def[i].polygon_count;
          for (int32_t j = 0 ; j < pfm_def[i].polygon_count ; j++)
            {
              pfm_def[i].open_args.head.polygon[j].y = pfm_def[i].polygon[j].y;
              pfm_def[i].open_args.head.polygon[j].x = pfm_def[i].polygon[j].x;

              pfm_def[i].open_args.head.mbr.min_y = qMin (pfm_def[i].open_args.head.mbr.min_y, pfm_def[i].open_args.head.polygon[j].y);
              pfm_def[i].open_args.head.mbr.min_x = qMin (pfm_def[i].open_args.head.mbr.min_x, pfm_def[i].open_args.head.polygon[j].x);
              pfm_def[i].open_args.head.mbr.max_y = qMax (pfm_def[i].open_args.head.mbr.max_y, pfm_def[i].open_args.head.polygon[j].y);
              pfm_def[i].open_args.head.mbr.max_x = qMax (pfm_def[i].open_args.head.mbr.max_x, pfm_def[i].open_args.head.polygon[j].x);
            }


          pfm_def[i].open_args.head.min_filtered_depth = 9999999.0;
          pfm_def[i].open_args.head.min_depth = 9999999.0;
          pfm_def[i].open_args.head.max_filtered_depth = -9999999.0;
          pfm_def[i].open_args.head.max_depth = -9999999.0;
          pfm_def[i].open_args.head.min_bin_count = 9999999;
          pfm_def[i].open_args.head.max_bin_count = 0;
          pfm_def[i].open_args.head.min_standard_dev = 9999999.0;
          pfm_def[i].open_args.head.max_standard_dev = -9999999.0;
        }
      else
        {
          strcpy (out_str, pfmLoader::tr ("Creating PFM checkpoint file %1 of %2 - %3").arg (i + 1).arg (pfm_file_count).arg (short_name).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
        }


      strcpy (pfm_def[i].open_args.list_path, pfm_def[i].name.toLatin1 ());
      strcpy (pfm_def[i].open_args.image_path, pfm_def[i].mosaic.toLatin1 ());
      strcpy (pfm_def[i].open_args.target_path, pfm_def[i].feature.toLatin1 ());
      pfm_def[i].open_args.max_depth = pfm_def[i].max_depth;
      pfm_def[i].open_args.offset = -pfm_def[i].min_depth;
      pfm_def[i].open_args.scale = 1.0 / pfm_def[i].precision;
      pfm_def[i].open_args.checkpoint = 1;
      pfm_def[i].open_args.head.dynamic_reload = NVTrue;
      pfm_def[i].open_args.head.num_bin_attr = 0;
      pfm_def[i].open_args.head.num_ndx_attr = 0;

      pfm_def[i].open_args.head.max_input_files = pfm_global.max_files;
      pfm_def[i].open_args.head.max_input_lines = pfm_global.max_lines;
      pfm_def[i].open_args.head.max_input_pings = pfm_global.max_pings;
      pfm_def[i].open_args.head.max_input_beams = pfm_global.max_beams;


      //  If we have attributes we need to define them.

      if (pfm_global.attribute_count)
        {
          pfm_def[i].open_args.head.num_ndx_attr = pfm_global.attribute_count;


          int32_t index = pfm_global.time_attribute_num - 1;
          if (pfm_global.time_attribute_num)
            {
              strcpy (pfm_def[i].open_args.head.ndx_attr_name[index], pfm_global.time_attribute.name);
              pfm_def[i].open_args.head.min_ndx_attr[index] = pfm_global.time_attribute.min;
              pfm_def[i].open_args.head.max_ndx_attr[index] = pfm_global.time_attribute.max;
              pfm_def[i].open_args.head.ndx_attr_scale[index] = pfm_global.time_attribute.scale;
            }

          for (int32_t j = 0 ; j < GSF_ATTRIBUTES ; j++)
            {
              int32_t index = pfm_global.gsf_attribute_num[j] - 1;
              if (pfm_global.gsf_attribute_num[j])
                {
                  strcpy (pfm_def[i].open_args.head.ndx_attr_name[index], pfm_global.gsf_attribute[j].name);
                  pfm_def[i].open_args.head.min_ndx_attr[index] = pfm_global.gsf_attribute[j].min;
                  pfm_def[i].open_args.head.max_ndx_attr[index] = pfm_global.gsf_attribute[j].max;
                  pfm_def[i].open_args.head.ndx_attr_scale[index] = pfm_global.gsf_attribute[j].scale;
                }
            }

          for (int32_t j = 0 ; j < HOF_ATTRIBUTES ; j++)
            {
              int32_t index = pfm_global.hof_attribute_num[j] - 1;
              if (pfm_global.hof_attribute_num[j])
                {
                  strcpy (pfm_def[i].open_args.head.ndx_attr_name[index], pfm_global.hof_attribute[j].name);
                  pfm_def[i].open_args.head.min_ndx_attr[index] = pfm_global.hof_attribute[j].min;
                  pfm_def[i].open_args.head.max_ndx_attr[index] = pfm_global.hof_attribute[j].max;
                  pfm_def[i].open_args.head.ndx_attr_scale[index] = pfm_global.hof_attribute[j].scale;
                }
            }

          for (int32_t j = 0 ; j < TOF_ATTRIBUTES ; j++)
            {
              int32_t index = pfm_global.tof_attribute_num[j] - 1;
              if (pfm_global.tof_attribute_num[j])
                {
                  strcpy (pfm_def[i].open_args.head.ndx_attr_name[index], pfm_global.tof_attribute[j].name);
                  pfm_def[i].open_args.head.min_ndx_attr[index] = pfm_global.tof_attribute[j].min;
                  pfm_def[i].open_args.head.max_ndx_attr[index] = pfm_global.tof_attribute[j].max;
                  pfm_def[i].open_args.head.ndx_attr_scale[index] = pfm_global.tof_attribute[j].scale;
                }
            }


          for (int32_t j = 0 ; j < CZMIL_ATTRIBUTES ; j++)
            {
              int32_t index = pfm_global.czmil_attribute_num[j] - 1;
              if (pfm_global.czmil_attribute_num[j])
                {
                  strcpy (pfm_def[i].open_args.head.ndx_attr_name[index], pfm_global.czmil_attribute[j].name);
                  pfm_def[i].open_args.head.min_ndx_attr[index] = pfm_global.czmil_attribute[j].min;
                  pfm_def[i].open_args.head.max_ndx_attr[index] = pfm_global.czmil_attribute[j].max;
                  pfm_def[i].open_args.head.ndx_attr_scale[index] = pfm_global.czmil_attribute[j].scale;
                }
            }

          for (int32_t j = 0 ; j < LAS_ATTRIBUTES ; j++)
            {
              int32_t index = pfm_global.las_attribute_num[j] - 1;
              if (pfm_global.las_attribute_num[j])
                {
                  strcpy (pfm_def[i].open_args.head.ndx_attr_name[index], pfm_global.las_attribute[j].name);
                  pfm_def[i].open_args.head.min_ndx_attr[index] = pfm_global.las_attribute[j].min;
                  pfm_def[i].open_args.head.max_ndx_attr[index] = pfm_global.las_attribute[j].max;
                  pfm_def[i].open_args.head.ndx_attr_scale[index] = pfm_global.las_attribute[j].scale;
                }
            }


          for (int32_t j = 0 ; j < BAG_ATTRIBUTES ; j++)
            {
              int32_t index = pfm_global.bag_attribute_num[j] - 1;
              if (pfm_global.bag_attribute_num[j])
                {
                  strcpy (pfm_def[i].open_args.head.ndx_attr_name[index], pfm_global.bag_attribute[j].name);
                  pfm_def[i].open_args.head.min_ndx_attr[index] = pfm_global.bag_attribute[j].min;
                  pfm_def[i].open_args.head.max_ndx_attr[index] = pfm_global.bag_attribute[j].max;
                  pfm_def[i].open_args.head.ndx_attr_scale[index] = pfm_global.bag_attribute[j].scale;
                }
            }
        }


      //  These are hard-wired to run if the data type is present in a new file (pfmLoad now limits data to a single type).

      if (flags.lid)
        {
          strcpy (pfm_def[i].open_args.head.user_flag_name[0], "Shallow Water Process!");
          strcpy (pfm_def[i].open_args.head.user_flag_name[1], "APD!");
          strcpy (pfm_def[i].open_args.head.user_flag_name[2], "Land!");
          strcpy (pfm_def[i].open_args.head.user_flag_name[3], "Second Depth Present!");
        }
      else if (flags.czmil)
        {
          load_parms[0].flags.czmil = flags.czmil;
          load_parms[0].pfm_global.czmil_flag_count = CZMIL_FLAGS;

          for (int32_t flg = 0 ; flg < CZMIL_FLAGS ; flg++)
            {
              load_parms[0].pfm_global.czmil_flag_num[flg] = flg + 1;
              int32_t flag_num = load_parms[0].pfm_global.czmil_flag_num[flg] - 1;
              strcpy (pfm_def[i].open_args.head.user_flag_name[flag_num], load_parms[0].pfm_global.czmil_flag_name[flg].toLatin1 ());
            }
        }


      //  If we have CUBE attributes we need to add them to any pre-existing bin attributes.

      if (flags.attr)
        {
          int32_t status = stat (pfm_def[i].open_args.list_path, &fstat);

          if (status == -1 && errno == ENOENT)
            {
              pfm_def[i].open_args.head.num_bin_attr = 6;
              strcpy (pfm_def[i].open_args.head.bin_attr_name[0], "###0");
              strcpy (pfm_def[i].open_args.head.bin_attr_name[1], "###1");
              strcpy (pfm_def[i].open_args.head.bin_attr_name[2], "###2");
              strcpy (pfm_def[i].open_args.head.bin_attr_name[3], "###3");
              strcpy (pfm_def[i].open_args.head.bin_attr_name[4], "###4");
              strcpy (pfm_def[i].open_args.head.bin_attr_name[5], "###5");


              pfm_def[i].open_args.head.min_bin_attr[0] = 0.0;
              pfm_def[i].open_args.head.min_bin_attr[1] = 0.0;
              pfm_def[i].open_args.head.min_bin_attr[2] = 0.0;
              pfm_def[i].open_args.head.min_bin_attr[3] = 0.0;
              pfm_def[i].open_args.head.min_bin_attr[4] = 0.0;
              pfm_def[i].open_args.head.min_bin_attr[5] = 0.0;
              pfm_def[i].open_args.head.max_bin_attr[0] = 100;
              pfm_def[i].open_args.head.max_bin_attr[1] = 100.0;
              pfm_def[i].open_args.head.max_bin_attr[2] = 100.0;
              pfm_def[i].open_args.head.max_bin_attr[3] = 32000.0;
              pfm_def[i].open_args.head.max_bin_attr[4] = 100.0;
              pfm_def[i].open_args.head.max_bin_attr[5] = 100.0;
              pfm_def[i].open_args.head.bin_attr_scale[0] = 1.0;
              pfm_def[i].open_args.head.bin_attr_scale[1] = 100.0;
              pfm_def[i].open_args.head.bin_attr_scale[2] = 100.0;
              pfm_def[i].open_args.head.bin_attr_scale[3] = 1.0;
              pfm_def[i].open_args.head.bin_attr_scale[4] = 100.0;
              pfm_def[i].open_args.head.bin_attr_scale[5] = 100.0;
            }
        }


      int32_t status = stat (pfm_def[i].open_args.list_path, &fstat);

      if (status == -1 && errno == ENOENT)
        {
          //  Using 10% of max depth as maximum vertical error value except when the max depth is less than 150, then we use 15.0.

          if (pfm_def[i].open_args.max_depth < 150.0)
            {
              pfm_def[i].open_args.head.max_vertical_error = 15.0;
            }
          else
            {
              pfm_def[i].open_args.head.max_vertical_error = pfm_def[i].open_args.max_depth * 0.10;
            }


          //  Setting maximum horizontal error to 10 times the bin size except when we are using bin sizes set in minutes
          //  (in which case bin_size_xy is set to 0.0).  In that case we must first figure out what the nominal bin size is
          //  in meters.

          if (pfm_def[i].open_args.head.bin_size_xy != 0.0)
            {
              pfm_def[i].open_args.head.max_horizontal_error = pfm_def[i].open_args.head.bin_size_xy * 10.0;
            }
          else
            {
              double dist, az;

              invgp (NV_A0, NV_B0, pfm_def[i].open_args.head.mbr.min_y, pfm_def[i].open_args.head.mbr.min_x, pfm_def[i].open_args.head.mbr.min_y +
                     (pfm_def[i].gbin_size / 60.0), pfm_def[i].open_args.head.mbr.min_x, &dist, &az);
              pfm_def[i].open_args.head.max_horizontal_error = dist * 10.0;
            }


          //  Horizontal and vertical error scales set to centimeters.

          pfm_def[i].open_args.head.horizontal_error_scale = 100.0;
          pfm_def[i].open_args.head.vertical_error_scale = 100.0;
        }


      if ((pfm_def[i].hnd = open_cached_pfm_file (&pfm_def[i].open_args)) < 0) pfm_error_exit (pfm_error);
      //if ((pfm_def[i].hnd = open_pfm_file (&pfm_def[i].open_args)) < 0) pfm_error_exit (pfm_error);


      //  Force HOF LIDAR flags if they were in a pre-existing file.

      for (int32_t j = 0 ; j < PFM_USER_FLAGS ; j++)
        {
          if (strstr (pfm_def[i].open_args.head.user_flag_name[j], "Shallow Water Process"))
            {
              flags.lid = NVTrue;
              break;
            }
        }


      //  Force CZMIL LIDAR flags if they were in a pre-existing file.

      for (int32_t j = 0 ; j < PFM_USER_FLAGS ; j++)
        {
          for (int32_t k = 0 ; k < CZMIL_FLAGS ; k++)
            {
              if (strstr (pfm_def[i].open_args.head.user_flag_name[j], pfm_global.czmil_flag_name[k].toLatin1 ()))
                {
                  pfm_global.czmil_flag_num[j] = j + 1;
                  break;
                }
            }
        }


      //  Force CUBE attribute updates if they were in a pre-existing file.

      for (int32_t j = 0 ; j < pfm_def[i].open_args.head.num_bin_attr ; j++)
        {
          if (!strcmp (pfm_def[i].open_args.head.bin_attr_name[j], "###0"))
            {
              flags.attr = NVTrue;
              break;
            }
        }


      //  Check for dateline crossing.

      pfm_def[i].dateline = NVFalse;
      if (!pfm_def[i].open_args.head.proj_data.projection && pfm_def[i].open_args.head.mbr.max_x > 180.0) pfm_def[i].dateline = NVTrue;


      //  Allocate and clear memory for the bin add_map.

      pfm_def[i].add_map = (int8_t *) calloc (pfm_def[i].open_args.head.bin_width * pfm_def[i].open_args.head.bin_height, sizeof (int8_t));

      if (pfm_def[i].add_map == NULL)
        {
          perror ("Unable to allocate memory for bin_map.");
          exit (-1);
        }
    }

  strcpy (out_str, pfmLoader::tr ("PFM structure initialization complete\n\n").toUtf8 ());
  fprintf (stderr, "%s", out_str);
  fflush (stderr);


  //  Zero sounding count 

  int64_t count[MAX_PFM_FILES], out_of_range[MAX_PFM_FILES];

  memset (count, 0, MAX_PFM_FILES * sizeof (int64_t));
  memset (out_of_range, 0, MAX_PFM_FILES * sizeof (int64_t));


  int64_t total_input_count = 0;
  QString tmp;


  //  Save the time to the handle file as a comment

  for (int32_t i = 0 ; i < pfm_file_count ; i++)
    {
      hfp = fopen (pfm_def[i].open_args.list_path, "a");

      QDateTime current_time = QDateTime::currentDateTime ();

      strcpy (out_str, pfmLoader::tr ("#\n#\n# Date : %1  (GMT)\n#\n").arg (current_time.toString ()).toUtf8 ());
      fprintf (hfp, "%s", out_str);
      fclose (hfp);


      //  Check for new file and make the initial parameter file.  Also, save the global and attribute options to an initial parameter file
      //  in the pfm.data folder if this is an initial load (not an append).

      if (!pfm_def[i].existing)
        {
          QString ipf_file;
          ipf_file.sprintf ("%s.data%1c%s.ipf", pfm_def[i].open_args.list_path, SEPARATOR, pfm_basename (pfm_def[i].open_args.list_path));

          writeParameterFile (ipf_file, pfm_global, flags);
        }
    }


  //  Set the program name for LAS_WKT.

  strcpy (wkt_data.progname, argv[0]);
  strcpy (wkt_data.pfm_wkt, "");


  total_input_count = 0;
  for (int32_t i = 0 ; i < input_file_count ; i++)
    {
      //  Make sure that the file status was good.

      if (input_file_def[i].status)
        {
          char name[1024], short_name[512];
          strcpy (name, input_file_def[i].name.toLatin1 ());
          strcpy (short_name, QFileInfo (input_file_def[i].name).fileName ().toLatin1 ());


          //  IMPORTANT NOTE - do not change this line without modifying slotLoadReadyReadStandardError in pfmLoad.

          fprintf (stderr, "Reading file %d of %d - %s                                         \n", i + 1, input_file_count, short_name);
          fflush (stderr);


          //  Note: When adding other file types to process, add them here, the processing within the call is 
          //  resposible for opening the file, parsing it, navigating it and calling Do_PFM_Processing with the 
          //  appropriate arguments.  We can use keywords within the filename string to determine the file type
          //  or open and look for a header. 

          switch (input_file_def[i].type)
            {
            case PFM_GSF_DATA:
              out_of_range[i] += load_gsf_file (pfm_file_count, count, input_file_def[i].name, load_parms, pfm_def);
              break;

            case PFM_CHARTS_HOF_DATA:
              out_of_range[i] += load_hof_file (pfm_file_count, count, input_file_def[i].name, load_parms, pfm_def);
              break;

            case PFM_SHOALS_TOF_DATA:
              out_of_range[i] += load_tof_file (pfm_file_count, count, input_file_def[i].name, load_parms, pfm_def);
              break;

            case PFM_NAVO_LLZ_DATA:
              out_of_range[i] += load_llz_file (pfm_file_count, count, input_file_def[i].name, load_parms, pfm_def);
              break;

            case PFM_CZMIL_DATA:
              out_of_range[i] += load_czmil_file (pfm_file_count, count, input_file_def[i].name, csf_attr, load_parms, pfm_def);
              break;

            case PFM_LAS_DATA:
              out_of_range[i] += load_las_file (pfm_file_count, count, input_file_def[i].name, load_parms, pfm_def, &wkt_data);
              break;

            case PFM_BAG_DATA:
              out_of_range[i] += load_bag_file (pfm_file_count, count, input_file_def[i].name, load_parms, pfm_def, &wkt_data);
              break;

            case PFM_DTED_DATA:
              out_of_range[i] += load_dted_file (pfm_file_count, count, input_file_def[i].name, load_parms, pfm_def);
              break;

            case PFM_CHRTR_DATA:
              out_of_range[i] += load_chrtr_file (pfm_file_count, count, input_file_def[i].name, load_parms, pfm_def);
              break;
            }

          total_input_count = 0;
          for (int32_t j = 0 ; j < pfm_file_count ; j++) 
            {
              hfp = fopen (pfm_def[j].open_args.list_path, "a");

              strcpy (out_str, pfmLoader::tr ("# File : %1\n# Total data points read = %2\n").arg (input_file_def[i].name).arg (count[j]).toUtf8 ());
              fprintf (hfp, "%s", out_str);
              fclose (hfp);

              total_input_count += count[j];
            }

          strcpy (out_str, pfmLoader::tr ("Total data points read = %1 for file %2                                         \n").arg (total_input_count).arg
                  (short_name).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
        }
    }


  //  Just in case we only loaded SRTM data.

  if (input_file_count)
    {
      //  IMPORTANT NOTE - do not change this line without modifying slotLoadReadyReadStandardError in pfmLoad.

      fprintf (stderr, "Reading input files complete\n\n");
      fflush (stderr);
    }


  //  If we are loading SRTM data, call the load function.

  if (load_parms[0].flags.srtmb || load_parms[0].flags.srtm1 || load_parms[0].flags.srtm3 || load_parms[0].flags.srtm30)
    {
      strcpy (out_str, pfmLoader::tr ("Reading SRTM data\n\n").toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);


      load_srtm_file (pfm_file_count, count, load_parms, pfm_def);


      for (int32_t j = 0 ; j < pfm_file_count ; j++) 
        {
          hfp = fopen (pfm_def[j].open_args.list_path, "a");

          if (load_parms[0].flags.srtmb)
            {
              strcpy (out_str, pfmLoader::tr ("\n\nBest available resolution SRTM data\n").toUtf8 ());
              fprintf (hfp, "%s", out_str);
            }
          else if (load_parms[0].flags.srtm1)
            {
              strcpy (out_str, pfmLoader::tr ("\n\n1 second resolution SRTM data\n").toUtf8 ());
              fprintf (hfp, "%s", out_str);
            }
          else if (load_parms[0].flags.srtm3)
            {
              strcpy (out_str, pfmLoader::tr ("\n\n3 second resolution SRTM data\n").toUtf8 ());
              fprintf (hfp, "%s", out_str);
            }
          else if (load_parms[0].flags.srtm30)
            {
              strcpy (out_str, pfmLoader::tr ("\n\n30 second resolution SRTM data\n").toUtf8 ());
              fprintf (hfp, "%s", out_str);
            }

          strcpy (out_str, pfmLoader::tr ("Total data points read = %1\n").arg (total_input_count).toUtf8 ());
          fprintf (hfp, "%s", out_str);
          fclose (hfp);

          total_input_count += count[j];
        }


      if (load_parms[0].flags.srtmb)
        {
          strcpy (out_str, pfmLoader::tr ("\n\nBest available resolution SRTM data\n").toUtf8 ());
          fprintf (stderr, "%s", out_str);
        }
      else if (load_parms[0].flags.srtm1)
        {
          strcpy (out_str, pfmLoader::tr ("\n\n1 second resolution SRTM data\n").toUtf8 ());
          fprintf (stderr, "%s", out_str);
        }
      else if (load_parms[0].flags.srtm3)
        {
          strcpy (out_str, pfmLoader::tr ("\n\n3 second resolution SRTM data\n").toUtf8 ());
          fprintf (stderr, "%s", out_str);
        }
      else if (load_parms[0].flags.srtm30)
        {
          strcpy (out_str, pfmLoader::tr ("\n\n30 second resolution SRTM data\n").toUtf8 ());
          fprintf (stderr, "%s", out_str);
        }

      strcpy (out_str, pfmLoader::tr ("Total data points read = %1\n").arg (total_input_count).toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
    }
  else
    {
      strcpy (out_str, pfmLoader::tr ("No SRTM data loaded\n").toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
    }


  //  Write that we've finished loading into each PFM handle file.  Also, check for WKT defined for LAS, CZMIL, GSF input files.

  for (int32_t k = 0 ; k < pfm_file_count ; k++)
    {
      //  Post 4.6 structure (handle file)

      hfp = fopen (pfm_def[k].open_args.list_path, "a");

      strcpy (out_str, pfmLoader::tr ("# --------------------------------------\n"
                                      "# Finished with data load\n"
                                      "# --------------------------------------\n").toUtf8 ());
      fprintf (hfp, "%s", out_str);


      //  Check for input WKT.

      if (wkt_data.input_wkt)
        {
          //  Make the WKT human readable

          OGRSpatialReference SRS;
          char pretty[8192];
          char *ppszPretty, *ptr_wkt = wkt_data.wkt;

          SRS.importFromWkt (&ptr_wkt);

          SRS.exportToPrettyWkt (&ppszPretty);

          strcpy (pretty, ppszPretty);
          OGRFree (ppszPretty);

          strcpy (out_str, pfmLoader::tr ("\nUsing the following WKT:\n\n").toUtf8 ());
          fprintf (stderr, "%s", out_str);
          if (!pfm_def[k].existing) fprintf (hfp, "%s", out_str);

          for (uint32_t m = 0 ; m < strlen (pretty) ; m++)
            {
              if (pretty[m] == '\n')
                {
                  fprintf (stderr, "\n");
                  if (!pfm_def[k].existing) fprintf (hfp, "\n#");
                }
              else
                {
                  fprintf (stderr, "%1c", pretty[m]);
                  if (!pfm_def[k].existing) fprintf (hfp, "%1c", pretty[m]);
                }
            }

          fprintf (stderr, "\n\n");
          if (!pfm_def[k].existing) fprintf (hfp, "#\n#\n");


          if (!pfm_def[k].existing) 
            {
              char wkt_file[1024];
              sprintf (wkt_file, "%s.data%1c%s.wkt", pfm_def[k].open_args.list_path, SEPARATOR, pfm_basename (pfm_def[k].open_args.list_path));

              FILE *wfp;

              if ((wfp = fopen (wkt_file, "w")) != NULL)
                {
                  fprintf (wfp, "%s\n", wkt_data.wkt);
                  fclose (wfp);
                }
            }
        }

      fclose (hfp);
    }


  for (int32_t k = 0 ; k < pfm_file_count ; k++)
    {
      char name[1024], short_name[512];
      strcpy (name, pfm_def[k].name.toLatin1 ());
      strcpy (short_name, QFileInfo (pfm_def[k].name).fileName ().toLatin1 ());


      //  Close and reopen the PFM files so that we can take advantage of the head and tail pointers in the bin records.

      close_cached_pfm_file (pfm_def[k].hnd);
      //close_pfm_file (pfm_def[k].hnd);
      pfm_def[k].open_args.checkpoint = 0;

      if ((pfm_def[k].hnd = open_existing_pfm_file (&pfm_def[k].open_args)) < 0)
        {
          strcpy (out_str, pfmLoader::tr ("An error occurred while trying to recompute the bin surfaces for file %1\n"
                                          "The error message was : %2\n"
                                          "Please try running pfm_recompute on the file to correct the problem.\n").arg (pfm_def[k].open_args.list_path).arg 
                  (pfm_error_str (pfm_error)).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);

          pfm_error_exit (pfm_error);
        }


      //  Compute diagonal for area filter.  First we have to check to see if we're using geographic or meter bin sizes.


      double bin_diagonal;
      if (fabs (pfm_def[k].open_args.head.x_bin_size_degrees - pfm_def[k].open_args.head.y_bin_size_degrees) < 0.000001)
        {
          double az;
          if (pfm_def[k].open_args.head.mbr.min_y <= 0.0)
            {
              invgp (NV_A0, NV_B0, pfm_def[k].open_args.head.mbr.max_y, pfm_def[k].open_args.head.mbr.min_x, 
                     pfm_def[k].open_args.head.mbr.max_y - (pfm_def[k].open_args.head.y_bin_size_degrees), 
                     pfm_def[k].open_args.head.mbr.min_x + (pfm_def[k].open_args.head.x_bin_size_degrees), 
                     &bin_diagonal, &az);
            }
          else
            {
              invgp (NV_A0, NV_B0, pfm_def[k].open_args.head.mbr.min_y, pfm_def[k].open_args.head.mbr.min_x, 
                     pfm_def[k].open_args.head.mbr.min_y + (pfm_def[k].open_args.head.y_bin_size_degrees), 
                     pfm_def[k].open_args.head.mbr.min_x + (pfm_def[k].open_args.head.x_bin_size_degrees), 
                     &bin_diagonal, &az);
            }
        }
      else
        {
          bin_diagonal = 2.0 * sqrt (pfm_def[k].open_args.head.bin_size_xy);
        }


      int32_t total_bins = pfm_def[k].open_args.head.bin_height * pfm_def[k].open_args.head.bin_width;


      //  If we are going to use the area filter we need to recompute all those cells that were modified.

      if (pfm_def[k].apply_area_filter)
        {
          InitializeAreaFilter (&pfm_def[k]);

          strcpy (out_str, pfmLoader::tr ("Filtering bins in PFM %1 of %2 - %3\n").arg (k + 1).arg (pfm_file_count).arg (short_name).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
        }
      else
        {
          strcpy (out_str, pfmLoader::tr ("Recomputing bins in PFM %1 of %2 - %3\n").arg (k + 1).arg (pfm_file_count).arg (short_name).toUtf8 ());
          fprintf (stderr, "%s", out_str);
          fflush (stderr);
        }


      NV_I32_COORD2 coord;
      BIN_RECORD bin_record;
      uint8_t clear_features (int32_t pfm_handle, NV_I32_COORD2 coord, char *lst, double feature_radius);


      //  Recompute all of the bins and min/max values.

      for (int32_t i = 0; i < pfm_def[k].open_args.head.bin_height; i++)
        {
          coord.y = i;
          for (int32_t j = 0; j < pfm_def[k].open_args.head.bin_width; j++)
            {
              coord.x = j;
              if (pfm_def[k].add_map[i * pfm_def[k].open_args.head.bin_width + j])
                {

                  //  Clear filter edits around features.

                  uint8_t trg = clear_features (pfm_def[k].hnd, coord, pfm_def[k].open_args.list_path, pfm_def[k].radius);


                  //  Don't filter if there was a feature within "radius" of the center of the bin.

                  if (!trg && pfm_def[k].apply_area_filter)
                    {
                      AreaFilter (&coord, &bin_record, &pfm_def[k], bin_diagonal);
                    }
                  else
                    {
                      recompute_bin_values_index (pfm_def[k].hnd, coord, &bin_record, 0);
                    }


                  //  If we're appending data to a pre-existing PFM, reset the PFM_CHECKED and PFM_VERIFIED flags, even around features.

                  if (pfm_def[k].existing)
                    {
                      bin_record.validity &= (~PFM_CHECKED & ~PFM_VERIFIED);
                      write_bin_record_validity_index (pfm_def[k].hnd, &bin_record, (PFM_CHECKED | PFM_VERIFIED));
                    }


                  if (bin_record.num_soundings)
                    {
                      if (bin_record.num_soundings < pfm_def[k].open_args.head.min_bin_count)
                        {
                          pfm_def[k].open_args.head.min_bin_count = bin_record.num_soundings;
                          pfm_def[k].open_args.head.min_count_coord = bin_record.coord;
                        }

                      if (bin_record.num_soundings > pfm_def[k].open_args.head.max_bin_count)
                        {
                          pfm_def[k].open_args.head.max_bin_count = bin_record.num_soundings;
                          pfm_def[k].open_args.head.max_count_coord = bin_record.coord;
                        }


                      if (bin_record.max_depth < pfm_def[k].max_depth + 1.0)
                        {
                          if (bin_record.validity & PFM_DATA)
                            {
                              if (bin_record.min_filtered_depth < pfm_def[k].open_args.head.min_filtered_depth)
                                {
                                  pfm_def[k].open_args.head.min_filtered_depth = bin_record.min_filtered_depth;
                                  pfm_def[k].open_args.head.min_filtered_coord = bin_record.coord;
                                }

                              if (bin_record.max_filtered_depth > pfm_def[k].open_args.head.max_filtered_depth)
                                {
                                  pfm_def[k].open_args.head.max_filtered_depth = bin_record.max_filtered_depth;
                                  pfm_def[k].open_args.head.max_filtered_coord = bin_record.coord;
                                }

                              if (bin_record.standard_dev < pfm_def[k].open_args.head.min_standard_dev)
                                pfm_def[k].open_args.head.min_standard_dev = bin_record.standard_dev;

                              if (bin_record.standard_dev > pfm_def[k].open_args.head.max_standard_dev)
                                pfm_def[k].open_args.head.max_standard_dev = bin_record.standard_dev;
                            }

                          if (bin_record.min_depth < pfm_def[k].open_args.head.min_depth)
                            {
                              pfm_def[k].open_args.head.min_depth = bin_record.min_depth;
                              pfm_def[k].open_args.head.min_coord = bin_record.coord;
                            }

                          if (bin_record.max_depth > pfm_def[k].open_args.head.max_depth)
                            {
                              pfm_def[k].open_args.head.max_depth = bin_record.max_depth;
                              pfm_def[k].open_args.head.max_coord = bin_record.coord;
                            }
                        }
                    }
                }
            }


          percent = (int32_t) (round (((float) (i * pfm_def[k].open_args.head.bin_width) / (float) total_bins) * 100.0));

          if (percent != old_percent)
            {
              strcpy (out_str, pfmLoader::tr ("%1% processed\r").arg (percent, 3, 10, QChar ('0')).toUtf8 ());
              fprintf (stderr, "%s", out_str);
              fflush (stderr);
              old_percent = percent;
            }
        }


      if (pfm_def[k].apply_area_filter)
        {
          strcpy (out_str, pfmLoader::tr ("Filtering bins - complete\n\n").toUtf8 ());
          fprintf (stderr, "%s", out_str);
        }
      else
        {
          strcpy (out_str, pfmLoader::tr ("Recomputing bins - complete\n\n").toUtf8 ());
          fprintf (stderr, "%s", out_str);
        }


      //  If we read a valid CZMIL CPF file, or a LAS 1.4 file with a geographic WKT (GEOGCS), or a BAG file with valid WKT or legacy reference system,
      //  or the user input a geographic WKT for LAS files (any version) we are going to store the WKT in the PFM header.  This will allow us to export
      //  data from PFM (e.g. pfmBag, pfmExport) correctly.

      if (strlen (wkt_data.pfm_wkt) > 10) strcpy (pfm_def[k].open_args.head.proj_data.wkt, wkt_data.pfm_wkt);


      //  Now write the updated bin header.

      write_bin_header (pfm_def[k].hnd, &pfm_def[k].open_args.head, NVFalse);


      free (pfm_def[k].add_map);


      close_pfm_file (pfm_def[k].hnd);


      //  Post 4.6 structure (handle file)

      hfp = fopen (pfm_def[k].open_args.list_path, "r+");
      if (fgets (string, sizeof (string), hfp) == NULL)
	{
          strcpy (out_str, pfmLoader::tr ("Error reading the handle file %1 in file %2, function %3, line %4.  This shouldn't happen!\n").arg 
                  (pfm_def[k].open_args.list_path).arg (__FILE__).arg (__FUNCTION__).arg (__LINE__).toUtf8 ());
          fprintf (stderr, "%s", out_str);
	  exit (-1);
	}

      char ctl_file[512];
      strcpy (ctl_file, pfm_def[k].open_args.list_path);

      if (strstr (string, "PFM Handle File"))
        {
          sprintf (ctl_file, "%s.data%1c%s.ctl", pfm_def[k].open_args.list_path, (char) SEPARATOR,
                   pfm_basename (pfm_def[k].open_args.list_path));
        }

      sprintf (string, "%s.bak", ctl_file);


      FILE *fp1 = fopen (ctl_file, "r");
      FILE *fp2 = fopen (string, "w+");

      if (fp1 != NULL && fp2 != NULL)
        {
          while (fgets (string, sizeof (string), fp1) != NULL) fprintf (fp2, "%s", string);
          fclose (fp1);


#ifdef NVLinux

          //  Substitute paths if flag is set.

          if (load_parms[0].flags.sub)
            {
              remove (ctl_file);
              fseek (fp2, 0, SEEK_SET);
              fp1 = fopen (ctl_file, "w");

              ngets (string, sizeof (string), fp2);
              fprintf (fp1, "%s\n", string);

              ngets (string, sizeof (string), fp2);
              inv_sub (string);
              fprintf (fp1, "%s\n", string);

              ngets (string, sizeof (string), fp2);
              inv_sub (string);
              fprintf (fp1, "%s\n", string);

              ngets (string, sizeof (string), fp2);
              if (strcmp (string, "NONE")) inv_sub (string);
              fprintf (fp1, "%s\n", string);

              ngets (string, sizeof (string), fp2);
              if (strcmp (string, "NONE")) inv_sub (string);
              fprintf (fp1, "%s\n", string);

              char pre[20], post[512];
              while (ngets (string, sizeof (string), fp2) != NULL)
                {
                  strncpy (pre, string, 11);
                  pre[11] = 0;
                  strcpy (post, &string[11]);
                  inv_sub (post);
                  fprintf (fp1, "%s%s\n", pre, post);
                }

              fclose (fp1);
            }

#endif

          fclose (fp2);
        }
      else
        {
          if (fp1 != NULL) fclose (fp1);
          if (fp2 != NULL) fclose (fp2);
          strcpy (out_str, pfmLoader::tr ("\n\nUnable to create backup list file %1.\n\n").arg (string).toUtf8 ());
          fprintf (stderr, "%s", out_str);
        }


      if (pfm_def[k].apply_area_filter) 
        {
          FILTER_SUMMARY summary;
          FinalizeAreaFilter (&summary);


          double good_ratio, bad_ratio, number = (double) summary.TotalSoundings;

          if (number > 0.0)
            {
              good_ratio = (double) summary.GoodSoundings / number * 100.0;
              bad_ratio = (double) summary.BadSoundings / number * 100.0;


              strcpy (out_str, pfmLoader::tr ("# --------------------------------------\n"
                                              "# PFM Filter Statistics\n"
                                              "# Total number of points processed %1\n"
                                              "# Number of soundings Good     : %2\n"
                                              "# Percent of soundings Good     : %3\n"
                                              "# Number of soundings Filtered : %4\n"
                                              "# Percent of soundings Filtered : %f\n"
                                              "# --------------------------------------\n").arg 
                      (summary.TotalSoundings).arg (summary.GoodSoundings).arg (good_ratio).arg (summary.BadSoundings).arg (bad_ratio).toUtf8 ());

              fprintf (stderr, "%s", out_str);

              fseek (hfp, 0, SEEK_END);
              fprintf (hfp, "%s", out_str);

            }
        }
      else
        {
          strcpy (out_str, pfmLoader::tr ("# --------------------------------------\n"
                                          "# Finished with recompute section\n"
                                          "# --------------------------------------\n").toUtf8 ());
          fprintf (stderr, "%s", out_str);

          fseek (hfp, 0, SEEK_END);
          fprintf (hfp, "%s", out_str);
        }


      if (out_of_range[k] > 5000) 
        {
          strcpy (out_str, pfmLoader::tr ("\n#**** WARNING ****\n"
                                          "#File %1 :\n"
                                          "#%2 soundings out of min/max range!\n"
                                          "#These will be marked invalid in the input files\n"
                                          "#if you unload this file!\n"
                                          "#*****************\n").arg 
                  (pfm_def[k].open_args.list_path).arg (out_of_range[k]).toUtf8 ());
          fprintf (stderr, "%s", out_str);

          fseek (hfp, 0, SEEK_END);
          fprintf (hfp, "%s", out_str);
        }

      fclose (hfp);
    }


  //  If we loaded LAZ files we need a warning that they are slower unloading.

  if (laz_warning)
    {
      strcpy (out_str, pfmLoader::tr ("\n************************************ WARNING ***************************************\n"
                                      "You have loaded LAZ files.\n"
                                      "Changes to LAZ data can be unloaded but it will be slower than unloading LAS files\n"
                                      "because the LAZ file must be uncompressed before unloading then re-compressed after.\n"
                                      "************************************** WARNING ***************************************\n\n").toUtf8 ());
      fprintf (stderr, "%s", out_str);
      fflush (stderr);
    }
}



pfmLoader::~pfmLoader ()
{
}
