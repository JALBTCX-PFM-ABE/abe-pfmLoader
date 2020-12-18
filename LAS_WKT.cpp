
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


#include "LAS_WKT.hpp"
#include "LAS_WKTHelp.hpp"


LAS_WKT::LAS_WKT (QWidget *pa, WKT *wd, LASheader *lash, char *path):
  QDialog (pa, (Qt::WindowFlags) Qt::WA_DeleteOnClose)
{
  parent = pa;
  wkt_data = wd;
  lasheader = lash;
  strcpy (filename, path);


  //  We need to get any recent WKT values from the globalABE settings file.  This file is used to store things that
  //  may be used by multiple ABE programs.

#ifdef NVWIN3X
  QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/" + "globalABE.ini";
#else
  QString ini_file = QString (getenv ("HOME")) + "/ABE.config/" + "globalABE.ini";
#endif

  QSettings settings (ini_file, QSettings::IniFormat);
  settings.beginGroup ("globalABE");


  for (int32_t i = 0 ; i < 10 ; i++)
    {
      wkt_data->wktString[i] = "";

      QString name = tr ("Recent WKT %1").arg (i);
      wkt_data->wktString[i] = settings.value (name, wkt_data->wktString[i]).toString ();
    }

  settings.endGroup ();


  char string[8192];

  wkt_data->pj_utm = NULL;
  wkt_data->pj_latlon = NULL;
  wkt_data->projected = NVFalse;
  strcpy (wkt_data->wkt, "");

  shortName = QFileInfo (wkt_data->progname).fileName ();


  ask_for_wkt = 0;
  wktD = NULL;


  //  First we have to determine if this file is supported.  Is this LAS 1.4?

  if (lasheader->version_minor == 4)
    {
      //  If the global encoding field says we have WKT, look for it.

      if (lasheader->global_encoding & 0x10)
        {
          uint8_t wkt_present = NVFalse;


          //  Check the variable length records.

          for (int32_t k = 0 ; k < (int32_t) lasheader->number_of_variable_length_records ; k++)
            {
              //  WKT coordinate system

              if (lasheader->vlrs[k].record_id == 2112)
                {
                  strcpy (wkt_data->wkt, (char *) lasheader->vlrs[k].data);
                  wkt_present = NVTrue;
                  break;
                }
            }


          //  If we didn't find it there, look in the extended VLRs.

          if (!wkt_present)
            {
              for (int32_t k = 0 ; k < (int32_t) lasheader->number_of_extended_variable_length_records ; k++)
                {
                  //  WKT coordinate system

                  if (lasheader->evlrs[k].record_id == 2112)
                    {
                      strcpy (wkt_data->wkt, (char *) lasheader->evlrs[k].data);
                      wkt_present = NVTrue;
                      break;
                    }
                }
            }


          if (wkt_present)
            {
              //  We want to check for a geographic coordinate system.  If it is geographic we don't do any transformations on the input data
              //  and want to put the WKT in the PFM header.  We only do this once - we want the very first one.

              if (strlen (wkt_data->pfm_wkt) < 10)
                {
                  QString tst = QString (wkt_data->wkt);
                  if (tst.simplified ().startsWith ("GEOGCS")) strcpy (wkt_data->pfm_wkt, wkt_data->wkt);
                }
            }
          else
            {
              //  Missing WKT even though global_encoding says it's there.

              ask_for_wkt = 1;
            }
        }
      else
        {
          if (lasheader->point_data_format > 5)
            {
              //  No WKT but it is required for point data formats above 5

              ask_for_wkt = 2;
            }
          else
            {
              //  Check for the 34735 record.

              check_34735 ();
            }
        }
    }


  //  LAS 1.3 or below.

  else
    {
      check_34735 ();
    }


  //  If we couldn't find the WKT or we couldn't decipher the 34735 (or it wasn't there etc.) ask for the WKT.

  if (ask_for_wkt)
    {
      //  If we're appending, input_wkt will already be set (and the WKT read in from the FILE.pfm.data/FILE.pfm.wkt file so all we want to do is use
      //  that WKT to set up the coordinate system.  It has already been read in to wkt_data->wkt so we just want to skip the dialog.

      if (!wkt_data->input_wkt)
        {
          wktDialog ();

          wkt_data->input_wkt = NVTrue;
        }
    }


  //  Check to see if we're using WKT

  if (strlen (wkt_data->wkt) > 50)
    {
      QString wktString = QString (wkt_data->wkt);

      if (wktString.contains ("PROJCS"))
        {
          wkt_data->projected = NVTrue;

          OGRSpatialReference SRS;
          char *ppszProj4, *ptr_wkt = wkt_data->wkt;

          SRS.importFromWkt (&ptr_wkt);

          SRS.exportToProj4 (&ppszProj4);

          strcpy (string, ppszProj4);
          OGRFree (ppszProj4);


          if (!(wkt_data->pj_utm = pj_init_plus (string)))
            {
              QMessageBox::critical (parent, "pfmLoader", tr ("%1 %2 %3 %4 - Error initializing UTM projection!").arg
                                     (wkt_data->progname).arg (__FILE__).arg (__FUNCTION__).arg (__LINE__));
              exit (-1);
            }


          strcpy (string, "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");

          OGRSpatialReference pfmSRS;
          ppszProj4 = string;
          pfmSRS.importFromProj4 (ppszProj4);
          pfmSRS.exportToWkt (&ptr_wkt);

          strcpy (wkt_data->pfm_wkt, ptr_wkt);
          OGRFree (ptr_wkt);


          if (!(wkt_data->pj_latlon = pj_init_plus (string)))
            {
              QMessageBox::critical (parent, "pfmLoader", tr ("%1 %2 %3 %4 - Error initializing latlon projection!").arg
                                     (wkt_data->progname).arg (__FILE__).arg (__FUNCTION__).arg (__LINE__));
              exit (-1);
            }
        }
      else
        {
          //  We want to check for a geographic coordinate system.  If it is geographic we don't do any transformations on the input data
          //  and want to put the WKT in the PFM header.  We only do this once - we want the very first one.

          if (strlen (wkt_data->pfm_wkt) < 10)
            {
              QString tst = QString (wkt_data->wkt);
              if (tst.simplified ().startsWith ("GEOGCS")) strcpy (wkt_data->pfm_wkt, wkt_data->wkt);
            }
        }
    }
}



LAS_WKT::~LAS_WKT ()
{
}



void 
LAS_WKT::check_34735 ()
{
  char string[8192];
  int32_t geographic_type = -1;
  int32_t zone = -1;
  uint8_t north = NVTrue;
  int32_t riegl_geo_key_rec = 0;
  int32_t riegl_geo_key_offset = 0;
  int32_t riegl_origin_rec = 0;
  int32_t riegl_origin_offset = 0;
  int32_t riegl_hem_rec = 0;
  int32_t riegl_hem_offset = 0;
  uint8_t VLR_34735 = NVFalse;


  //  Check for a VLR

  if (!lasheader->number_of_variable_length_records)
    {
      //  No VLR for format less than 1.4 or point_data_format < 6 (34735 is required)

      ask_for_wkt = 3;
    }
  else
    {
      for (int32_t k = 0 ; k < (int32_t) lasheader->number_of_variable_length_records ; k++)
        {
          //  GeoKeyDirectoryTag

          if (lasheader->vlrs[k].record_id == 34735)
            {
              VLR_34735 = NVTrue;


              for (int32_t j = 0 ; j < lasheader->vlr_geo_keys->number_of_keys ; j++)
                {
                  //  GTModelTypeGeoKey

                  if (lasheader->vlr_geo_key_entries[j].key_id == 1024)
                    {
                      switch (lasheader->vlr_geo_key_entries[j].value_offset)
                        {
                        case 1:
                          wkt_data->projected = NVTrue;
                          break;

                        case 2:
                          wkt_data->projected = NVFalse;
                          zone = 0;
                          break;

                        default:

                          //  Not projected or Geographic that I understand.

                          ask_for_wkt = 4;
                          break;
                        }
                    }


                  //  GeographicTypeGeoKey

                  if (lasheader->vlr_geo_key_entries[j].key_id == 2048) geographic_type = lasheader->vlr_geo_key_entries[j].value_offset;


                  //  ProjectedCSTypeGeoKey

                  if (lasheader->vlr_geo_key_entries[j].key_id == 3072) zone = lasheader->vlr_geo_key_entries[j].value_offset;


                  //  These are hacks for Riegl data because they didn't set the right fields in the 34735 VLR.

                  if (lasheader->vlr_geo_key_entries[j].key_id == 1026)
                    {
                      riegl_geo_key_rec = lasheader->vlr_geo_key_entries[j].tiff_tag_location;
                      riegl_geo_key_offset = lasheader->vlr_geo_key_entries[j].value_offset;
                    }

                  if (lasheader->vlr_geo_key_entries[j].key_id == 3080)
                    {
                      riegl_origin_rec = lasheader->vlr_geo_key_entries[j].tiff_tag_location;
                      riegl_origin_offset = lasheader->vlr_geo_key_entries[j].value_offset;
                    }

                  if (lasheader->vlr_geo_key_entries[j].key_id == 3073)
                    {
                      riegl_hem_rec = lasheader->vlr_geo_key_entries[j].tiff_tag_location;
                      riegl_hem_offset = lasheader->vlr_geo_key_entries[j].value_offset;
                    }
                }


              //  If we crapped out, break.

              if (ask_for_wkt) break;
            }


          //  Riegl GeoAsciiParamsTag (34737)

          if (lasheader->vlrs[k].record_id == riegl_geo_key_rec || lasheader->vlrs[k].record_id == riegl_hem_rec)
            {
              //  Only do this if we haven't already gotten the geographic type from the GeographicTypeGeoKey

              if (geographic_type == 32767)
                {
                  if (strstr (&lasheader->vlr_geo_ascii_params[riegl_geo_key_offset], "NAD83")) geographic_type = GCS_NAD83;
                  if (strstr (&lasheader->vlr_geo_ascii_params[riegl_geo_key_offset], "WGS84")) geographic_type = GCS_WGS_84;

                  if (strstr (&lasheader->vlr_geo_ascii_params[riegl_hem_offset], "UTM_North"))
                    {
                      north = NVTrue;
                    }
                  else
                    {
                      north = NVFalse;
                    }
                }
            }


          //  Riegl GeoDoubleParamsTag (34736)

          if (lasheader->vlrs[k].record_id == riegl_origin_rec)
            {
              //  Only do this if we haven't already gotten the zone from the ProjectedCSTypeGeoKey

              if (zone == 32767)
                {
                  zone = (int32_t) (31.0 + lasheader->vlr_geo_double_params[riegl_origin_offset] / 6.0);
                  if (zone >= 61) zone = 60;	
                  if (zone <= 0) zone = 1;
                }
            }
        }
    }


  //  No 34735 VLR

  if (!VLR_34735) ask_for_wkt = 5;


  //  So far so good...

  if (!ask_for_wkt)
    {
      if (wkt_data->projected)
        {
          //  Some people don't put in the GeographicTypeGeoKey and just depend on the ProjectedCSTypeGeoKey.  If this happens we'll try
          //  to set the GeographicTypeGeoKey from the ProjectedCSTypeGeoKey.

          if (geographic_type < 0)
            {
              if (zone >= PCS_NAD83_UTM_zone_3N && zone <= PCS_NAD83_UTM_zone_23N) geographic_type = GCS_NAD83;
              if ((zone >= PCS_WGS84_UTM_zone_1N && zone <= PCS_WGS84_UTM_zone_60N) ||
                  (zone >= PCS_WGS84_UTM_zone_1S && zone <= PCS_WGS84_UTM_zone_60S)) geographic_type = GCS_WGS_84;


              //  Some people just assume that it's WGS_84 so, if we didn't get any datum, set it to WGS_84.

              if (geographic_type < 0) geographic_type = GCS_WGS_84;
            }


          //  If we got our zone from the Riegl extra records we don't need to decipher the standard zone names/numbers.

          if (zone < 0 || zone > 60)
            {
              if (geographic_type == GCS_NAD83 && zone >= PCS_NAD83_UTM_zone_3N && zone <= PCS_NAD83_UTM_zone_23N)
                {
                  zone %= 26900;
                  north = NVTrue;
                }
              else if (geographic_type == GCS_WGS_84 && zone >= PCS_WGS84_UTM_zone_1N && zone <= PCS_WGS84_UTM_zone_60N)
                {
                  zone %= 32600;
                  north = NVTrue;
                }
              else if (geographic_type == GCS_WGS_84 && zone >= PCS_WGS84_UTM_zone_1S && zone <= PCS_WGS84_UTM_zone_60S)
                {
                  zone %= 32700;
                  north = NVFalse;
                }
              else
                {
                  //  Not NAD83 or WGS84 UTM

                  ask_for_wkt = 6;
                }
            }
        }
    }


  if (!ask_for_wkt)
    {
      //  Please note that I am not too concerned about getting the correct datum and/or ellipsoid or vertical units.  The reason
      //  being that, for the purposes of PFM, we only want self-consistent data so that we can edit it.  We don't save the
      //  converted positions from PFM.  We only save the status (mostly just valid/invalid) and/or classification.

      if (wkt_data->projected)
        {
          if (north)
            {
              if (geographic_type == GCS_NAD83)
                {
                  sprintf (string, "+proj=utm +zone=%d +ellps=GRS80 +datum=NAD83", zone);
                }
              else
                {
                  sprintf (string, "+proj=utm +zone=%d +ellps=WGS84 +datum=WGS84", zone);
                }
            }
          else
            {
              if (geographic_type == GCS_NAD83)
                {
                  sprintf (string, "+proj=utm +zone=%d +ellps=GRS80 +datum=NAD83 +south", zone);
                }
              else
                {
                  sprintf (string, "+proj=utm +zone=%d +ellps=WGS84 +datum=WGS84 +south", zone);
                }
            }

          if (!(wkt_data->pj_utm = pj_init_plus (string)))
            {
              QMessageBox::critical (parent, "pfmLoader", tr ("%1 %2 %3 %4 - Error initializing UTM projection!").arg
                                     (wkt_data->progname).arg (__FILE__).arg (__FUNCTION__).arg (__LINE__));
              exit (-1);
            }


          OGRSpatialReference pfmSRS;
          char *ppszProj4 = string, *ptr_wkt;

          if (geographic_type == GCS_NAD83)
            {
              sprintf (string, "+proj=longlat +ellps=GRS80 +datum=NAD83 +no_defs");

              pfmSRS.importFromProj4 (ppszProj4);
              pfmSRS.exportToWkt (&ptr_wkt);

              strcpy (wkt_data->pfm_wkt, ptr_wkt);
              OGRFree (ptr_wkt);
            }
          else
            {
              sprintf (string, "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");

              pfmSRS.importFromProj4 (ppszProj4);
              pfmSRS.exportToWkt (&ptr_wkt);

              strcpy (wkt_data->pfm_wkt, ptr_wkt);
              OGRFree (ptr_wkt);
            }

          if (!(wkt_data->pj_latlon = pj_init_plus (string)))
            {
              QMessageBox::critical (parent, "pfmLoader", tr ("%1 %2 %3 %4 - Error initializing latlon projection!").arg
                                     (wkt_data->progname).arg (__FILE__).arg (__FUNCTION__).arg (__LINE__));
              exit (-1);
            }
        }
    }
}



void 
LAS_WKT::wktDialog ()
{
  recent_id = -1;


  if (wktD) wktD->close ();


  wktD = new QDialog (this, (Qt::WindowFlags) Qt::WA_DeleteOnClose);
  wktD->setWindowTitle (tr ("%1 Well-known Text").arg (shortName));

  wktD->resize (800, 500);


  QVBoxLayout *vbox = new QVBoxLayout (wktD);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  QString msg = "<br>";


  switch (ask_for_wkt)
    {
      //  LAS 1.4 - Missing WKT even though global_encoding says it's there.

    case 1:
      msg = tr ("The LAS file <b>%1</b> is version 1.4.  The <b>global_encoding</b> field of the LAS header indicates that "
                "there is a Coordinate System Well-known Text (WKT) record in the file however, it appears to be missing.  This "
                "means that the LAS file does not meet the LAS 1.4 specification.").arg (filename);
      break;


      //  LAS 1.4 - No WKT but it is required for point data formats above 5

    case 2:
      msg = tr ("The LAS file <b>%1</b> is version 1.4.  The <b>global_encoding</b> field of the LAS header indicates that "
                "there is not a Coordinate System Well-known Text (WKT) record in the file.  Since the Point Data Record Format "
                "for this file is greater than 5, the use of the WKT record is required.  This LAS file does not meet the LAS "
                "1.4 specification.").arg (filename);
      break;


      //  No VLRs for format less than 1.4 (34735 is required)

    case 3:
      msg = tr ("The LAS file <b>%1</b> is version 1.%2.  There are no Variable Length Records "
                "(VLR) defined for this file.  According to the LAS 1.%2 specification, a 34735 "
                "(GeoKeyDirectoryTag) VLR is required.").arg (filename).arg (lasheader->version_minor);
      break;


      //  Not projected or geographic GTModelTypeGeoKey.

    case 4:
      msg = tr ("The LAS file <b>%1</b> has a 34735 Variable Length Record (VLR) but the <b>GTModelTypeGeoKey</b> isn't "
                "<b>ModelTypeProjected</b> or <b>ModelTypeGeographic</b>.  <b>%2</b> only handles those two types.").arg (filename).arg (shortName);
      break;


      //  No 34735 VLR

    case 5:
      msg = tr ("The LAS file <b>%1</b> is version 1.%2.  The required 34735 (GeoKeyDirectoryTag) Variable Length Record (VLR) is missing.  According to the LAS 1.%2 "
                "specification, a 34735 (GeoKeyDirectoryTag) VLR is required").arg (filename).arg (lasheader->version_minor);
      break;


      //  Not NAD83 or WGS84 projected

    case 6:
      msg = tr ("The LAS file <b>%1</b> has a 34735 Variable Length Record (VLR) with the <b>GTModelTypeGeoKey</b> set to "
                "<b>ModelTypeProjected</b>.  <b>%2</b> has determined that the <b>Geographic CS Type</b> is not <b>GCS_NAD83</b> "
                "(4269) or <b>GCS_WGS_84</b> (4326).  <b>%2</b> only handles those two types.").arg (filename).arg (shortName);
      break;
    }

  msg += tr ("<br><br>Due to the above listed condition <b>%1</b> is unable to convert positions for the input LAS data to "
             "latitude and longitude.  If you would like to know why the positions are being converted to latitude and longitude you can read "
             "<a href=\"http://pfmabe.software/a-note-about-utm-vs-latlon/\">this article</a> on pfmabe.software.<br><br>"
             "This dialog will allow you to enter Well-known Text (WKT) to be used to convert the positions.  WKT for many different areas "
             "is available on <a href=\"http://spatialreference.org/\">spatialreference.org</a> (for example, "
             "<a href=\"http://spatialreference.org/ref/epsg/2451/\">EPSG:2451</a>).  Only GEOGCS or PROJCS WKT is supported.<br><br>"
             "Simply copy the <b>OGC WKT</b> that best matches the data in the above LAS file and paste it into the text box below.<br><br>"
             "Do not be overly concerned about whether the WKT is exactly correct for your data.  The Area-Based Editor (ABE) is only used "
             "to modify status (e.g. <b>Withheld</b>, <b>Synthetic</b>, <b>Key-point</b>) and/or classification (e.g. <b>Low Vegetation</b>, "
             "<b>Building</b>, <b>Road Surface</b>) in the LAS files (when unloaded from ABE).  The positions in the LAS files are not "
             "modified.  Because of this, the absolute accuracy of the positions in ABE is not really important.  It is much more important "
             "that the positions are accurate relative to each other and fairly accurate in relation to the globe (in order to use Google Earth for "
             "external reference).<br><br>"
             "If the data in the LAS file is projected, <b>%1</b> will convert the input positions to "
             "WGS84 latitude and longitude.<br><br>"
             "It is assumed that all LAS files being read for the current run are from the same general area and the WKT used for this file "
             "will be used for all subsequent files.<br><br>"
             "If you do not enter WKT data and click the <b>Close</b> button, <b>%1</b> will be terminated.").arg (shortName);

  QLabel *wktLabel = new QLabel (this);
  wktLabel->setToolTip (tr ("Click <b>Help</b> button then click here for an even more verbose explanation"));
  wktLabel->setWhatsThis (wktLabelText);
  wktLabel->setWordWrap (true);
  wktLabel->setOpenExternalLinks (true);
  wktLabel->setTextFormat (Qt::RichText);

  wktLabel->setText (msg);


  vbox->addWidget (wktLabel);


  QHBoxLayout *wktLayout = new QHBoxLayout (0);
  vbox->addLayout (wktLayout);

  recentWKT = new QComboBox (this);
  recentWKT->setToolTip (tr ("Select a previously used WKT string (hover to see WKT in tool tip)"));
  recentWKT->setWhatsThis (tr ("Select a previously used WKT string (hover to see WKT in tool tip)"));
  recentWKT->setEditable (false);
  for (int32_t i = 0 ; i < 10 ; i++)
    {
      if (!wkt_data->wktString[i].isEmpty ())
        {
          //  Make the WKT human readable

          OGRSpatialReference SRS;
          char wkt[8192], pretty[8192];
          strcpy (wkt, wkt_data->wktString[i].toLatin1 ());
          char *ppszPretty, *ptr_wkt = wkt;

          SRS.importFromWkt (&ptr_wkt);

          SRS.exportToPrettyWkt (&ppszPretty);

          strcpy (pretty, ppszPretty);
          OGRFree (ppszPretty);

          QString prettyToolTip = QString (pretty);


          QString tmp;

          if (wkt_data->wktString[i].contains ("PROJCS"))
            {
              tmp = wkt_data->wktString[i].section ("PROJCS[", 1, 1).section (',', 0, 0);
            }
          else
            {
              tmp = wkt_data->wktString[i].section ("GEOGCS[", 1, 1).section (',', 0, 0);
            }

          recentWKT->addItem (tmp);
          recentWKT->setItemData (i, prettyToolTip, Qt::ToolTipRole);
        }
    }
  connect (recentWKT, SIGNAL (activated (int)), this, SLOT (slotRecentWKTActivated (int)));
  wktLayout->addWidget (recentWKT);



  wktText = new QLineEdit (this);
  wktText->setToolTip (tr ("Enter the Well-known Text (WKT) for the LAS input file"));
  wktText->setWhatsThis (tr ("Enter the Well-known Text (WKT) coordinate system definition for the LAS input file"));
  wktText->setPlaceholderText (tr ("Well-known Text (WKT)"));
  connect (wktText, SIGNAL (textEdited (QString)), this, SLOT (slotWktTextEdited (QString)));
  wktLayout->addWidget (wktText);


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (this);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), wktD);
  closeButton->setToolTip (tr ("Discard WKT text, close this dialog, and exit the program"));
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseWKT ()));
  actions->addWidget (closeButton);

  QPushButton *acceptButton = new QPushButton (tr ("Accept"), wktD);
  acceptButton->setToolTip (tr ("Accept the WKT text that has been entered, close this dialog, and process the files"));
  connect (acceptButton, SIGNAL (clicked ()), this, SLOT (slotAcceptWKT ()));
  actions->addWidget (acceptButton);


  //  Put the dialog in the middle of the screen.

  wktD->move (x () + width () / 2 - wktD->width () / 2, y () + height () / 2 - wktD->height () / 2);


  //  Use exec so that the main program will wait for this to get done.

  wktD->exec ();
}



void 
LAS_WKT::slotRecentWKTActivated (int id)
{
  recent_id = id;

  wktText->setText (wkt_data->wktString[id]);
}



void 
LAS_WKT::slotWktTextEdited (QString text __attribute__ ((unused)))
{
  recent_id = -1;
}



void
LAS_WKT::slotHelp ()
{
  QWhatsThis::enterWhatsThisMode ();
}



void 
LAS_WKT::slotAcceptWKT ()
{
  QString text = wktText->text ().simplified ();

  if (!text.isEmpty ())
    {
      if ((!text.startsWith ("GEOGCS")) && (!text.startsWith ("PROJCS")))
        {
          QMessageBox::warning (parent, tr ("%1 Well-known Text").arg (shortName), tr ("The WKT data entered does not appear to be correct and/or supported."));

          wktText->setText ("");
        }
      else
        {
          strcpy (wkt_data->wkt, text.toLatin1 ());



          //  Check to see if we used one of the recently used WKT strings.

          if (recent_id < 0)
            {
              //  Add the new WKT string to the top of the recently used list.

              for (int32_t i = 8 ; i >= 0 ; i--) wkt_data->wktString[i + 1] = wkt_data->wktString[i];
              wkt_data->wktString[0] = text;
            }
          else
            {
              //  If the user selected the first one we don't need to re-order.

              if (recent_id > 0)
                {
                  //  Re-order the recently used WKT strings.

                  for (int32_t i = recent_id - 1 ; i >= 0 ; i--) wkt_data->wktString[i + 1] = wkt_data->wktString[i];
                  wkt_data->wktString[0] = text;
                }
            }


          //  We need to save the WKT strings to the globalABE settings file.  This file is used to store things that
          //  may be used by multiple ABE programs.

#ifdef NVWIN3X
          QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/" + "globalABE.ini";
#else
          QString ini_file = QString (getenv ("HOME")) + "/ABE.config/" + "globalABE.ini";
#endif

          QSettings settings (ini_file, QSettings::IniFormat);
          settings.beginGroup ("globalABE");


          for (int32_t i = 0 ; i < 10 ; i++)
            {
              QString name = tr ("Recent WKT %1").arg (i);
              settings.setValue (name, wkt_data->wktString[i]);
            }

          settings.endGroup ();


          wktD->close ();
        }
    }
  else
    {
      QMessageBox::warning (parent, tr ("%1 Well-known Text").arg (shortName), tr ("The WKT text input field is empty."));
    }
}



void 
LAS_WKT::slotCloseWKT ()
{
  //  Just a friendly message for pfmLoad if that's how this was started.  Do not modify!

  fprintf (stderr, "LAS_WKT closed by user\n");
  fflush (stderr);


  wktD->close ();


  exit (-1);
}
