
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



#include "attributes.hpp"



/*********************************************  IMPORTANT NOTES  **************************************************\

    Don't make the descriptions longer than 29 characters or you'll blow out the PFM attribute name length.

    To add to these you must change the ???_ATTRIBUTES definitions in attributes.hpp

    Because I always forget this - NEVER USE SLASHES (/) IN ANY OF THESE STRINGS!  If you 
    read the documentation for QSettings you'll understand why.

\*********************************************  IMPORTANT NOTES  **************************************************/



//  Time attribute

void setTimeAttributes (ATTR_BOUNDS *time_attribute)
{
  memset (time_attribute, 0, sizeof (ATTR_BOUNDS));

  strcpy (time_attribute->name, "Time (POSIX minutes)");
  time_attribute->scale = 1.0;
  time_attribute->min = (int32_t) ABE_NULL_TIME;
  time_attribute->max = (int32_t) -ABE_NULL_TIME;
}



//  GSF attributes

void setGSFAttributes (ATTR_BOUNDS *gsf_attribute)
{
  memset (gsf_attribute, 0, GSF_ATTRIBUTES * sizeof (ATTR_BOUNDS));

  strcpy (gsf_attribute[0].name, "GSF Heading");
  gsf_attribute[0].scale = 100.0;
  gsf_attribute[0].min = 0.0;
  gsf_attribute[0].max = 360.0;
  strcpy (gsf_attribute[1].name, "GSF Pitch");
  gsf_attribute[1].scale = 100.0;
  gsf_attribute[1].min = -30.0;
  gsf_attribute[1].max = 30.0;
  strcpy (gsf_attribute[2].name,"GSF Roll");
  gsf_attribute[2].scale = 100.0;
  gsf_attribute[2].min = -50.0;
  gsf_attribute[2].max = 50.0;
  strcpy (gsf_attribute[3].name,"GSF Heave");
  gsf_attribute[3].scale = 100.0;
  gsf_attribute[3].min = -25.0;
  gsf_attribute[3].max = 25.0;
  strcpy (gsf_attribute[4].name,"GSF Course");
  gsf_attribute[4].scale = 100.0;
  gsf_attribute[4].min = 0.0;
  gsf_attribute[4].max = 360.0;
  strcpy (gsf_attribute[5].name,"GSF Speed");
  gsf_attribute[5].scale = 100.0;
  gsf_attribute[5].min = 0.0;
  gsf_attribute[5].max = 30.0;
  strcpy (gsf_attribute[6].name,"GSF Height above ellipsoid");
  gsf_attribute[6].scale = 1000.0;
  gsf_attribute[6].min = -400.0;
  gsf_attribute[6].max = 400.0;
  strcpy (gsf_attribute[7].name,"GSF Ellipsoid datum distance");
  gsf_attribute[7].scale = 1000.0;
  gsf_attribute[7].min = -400.0;
  gsf_attribute[7].max = 400.0;
  strcpy (gsf_attribute[8].name,"GSF Tide correction");
  gsf_attribute[8].scale = 1000.0;
  gsf_attribute[8].min = -20.0;
  gsf_attribute[8].max = 20.0;
  strcpy (gsf_attribute[9].name,"GSF Tide type");
  gsf_attribute[9].scale = 1.0;
  gsf_attribute[9].min = 0;
  gsf_attribute[9].max = 4;
  strcpy (gsf_attribute[10].name,"GSF Across track distance");
  gsf_attribute[10].scale = 100.0;
  gsf_attribute[10].min = -50000.0;
  gsf_attribute[10].max = 50000.0;
  strcpy (gsf_attribute[11].name,"GSF Along track distance");
  gsf_attribute[11].scale = 100.0;
  gsf_attribute[11].min = -20000.0;
  gsf_attribute[11].max = 20000.0;
  strcpy (gsf_attribute[12].name,"GSF Travel time");
  gsf_attribute[12].scale = 1000.0;
  gsf_attribute[12].min = 0.0;
  gsf_attribute[12].max = 20.0;
  strcpy (gsf_attribute[13].name,"GSF Beam angle");
  gsf_attribute[13].scale = 100.0;
  gsf_attribute[13].min = -180.0;
  gsf_attribute[13].max = 180.0;
  strcpy (gsf_attribute[14].name,"GSF Beam angle forward");
  gsf_attribute[14].scale = 100.0;
  gsf_attribute[14].min = -180.0;
  gsf_attribute[14].max = 180.0;
  strcpy (gsf_attribute[15].name,"GSF Mean calibrated amp.");
  gsf_attribute[15].scale = 100.0;
  gsf_attribute[15].min = -200.0;
  gsf_attribute[15].max = 200.0;
  strcpy (gsf_attribute[16].name,"GSF Mean relative amp.");
  gsf_attribute[16].scale = 100.0;
  gsf_attribute[16].min = -200.0;
  gsf_attribute[16].max = 200.0;
  strcpy (gsf_attribute[17].name,"GSF Echo width");
  gsf_attribute[17].scale = 100.0;
  gsf_attribute[17].min = -200.0;
  gsf_attribute[17].max = 200.0;
  strcpy (gsf_attribute[18].name,"GSF Simrad Quality factor");
  gsf_attribute[18].scale = 1.0;
  gsf_attribute[18].min = 0;
  gsf_attribute[18].max = 255;
  strcpy (gsf_attribute[19].name,"GSF Signal to noise");
  gsf_attribute[19].scale = 100.0;
  gsf_attribute[19].min = 0.0;
  gsf_attribute[19].max = 100.0;
  strcpy (gsf_attribute[20].name,"GSF Receive heave");
  gsf_attribute[20].scale = 100.0;
  gsf_attribute[20].min = -100.0;
  gsf_attribute[20].max = 100.0;
  strcpy (gsf_attribute[21].name,"GSF Depth error");
  gsf_attribute[21].scale = 100.0;
  gsf_attribute[21].min = -1000.0;
  gsf_attribute[21].max = 1000.0;
  strcpy (gsf_attribute[22].name,"GSF Across track error");
  gsf_attribute[22].scale = 100.0;
  gsf_attribute[22].min = -5000.0;
  gsf_attribute[22].max = 5000.0;
  strcpy (gsf_attribute[23].name,"GSF Along track error");
  gsf_attribute[23].scale = 100.0;
  gsf_attribute[23].min = -5000.0;
  gsf_attribute[23].max = 5000.0;
  strcpy (gsf_attribute[24].name,"GSF Reson quality flag");
  gsf_attribute[24].scale = 1.0;
  gsf_attribute[24].min = 0;
  gsf_attribute[24].max = 255;
  strcpy (gsf_attribute[25].name,"GSF Beam flag");
  gsf_attribute[25].scale = 1.0;
  gsf_attribute[25].min = 0;
  gsf_attribute[25].max = 255;
  strcpy (gsf_attribute[26].name,"GSF Beam angle forward");
  gsf_attribute[26].scale = 100.0;
  gsf_attribute[26].min = -90.0;
  gsf_attribute[26].max = 90.0;
  strcpy (gsf_attribute[27].name,"GSF Transit sector");
  gsf_attribute[27].scale = 1.0;
  gsf_attribute[27].min = 0;
  gsf_attribute[27].max = 65535;
  strcpy (gsf_attribute[28].name,"GSF Detection info");
  gsf_attribute[28].scale = 1.0;
  gsf_attribute[28].min = 0;
  gsf_attribute[28].max = 65535;
  strcpy (gsf_attribute[29].name,"GSF Incident beam adj.");
  gsf_attribute[29].scale = 100.0;
  gsf_attribute[29].min = -90.0;
  gsf_attribute[29].max = 90.0;
  strcpy (gsf_attribute[30].name,"GSF System cleaning");
  gsf_attribute[30].scale = 1.0;
  gsf_attribute[30].min = 0;
  gsf_attribute[30].max = 65535;
  strcpy (gsf_attribute[31].name,"GSF Doppler correction");
  gsf_attribute[31].scale = 100.0;
  gsf_attribute[31].min = -5000.0;
  gsf_attribute[31].max = 5000.0;
}



  //  HOF attributes

void setHOFAttributes (ATTR_BOUNDS *hof_attribute)
{
  memset (hof_attribute, 0, HOF_ATTRIBUTES * sizeof (ATTR_BOUNDS));

  strcpy (hof_attribute[0].name, "HOF Haps version");
  hof_attribute[0].scale = 1.0;
  hof_attribute[0].min = 0.0;
  hof_attribute[0].max = 10.0;
  strcpy (hof_attribute[1].name, "HOF Position confidence");
  hof_attribute[1].scale = 1.0;
  hof_attribute[1].min = -100.0;
  hof_attribute[1].max = 100.0;
  strcpy (hof_attribute[2].name, "HOF Status");
  hof_attribute[2].scale = 1.0;
  hof_attribute[2].min = 0.0;
  hof_attribute[2].max = 126.0;
  strcpy (hof_attribute[3].name, "HOF Suggested DKS");
  hof_attribute[3].scale = 1.0;
  hof_attribute[3].min = 0.0;
  hof_attribute[3].max = 126.0;
  strcpy (hof_attribute[4].name, "HOF Suspect status");
  hof_attribute[4].scale = 1.0;
  hof_attribute[4].min = 0.0;
  hof_attribute[4].max = 126.0;
  strcpy (hof_attribute[5].name, "HOF Tide status");
  hof_attribute[5].scale = 1.0;
  hof_attribute[5].min = 0.0;
  hof_attribute[5].max = 4.0;
  strcpy (hof_attribute[6].name, "HOF Abbreviated depth conf.");
  hof_attribute[6].scale = 1.0;
  hof_attribute[6].min = -100.0;
  hof_attribute[6].max = 100.0;
  strcpy (hof_attribute[7].name, "HOF Abbreviated sec dep conf.");
  hof_attribute[7].scale = 1.0;
  hof_attribute[7].min = -100.0;
  hof_attribute[7].max = 100.0;
  strcpy (hof_attribute[8].name, "HOF Data type");
  hof_attribute[8].scale = 1.0;
  hof_attribute[8].min = 0.0;
  hof_attribute[8].max = 1.0;
  strcpy (hof_attribute[9].name, "HOF Land mode");
  hof_attribute[9].scale = 1.0;
  hof_attribute[9].min = 0.0;
  hof_attribute[9].max = 1.0;
  strcpy (hof_attribute[10].name, "HOF Classification status");
  hof_attribute[10].scale = 1.0;
  hof_attribute[10].min = 0.0;
  hof_attribute[10].max = 4096.0;
  strcpy (hof_attribute[11].name, "HOF Wave height");
  hof_attribute[11].scale = 100.0;
  hof_attribute[11].min = -25.0;
  hof_attribute[11].max = 25.0;
  strcpy (hof_attribute[12].name, "HOF Elevation");
  hof_attribute[12].scale = 1000.0;
  hof_attribute[12].min = -1000.0;
  hof_attribute[12].max = 1000.0;
  strcpy (hof_attribute[13].name, "HOF Topo value");
  hof_attribute[13].scale = 1000.0;
  hof_attribute[13].min = -1000.0;
  hof_attribute[13].max = 1000.0;
  strcpy (hof_attribute[14].name, "HOF Altitude");
  hof_attribute[14].scale = 100.0;
  hof_attribute[14].min = 0.0;
  hof_attribute[14].max = 1000.0;
  strcpy (hof_attribute[15].name, "HOF KGPS topo value");
  hof_attribute[15].scale = 100.0;
  hof_attribute[15].min = -1000.0;
  hof_attribute[15].max = 1000.0;
  strcpy (hof_attribute[16].name, "HOF KGPS datum value");
  hof_attribute[16].scale = 100.0;
  hof_attribute[16].min = -1000.0;
  hof_attribute[16].max = 1000.0;
  strcpy (hof_attribute[17].name, "HOF KGPS water level");
  hof_attribute[17].scale = 100.0;
  hof_attribute[17].min = -1000.0;
  hof_attribute[17].max = 1000.0;
  strcpy (hof_attribute[18].name, "HOF K value (TBD)");
  hof_attribute[18].scale = 1.0;
  hof_attribute[18].min =0.0;
  hof_attribute[18].max = 126.0;
  strcpy (hof_attribute[19].name, "HOF Intensity (TBD)");
  hof_attribute[19].scale = 1.0;
  hof_attribute[19].min = 0.0;
  hof_attribute[19].max = 1000.0;
  strcpy (hof_attribute[20].name, "HOF Bottom confidence");
  hof_attribute[20].scale = 1.0;
  hof_attribute[20].min = 0.0;
  hof_attribute[20].max = 126.0;
  strcpy (hof_attribute[21].name, "HOF Secondary depth conf.");
  hof_attribute[21].scale = 1.0;
  hof_attribute[21].min = 0.0;
  hof_attribute[21].max = 126.0;
  strcpy (hof_attribute[22].name, "HOF Nadir angle");
  hof_attribute[22].scale = 100.0;
  hof_attribute[22].min = -45.0;
  hof_attribute[22].max = 45.0;
  strcpy (hof_attribute[23].name, "HOF Scanner azimuth");
  hof_attribute[23].scale = 100.0;
  hof_attribute[23].min = -180.0;
  hof_attribute[23].max = 180.0;
  strcpy (hof_attribute[24].name, "HOF Surface APD FOM");
  hof_attribute[24].scale = 1000.0;
  hof_attribute[24].min = 0.0;
  hof_attribute[24].max = 1000.0;
  strcpy (hof_attribute[25].name, "HOF Surface IR FOM");
  hof_attribute[25].scale = 1000.0;
  hof_attribute[25].min = 0.0;
  hof_attribute[25].max = 1000.0;
  strcpy (hof_attribute[26].name, "HOF Surface Raman FOM");
  hof_attribute[26].scale = 1000.0;
  hof_attribute[26].min = 0.0;
  hof_attribute[26].max = 1000.0;
  strcpy (hof_attribute[27].name, "HOF Coded depth conf.");
  hof_attribute[27].scale = 1.0;
  hof_attribute[27].min = 0.0;
  hof_attribute[27].max = 1000000.0;
  strcpy (hof_attribute[28].name, "HOF Coded sec. depth conf.");
  hof_attribute[28].scale = 1.0;
  hof_attribute[28].min = 0.0;
  hof_attribute[28].max = 1000000.0;
  strcpy (hof_attribute[29].name, "HOF Warnings (1)");
  hof_attribute[29].scale = 1.0;
  hof_attribute[29].min = 0.0;
  hof_attribute[29].max = 32766.0;
  strcpy (hof_attribute[30].name, "HOF Warnings (2)");
  hof_attribute[30].scale = 1.0;
  hof_attribute[30].min = 0.0;
  hof_attribute[30].max = 32766.0;
  strcpy (hof_attribute[31].name, "HOF Warnings (3)");
  hof_attribute[31].scale = 1.0;
  hof_attribute[31].min = 0.0;
  hof_attribute[31].max = 32766.0;
  strcpy (hof_attribute[32].name, "HOF First calculated bfom*10");
  hof_attribute[32].scale = 1.0;
  hof_attribute[32].min = 0.0;
  hof_attribute[32].max = 1022.0;
  strcpy (hof_attribute[33].name, "HOF Second calculated bfom*10");
  hof_attribute[33].scale = 1.0;
  hof_attribute[33].min = 0.0;
  hof_attribute[33].max = 1022.0;
  strcpy (hof_attribute[34].name, "HOF First bottom run req.");
  hof_attribute[34].scale = 1.0;
  hof_attribute[34].min = 0.0;
  hof_attribute[34].max = 15.0;
  strcpy (hof_attribute[35].name, "HOF Second bottom run req.");
  hof_attribute[35].scale = 1.0;
  hof_attribute[35].min = 0.0;
  hof_attribute[35].max = 15.0;
  strcpy (hof_attribute[36].name, "HOF Primary return bin");
  hof_attribute[36].scale = 1.0;
  hof_attribute[36].min = 0.0;
  hof_attribute[36].max = 510.0;
  strcpy (hof_attribute[37].name, "HOF Secondary return bin");
  hof_attribute[37].scale = 1.0;
  hof_attribute[37].min = 0.0;
  hof_attribute[37].max = 510.0;
  strcpy (hof_attribute[38].name, "HOF PMT bottom bin");
  hof_attribute[38].scale = 1.0;
  hof_attribute[38].min = 0.0;
  hof_attribute[38].max = 510.0;
  strcpy (hof_attribute[39].name, "HOF PMT secondary bottom bin");
  hof_attribute[39].scale = 1.0;
  hof_attribute[39].min = 0.0;
  hof_attribute[39].max = 510.0;
  strcpy (hof_attribute[40].name, "HOF APD bottom bin");
  hof_attribute[40].scale = 1.0;
  hof_attribute[40].min = 0.0;
  hof_attribute[40].max = 510.0;
  strcpy (hof_attribute[41].name, "HOF APD secondary bottom bin");
  hof_attribute[41].scale = 1.0;
  hof_attribute[41].min = 0.0;
  hof_attribute[41].max = 510.0;
  strcpy (hof_attribute[42].name, "HOF Bottom channel");
  hof_attribute[42].scale = 1.0;
  hof_attribute[42].min = 0.0;
  hof_attribute[42].max = 2.0;
  strcpy (hof_attribute[43].name, "HOF Secondary bottom channel");
  hof_attribute[43].scale = 1.0;
  hof_attribute[43].min = 0.0;
  hof_attribute[43].max = 2.0;
  strcpy (hof_attribute[44].name, "HOF Surface bin from APD");
  hof_attribute[44].scale = 1.0;
  hof_attribute[44].min = 0.0;
  hof_attribute[44].max = 510.0;
  strcpy (hof_attribute[45].name, "HOF Surface bin from IR");
  hof_attribute[45].scale = 1.0;
  hof_attribute[45].min = 0.0;
  hof_attribute[45].max = 510.0;
  strcpy (hof_attribute[46].name, "HOF Surface bin from Raman");
  hof_attribute[46].scale = 1.0;
  hof_attribute[46].min = 0.0;
  hof_attribute[46].max = 510.0;
  strcpy (hof_attribute[47].name, "HOF Surface channel use");
  hof_attribute[47].scale = 1.0;
  hof_attribute[47].min = 0.0;
  hof_attribute[47].max = 2.0;
}


 
//  TOF attributes

void setTOFAttributes (ATTR_BOUNDS *tof_attribute)
{
  memset (tof_attribute, 0, TOF_ATTRIBUTES * sizeof (ATTR_BOUNDS));

  strcpy (tof_attribute[0].name, "TOF Classification status");
  tof_attribute[0].scale = 1.0;
  tof_attribute[0].min = 0;
  tof_attribute[0].max = 4094;
  strcpy (tof_attribute[1].name, "TOF Altitude");
  tof_attribute[1].scale = 100.0;
  tof_attribute[1].max = 0.0;
  tof_attribute[1].min = 1000.0;
  strcpy (tof_attribute[2].name, "TOF First return intensity");
  tof_attribute[2].scale = 1.0;
  tof_attribute[2].min = 0;
  tof_attribute[2].max = 126;
  strcpy (tof_attribute[3].name, "TOF Last return intensity");
  tof_attribute[3].scale = 1.0;
  tof_attribute[3].min = 0;
  tof_attribute[3].max = 126;
  strcpy (tof_attribute[4].name, "TOF First return confidence");
  tof_attribute[4].scale = 1.0;
  tof_attribute[4].min = 0;
  tof_attribute[4].max = 126;
  strcpy (tof_attribute[5].name, "TOF Last return confidence");
  tof_attribute[5].scale = 1.0;
  tof_attribute[5].min = 0;
  tof_attribute[5].max = 126;
  strcpy (tof_attribute[6].name, "TOF Nadir angle");
  tof_attribute[6].scale = 100.0;
  tof_attribute[6].min = -45.0;
  tof_attribute[6].max = 45.0;
  strcpy (tof_attribute[7].name, "TOF Scanner azimuth");
  tof_attribute[7].scale = 100.0;
  tof_attribute[7].min = -180.0;
  tof_attribute[7].max = 180.0;
  strcpy (tof_attribute[8].name, "TOF Position confidence");
  tof_attribute[8].scale = 1.0;
  tof_attribute[8].min = 0;
  tof_attribute[8].max = 126;
}



//  CZMIL attributes

void setCZMILAttributes (ATTR_BOUNDS *czmil_attribute, QString *czmil_flag_name)
{
  memset (czmil_attribute, 0, CZMIL_ATTRIBUTES * sizeof (ATTR_BOUNDS));

  strcpy (czmil_attribute[0].name, "CZMIL Reflectance");
  czmil_attribute[0].scale = 10000.0;
  czmil_attribute[0].min = 0.0;
  czmil_attribute[0].max = 1.0;
  strcpy (czmil_attribute[1].name, "CZMIL Classification");
  czmil_attribute[1].scale = 1.0;
  czmil_attribute[1].min = 0;
  czmil_attribute[1].max = 254;
  strcpy (czmil_attribute[2].name, "CZMIL Off nadir angle");
  czmil_attribute[2].scale = 10000.0;
  czmil_attribute[2].min = -105.0;
  czmil_attribute[2].max = 105.0;
  strcpy (czmil_attribute[3].name, "CZMIL Water level");
  czmil_attribute[3].scale = 1000.0;
  czmil_attribute[3].min = -1000.0;
  czmil_attribute[3].max = 1000.0;
  strcpy (czmil_attribute[4].name, "CZMIL Local datum offset");
  czmil_attribute[4].scale = 1000.0;
  czmil_attribute[4].min = -1000.0;
  czmil_attribute[4].max = 1000.0;
  strcpy (czmil_attribute[5].name, "CZMIL Interest point");
  czmil_attribute[5].scale = 100.0;
  czmil_attribute[5].min = 0.0;
  czmil_attribute[5].max = (float) CZMIL_MAX_PACKETS * 64.0;
  strcpy (czmil_attribute[6].name, "CZMIL Interest point rank");
  czmil_attribute[6].scale = 1.0;
  czmil_attribute[6].min = 0;
  czmil_attribute[6].max = CZMIL_MAX_RETURNS;
  strcpy (czmil_attribute[7].name, "CZMIL Channel number");
  czmil_attribute[7].scale = 1.0;
  czmil_attribute[7].min = 0;
  czmil_attribute[7].max = 9;
  strcpy (czmil_attribute[8].name, "CZMIL Return number");
  czmil_attribute[8].scale = 1.0;
  czmil_attribute[8].min = 0;
  czmil_attribute[8].max = CZMIL_MAX_RETURNS;
  strcpy (czmil_attribute[9].name, "CZMIL Kd");
  czmil_attribute[9].scale = 10000.0;
  czmil_attribute[9].min = 0.0;
  czmil_attribute[9].max = 1.0;
  strcpy (czmil_attribute[10].name, "CZMIL Laser energy");
  czmil_attribute[10].scale = 10000.0;
  czmil_attribute[10].min = 0.0;
  czmil_attribute[10].max = 6.5535;
  strcpy (czmil_attribute[11].name, "CZMIL T0 interest point");
  czmil_attribute[11].scale = 100.0;
  czmil_attribute[11].min = 0.0;
  czmil_attribute[11].max = 64.0;
  strcpy (czmil_attribute[12].name, "CZMIL Detection probability");
  czmil_attribute[12].scale = 10000.0;
  czmil_attribute[12].min = 0.0;
  czmil_attribute[12].max = 1.0;
  strcpy (czmil_attribute[13].name, "CZMIL Processing mode");
  czmil_attribute[13].scale = 1.0;
  czmil_attribute[13].min = 0;
  czmil_attribute[13].max = 63;
  strcpy (czmil_attribute[14].name, "CZMIL Filter reason");
  czmil_attribute[14].scale = 1.0;
  czmil_attribute[14].min = 0;
  czmil_attribute[14].max = 63;
  strcpy (czmil_attribute[15].name, "CZMIL D_index");
  czmil_attribute[15].scale = 1.0;
  czmil_attribute[15].min = 0.0;
  czmil_attribute[15].max = 1023.0;
  strcpy (czmil_attribute[16].name, "CZMIL D_index_cube");
  czmil_attribute[16].scale = 1.0;
  czmil_attribute[16].min = 0.0;
  czmil_attribute[16].max = 1023.0;
  strcpy (czmil_attribute[17].name, "CZMIL User data");
  czmil_attribute[17].scale = 1.0;
  czmil_attribute[17].min = 0;
  czmil_attribute[17].max = 255;
  strcpy (czmil_attribute[18].name, "CZMIL Urban noise flags");
  czmil_attribute[18].scale = 1.0;
  czmil_attribute[18].min = 0;
  czmil_attribute[18].max = 255;
  strcpy (czmil_attribute[19].name, "CZMIL CSF Scan angle");
  czmil_attribute[19].scale = 100.0;
  czmil_attribute[19].min = 0.0;
  czmil_attribute[19].max = 360.0;
  strcpy (czmil_attribute[20].name, "CZMIL CSF Altitude");
  czmil_attribute[20].scale = 1000.0;
  czmil_attribute[20].min = -1000.0;
  czmil_attribute[20].max = 1000.0;
  strcpy (czmil_attribute[21].name, "CZMIL CSF Roll");
  czmil_attribute[21].scale = 100.0;
  czmil_attribute[21].min = -100.0;
  czmil_attribute[21].max = 100.0;
  strcpy (czmil_attribute[22].name, "CZMIL CSF Pitch");
  czmil_attribute[22].scale = 100.0;
  czmil_attribute[22].min = -100.0;
  czmil_attribute[22].max = 100.0;
  strcpy (czmil_attribute[23].name, "CZMIL CSF Heading");
  czmil_attribute[23].scale = 100.0;
  czmil_attribute[23].min = 0.0;
  czmil_attribute[23].max = 360.0;


  //  Also the CZMIL user flags.

  czmil_flag_name[0] = "CZMIL Land";
  czmil_flag_name[1] = "CZMIL Water";
  czmil_flag_name[2] = "CZMIL Hybrid";
  czmil_flag_name[3] = "CZMIL Shallow Water";
  czmil_flag_name[4] = "CZMIL Reprocessed";
  czmil_flag_name[5] = "CZMIL HP Filtered";
}



//  LAS attributes

void setLASAttributes (ATTR_BOUNDS *las_attribute)
{
  memset (las_attribute, 0, LAS_ATTRIBUTES * sizeof (ATTR_BOUNDS));

  strcpy (las_attribute[0].name, "LAS Intensity");
  las_attribute[0].scale = 1.0;
  las_attribute[0].min = 0;
  las_attribute[0].max = 65535;
  strcpy (las_attribute[1].name, "LAS Classification");
  las_attribute[1].scale = 1.0;
  las_attribute[1].min = 0;
  las_attribute[1].max = 255;
  strcpy (las_attribute[2].name, "LAS Scan angle rank");
  las_attribute[2].scale = 1.0;
  las_attribute[2].min = -90;
  las_attribute[2].max = 90;
  strcpy (las_attribute[3].name, "LAS Number of returns");
  las_attribute[3].scale = 1.0;
  las_attribute[3].min = 0;
  las_attribute[3].max = 5;
  strcpy (las_attribute[4].name, "LAS Return number");
  las_attribute[4].scale = 1.0;
  las_attribute[4].min = 0;
  las_attribute[4].max = 5;
  strcpy (las_attribute[5].name, "LAS Scan direction flag");
  las_attribute[5].scale = 1.0;
  las_attribute[5].min = 0;
  las_attribute[5].max = 1;
  strcpy (las_attribute[6].name, "LAS Edge of flightline flag");
  las_attribute[6].scale = 1.0;
  las_attribute[6].min = 0;
  las_attribute[6].max = 1;
  strcpy (las_attribute[7].name, "LAS User data");
  las_attribute[7].scale = 1.0;
  las_attribute[7].min = 0;
  las_attribute[7].max = 255;
  strcpy (las_attribute[8].name, "LAS Point source ID");
  las_attribute[8].scale = 1.0;
  las_attribute[8].min = 0;
  las_attribute[8].max = 65535;
  strcpy (las_attribute[9].name, "LAS Scanner channel");
  las_attribute[9].scale = 1.0;
  las_attribute[9].min = 0;
  las_attribute[9].max = 3;
  strcpy (las_attribute[10].name, "LAS RGB red");
  las_attribute[10].scale = 1.0;
  las_attribute[10].min = 0;
  las_attribute[10].max = 65535;
  strcpy (las_attribute[11].name, "LAS RGB green");
  las_attribute[11].scale = 1.0;
  las_attribute[11].min = 0;
  las_attribute[11].max = 65535;
  strcpy (las_attribute[12].name, "LAS RGB blue");
  las_attribute[12].scale = 1.0;
  las_attribute[12].min = 0;
  las_attribute[12].max = 65535;
  strcpy (las_attribute[13].name, "LAS NIR");
  las_attribute[13].scale = 1.0;
  las_attribute[13].min = 0;
  las_attribute[13].max = 65535;
  strcpy (las_attribute[14].name, "LAS Wave packet index");
  las_attribute[14].scale = 1.0;
  las_attribute[14].min = 0;
  las_attribute[14].max = 255;
  strcpy (las_attribute[15].name, "LAS Wave return location");
  las_attribute[15].scale = 10.0;
  las_attribute[15].min = 0;
  las_attribute[15].max = 214748364;
}



//  BAG attributes

void setBAGAttributes (ATTR_BOUNDS *bag_attribute)
{
  memset (bag_attribute, 0, BAG_ATTRIBUTES * sizeof (ATTR_BOUNDS));

  strcpy (bag_attribute[0].name, "BAG Uncertainty");
  bag_attribute[0].scale = 1000.0;
  bag_attribute[0].min = 0.0;
  bag_attribute[0].max = 1001.0;
}



void countTimeAttributes (int16_t time_attribute_num, int16_t *attribute_count)
{
  if (time_attribute_num) (*attribute_count)++;
}


void countGSFAttributes (int16_t *gsf_attribute_num, int16_t *attribute_count)
{
  for (int32_t i = 0 ; i < GSF_ATTRIBUTES ; i++)
    {
      if (gsf_attribute_num[i]) (*attribute_count)++;
    }
}


void countHOFAttributes (int16_t *hof_attribute_num, int16_t *attribute_count)
{
  for (int32_t i = 0 ; i < HOF_ATTRIBUTES ; i++)
    {
      if (hof_attribute_num[i]) (*attribute_count)++;
    }
}


void countTOFAttributes (int16_t *tof_attribute_num, int16_t *attribute_count)
{
  for (int32_t i = 0 ; i < TOF_ATTRIBUTES ; i++)
    {
      if (tof_attribute_num[i]) (*attribute_count)++;
    }
}


void countCZMILAttributes (int16_t *czmil_attribute_num, int16_t *attribute_count)
{
  for (int32_t i = 0 ; i < CZMIL_ATTRIBUTES ; i++)
    {
      if (czmil_attribute_num[i]) (*attribute_count)++;
    }
}


void countLASAttributes (int16_t *las_attribute_num, int16_t *attribute_count)
{
  for (int32_t i = 0 ; i < LAS_ATTRIBUTES ; i++)
    {
      if (las_attribute_num[i]) (*attribute_count)++;
    }
}


void countBAGAttributes (int16_t *bag_attribute_num, int16_t *attribute_count)
{
  for (int32_t i = 0 ; i < BAG_ATTRIBUTES ; i++)
    {
      if (bag_attribute_num[i]) (*attribute_count)++;
    }
}


int32_t gsfAttributesSet (int16_t *gsf_attribute_num)
{
  int32_t count = 0;

  for (int32_t i = 0 ; i < GSF_ATTRIBUTES ; i++)
    {
      if (gsf_attribute_num[i]) count++;
    }

  return (count);
}


int32_t hofAttributesSet (int16_t *hof_attribute_num)
{
  int32_t count = 0;

  for (int32_t i = 0 ; i < HOF_ATTRIBUTES ; i++)
    {
      if (hof_attribute_num[i]) count++;
    }

  return (count);
}


int32_t tofAttributesSet (int16_t *tof_attribute_num)
{
  int32_t count = 0;

  for (int32_t i = 0 ; i < TOF_ATTRIBUTES ; i++)
    {
      if (tof_attribute_num[i]) count++;
    }

  return (count);
}


int32_t czmilAttributesSet (int16_t *czmil_attribute_num)
{
  int32_t count = 0;

  for (int32_t i = 0 ; i < CZMIL_ATTRIBUTES ; i++)
    {
      if (czmil_attribute_num[i]) count++;
    }

  return (count);
}


int32_t lasAttributesSet (int16_t *las_attribute_num)
{
  int32_t count = 0;

  for (int32_t i = 0 ; i < LAS_ATTRIBUTES ; i++)
    {
      if (las_attribute_num[i]) count++;
    }

  return (count);
}


int32_t bagAttributesSet (int16_t *bag_attribute_num)
{
  int32_t count = 0;

  for (int32_t i = 0 ; i < BAG_ATTRIBUTES ; i++)
    {
      if (bag_attribute_num[i]) count++;
    }

  return (count);
}


void getGSFAttributes (float *l_attr, int16_t time_attribute_num, int16_t *gsf_attribute_num, gsfSwathBathyPing *mb_ping, int32_t beam)
{
  int32_t index = time_attribute_num - 1;
  if (time_attribute_num) l_attr[index] = NINT ((double) mb_ping->ping_time.tv_sec / 60.0);


  for (int32_t i = 0 ; i < GSF_ATTRIBUTES ; i++)
    {
      int32_t index = gsf_attribute_num[i] - 1;

      l_attr[index]= 0.0;

      if (gsf_attribute_num[i])
        {
          switch (i)
            {
            case 0:
              l_attr[index] = mb_ping->heading;
              break;

            case 1:
              l_attr[index] = mb_ping->pitch;
              break;

            case 2:
              l_attr[index] = mb_ping->roll;
              break;

            case 3:
              l_attr[index] = mb_ping->heave;
              break;

            case 4:
              l_attr[index] = mb_ping->course;
              break;

            case 5:
              l_attr[index] = mb_ping->speed;
              break;

            case 6:
              l_attr[index] = mb_ping->height;
              break;

            case 7:
              l_attr[index] = mb_ping->sep;
              break;

            case 8:
              l_attr[index] = mb_ping->tide_corrector;
              break;

            case 9:
              l_attr[index] = (mb_ping->ping_flags & (GSF_PING_USER_FLAG_15 | GSF_PING_USER_FLAG_14)) >> 14;
              break;

            case 10:
              if (mb_ping->across_track) l_attr[index] = mb_ping->across_track[beam];
              break;

            case 11:
              if (mb_ping->across_track) l_attr[index] = mb_ping->along_track[beam];
              break;

            case 12:
              if (mb_ping->travel_time) l_attr[index] = mb_ping->travel_time[beam];
              break;

            case 13:
              if (mb_ping->beam_angle) l_attr[index] = mb_ping->beam_angle[beam];
              break;

            case 14:
              if (mb_ping->beam_angle_forward) l_attr[index] = mb_ping->beam_angle_forward[beam];
              break;

            case 15:
              if (mb_ping->mc_amplitude) l_attr[index] = mb_ping->mc_amplitude[beam];
              break;

            case 16:
              if (mb_ping->mr_amplitude) l_attr[index] = mb_ping->mr_amplitude[beam];
              break;

            case 17:
              if (mb_ping->echo_width) l_attr[index] = mb_ping->echo_width[beam];
              break;

            case 18:
              if (mb_ping->quality_factor) l_attr[index] = mb_ping->quality_factor[beam];
              break;

            case 19:
              if (mb_ping->signal_to_noise) l_attr[index] = mb_ping->signal_to_noise[beam];
              break;

            case 20:
              if (mb_ping->receive_heave) l_attr[index] = mb_ping->receive_heave[beam];
              break;

            case 21:
              if (mb_ping->depth_error) l_attr[index] = mb_ping->depth_error[beam];
              break;

            case 22:
              if (mb_ping->across_track_error) l_attr[index] = mb_ping->across_track_error[beam];
              break;

            case 23:
              if (mb_ping->along_track_error) l_attr[index] = mb_ping->along_track_error[beam];
              break;

            case 24:
              if (mb_ping->quality_flags) l_attr[index] = mb_ping->quality_flags[beam];
              break;

            case 25:
              if (mb_ping->beam_flags) l_attr[index] = mb_ping->beam_flags[beam];
              break;

            case 26:
              if (mb_ping->beam_angle_forward) l_attr[index] = mb_ping->beam_angle_forward[beam];
              break;

            case 27:
              if (mb_ping->sector_number) l_attr[index] = mb_ping->sector_number[beam];
              break;

            case 28:
              if (mb_ping->detection_info) l_attr[index] = mb_ping->detection_info[beam];
              break;

            case 29:
              if (mb_ping->incident_beam_adj) l_attr[index] = mb_ping->incident_beam_adj[beam];
              break;

            case 30:
              if (mb_ping->system_cleaning) l_attr[index] = mb_ping->system_cleaning[beam];
              break;

            case 31:
              if (mb_ping->doppler_corr) l_attr[index] = mb_ping->doppler_corr[beam];
              break;
            }
        }
    }
}



void getHOFAttributes (float *l_attr, int16_t time_attribute_num, int16_t *hof_attribute_num, HYDRO_OUTPUT_T *hof)
{
  int32_t index = time_attribute_num - 1;
  if (time_attribute_num) l_attr[index] = hof->timestamp / 60000000;


  for (int32_t i = 0 ; i < HOF_ATTRIBUTES ; i++)
    {
      int32_t index = hof_attribute_num[i] - 1;

      l_attr[index]= 0.0;

      if (hof_attribute_num[i])
        {
          switch (i)
            {
            case 0:
              l_attr[index] = hof->haps_version;
              break;

            case 1:
              l_attr[index] = hof->position_conf;
              break;

            case 2:
              l_attr[index] = hof->status;
              break;

            case 3:
              l_attr[index] = hof->suggested_dks;
              break;

            case 4:
              l_attr[index] = hof->suspect_status;
              break;

            case 5:
              l_attr[index] = hof->tide_status;
              break;

            case 6:
              l_attr[index] = hof->abdc;
              break;

            case 7:
              l_attr[index] = hof->sec_abdc;
              break;

            case 8:
              l_attr[index] = hof->data_type;
              break;

            case 9:
              l_attr[index] = hof->land_mode;
              break;

            case 10:
              l_attr[index] = hof->classification_status;
              break;

            case 11:
              l_attr[index] = hof->wave_height;
              break;

            case 12:
              l_attr[index] = hof->elevation;
              break;

            case 13:
              l_attr[index] = hof->topo;
              break;

            case 14:
              l_attr[index] = hof->altitude;
              break;

            case 15:
              l_attr[index] = hof->kgps_topo;
              break;

            case 16:
              l_attr[index] = hof->kgps_datum;
              break;

            case 17:
              l_attr[index] = hof->kgps_water_level;
              break;

            case 18:
              l_attr[index] = hof->k;
              break;

            case 19:
              l_attr[index] = hof->intensity;
              break;

            case 20:
              l_attr[index] = hof->bot_conf;
              break;

            case 21:
              l_attr[index] = hof->sec_bot_conf;
              break;

            case 22:
              l_attr[index] = hof->nadir_angle;
              break;

            case 23:
              l_attr[index] = hof->scanner_azimuth;
              break;

            case 24:
              l_attr[index] = hof->sfc_fom_apd;
              break;

            case 25:
              l_attr[index] = hof->sfc_fom_ir;
              break;

            case 26:
              l_attr[index] = hof->sfc_fom_ram;
              break;

            case 27:
              l_attr[index] = hof->depth_conf;
              break;

            case 28:
              l_attr[index] = hof->sec_depth_conf;
              break;

            case 29:
              l_attr[index] = hof->warnings;
              break;

            case 30:
              l_attr[index] = hof->warnings2;
              break;

            case 31:
              l_attr[index] = hof->warnings3;
              break;

            case 32:
              l_attr[index] = hof->calc_bfom_thresh_times10[0];
              break;

            case 33:
              l_attr[index] = hof->calc_bfom_thresh_times10[1];
              break;

            case 34:
              l_attr[index] = hof->calc_bot_run_required[0];
              break;

            case 35:
              l_attr[index] = hof->calc_bot_run_required[1];
              break;

            case 36:
              l_attr[index] = hof->bot_bin_first;
              break;

            case 37:
              l_attr[index] = hof->bot_bin_second;
              break;

            case 38:
              l_attr[index] = hof->bot_bin_used_pmt;
              break;

            case 39:
              l_attr[index] = hof->sec_bot_bin_used_pmt;
              break;

            case 40:
              l_attr[index] = hof->bot_bin_used_apd;
              break;

            case 41:
              l_attr[index] = hof->sec_bot_bin_used_apd;
              break;

            case 42:
              l_attr[index] = hof->bot_channel;
              break;

            case 43:
              l_attr[index] = hof->sec_bot_chan;
              break;

            case 44:
              l_attr[index] = hof->sfc_bin_apd;
              break;

            case 45:
              l_attr[index] = hof->sfc_bin_ir;
              break;

            case 46:
              l_attr[index] = hof->sfc_bin_ram;
              break;

            case 47:
              l_attr[index] = hof->sfc_channel_used;
              break;
            }
        }
    }
}



void getTOFAttributes (float *l_attr, int16_t time_attribute_num, int16_t *tof_attribute_num, TOPO_OUTPUT_T *tof)
{
  int32_t index = time_attribute_num - 1;
  if (time_attribute_num) l_attr[index] = tof->timestamp / 60000000;


  for (int32_t i = 0 ; i < TOF_ATTRIBUTES ; i++)
    {
      int32_t index = tof_attribute_num[i] - 1;

      l_attr[index]= 0.0;

      if (tof_attribute_num[i])
        {
          switch (i)
            {
            case 0:
              l_attr[index] = tof->classification_status;
              break;

            case 1:
              l_attr[index] = tof->altitude;
              break;

            case 2:
              l_attr[index] = tof->intensity_first;
              break;

            case 3:
              l_attr[index] = tof->intensity_last;
              break;

            case 4:
              l_attr[index] = tof->conf_first;
              break;

            case 5:
              l_attr[index] = tof->conf_last;
              break;

            case 6:
              l_attr[index] = tof->nadir_angle;
              break;

            case 7:
              l_attr[index] = tof->scanner_azimuth;
              break;

            case 8:
              l_attr[index] = tof->pos_conf;
              break;
            }
        }
    }
}



void getCZMILAttributes (float *l_attr, int16_t time_attribute_num, int16_t *czmil_attribute_num, CZMIL_CPF_Data *cpf, CZMIL_CSF_Data *csf,
                         int32_t channel, int32_t retnum, uint16_t major_version)
{
  //  Using this to make sure we only compute urban noise filter flags ONCE per shot.

  static uint64_t prev_time = -1;
  static uint8_t urban_noise_flags = 0;
  static uint8_t rets[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};


  int32_t index = time_attribute_num - 1;
  if (time_attribute_num) l_attr[index] = cpf->timestamp / 60000000;


  for (int32_t i = 0 ; i < CZMIL_ATTRIBUTES ; i++)
    {
      int32_t index = czmil_attribute_num[i] - 1;

      l_attr[index]= 0.0;

      if (czmil_attribute_num[i])
        {
          switch (i)
            {
            case 0:
              l_attr[index] = cpf->channel[channel][retnum].reflectance;


              //  Pre 2.0 CZMIL files used .99 for NULL reflectance.  We set it to 0.0 so that the color spectrum is correct in pfmEdit3D.

              if (major_version < 2 && l_attr[index] > 0.98) l_attr[index] = 0.0;
              break;

            case 1:
              l_attr[index] = cpf->channel[channel][retnum].classification;
              break;

            case 2:
              l_attr[index] = cpf->off_nadir_angle;
              break;

            case 3:
              l_attr[index] = cpf->water_level;
              break;

            case 4:
              l_attr[index] = cpf->local_vertical_datum_offset;
              break;

            case 5:
              l_attr[index] = cpf->channel[channel][retnum].interest_point;
              break;

            case 6:
              l_attr[index] = cpf->channel[channel][retnum].ip_rank;
              break;

            case 7:
              l_attr[index] = channel;
              break;

            case 8:
              l_attr[index] = retnum;
              break;

            case 9:
              l_attr[index] = cpf->kd;
              break;

            case 10:
              l_attr[index] = cpf->laser_energy;
              break;

            case 11:
              l_attr[index] = cpf->t0_interest_point;
              break;

            case 12:
              l_attr[index] = cpf->channel[channel][retnum].probability;
              break;

            case 13:
              l_attr[index] = cpf->optech_classification[channel];
              break;

            case 14:
              l_attr[index] = cpf->channel[channel][retnum].filter_reason;
              break;

            case 15:
              l_attr[index] = cpf->channel[channel][retnum].d_index;
              break;

            case 16:
              l_attr[index] = cpf->d_index_cube;
              break;

            case 17:
              l_attr[index] = cpf->user_data;
              break;


              //  This is where we set the urban noise filter flags (also know as Nick Johnson flags ;-)

            case 18:

              //  We only want to do this once per shot, not for every return.

              if (cpf->timestamp != prev_time)
                {
                  for (int32_t j = 0 ; j < 9 ; j++) rets[j] = 0;

                  urban_noise_flags = CZMIL_URBAN_TOPO;
                  uint8_t topo = NVTrue;

                  prev_time = cpf->timestamp;


                  //  Note that we are not checking the IR or deep channels.

                  for (int32_t j = 0 ; j < 7 ; j++)
                    {
                      //  Only use shots where all channels were processed as land.

                      if (cpf->optech_classification[j] != 1)
                        {
                          //  Zero out return counts for channels that aren't all topo.

                          for (int32_t m = 0 ; m < 9 ; m++) rets[m] = 0;
                          urban_noise_flags = CZMIL_URBAN_NOT_TOPO;
                          topo = NVFalse;
                          break;
                        }


                      //  Count the valid returns for this channel.

                      for (int32_t k = 0 ; k < cpf->returns[j] ; k++)
                        {
                          if (!(cpf->channel[j][k].status & CZMIL_RETURN_INVAL)) rets[j]++;
                        }
                    }


                  if (topo)
                    {
                      uint8_t cnt = 0;


                      //  Note that we are not checking the IR or deep channels.

                      for (int32_t j = 0 ; j < 7 ; j++)
                        {
                          if (rets[j] > 3) cnt++;
                        }

                      if (cnt <= 3) urban_noise_flags |= CZMIL_URBAN_MAJORITY;
                    }
                }


              //  Mark topo returns, we only do this for the shallow channel returns.

              if (channel < 7)
                {
                  //  Mark topo returns with 2 valid returns per channel (non-topo processed channels will have rets[channel] set to 0).

                  if (rets[channel] == 1 || rets[channel] == 2)
                    {
                      l_attr[index] = urban_noise_flags | CZMIL_URBAN_HARD_HIT;
                    }


                  //  If there greater than 3 returns we have to make sure that CZMIL_URBAN_MAJORITY is turned off.  This is for
                  //  the channels on a CZMIL_URBAN_MAJORITY shot.  Those with 4 returns will just be marked CZMIL_URBAN_TOPO.
                  //  Those with 5 or more returns will be marked as CZMIL_URBAN_SOFT.

                  else if (rets[channel] > 3)
                    {
                      urban_noise_flags &= ~CZMIL_URBAN_MAJORITY;


                      //  Mark topo returns with 5 or more valid returns per channel (non-topo processed channels will have
                      //  rets[channel] set to 0).

                      if (rets[channel] > 4)
                        {
                          l_attr[index] = urban_noise_flags | CZMIL_URBAN_SOFT_HIT;
                        }
                    }
                  else
                    {
                      l_attr[index] = urban_noise_flags;
                    }
                }

              break;


              //  The following are from the CSF file.

            case 19:
              l_attr[index] = csf->scan_angle;
              break;

            case 20:
              l_attr[index] = csf->altitude;
              break;

            case 21:
              l_attr[index] = csf->roll;
              break;

            case 22:
              l_attr[index] = csf->pitch;
              break;

            case 23:
              l_attr[index] = csf->heading;
              break;
            }
        }
    }
}



void getLASAttributes (float *l_attr, int16_t time_attribute_num, int16_t *las_attribute_num, float *posix_minutes, LASpoint *record, uint8_t ext)
{
  int32_t index = time_attribute_num - 1;
  if (time_attribute_num) l_attr[index] = *posix_minutes;


  for (int32_t i = 0 ; i < LAS_ATTRIBUTES ; i++)
    {
      int32_t index = las_attribute_num[i] - 1;

      l_attr[index]= 0.0;

      if (las_attribute_num[i])
        {
          if (ext)
            {
              switch (i)
                {
                case 0:
                  l_attr[index] = record->intensity;
                  break;

                case 1:
                  l_attr[index] = record->extended_classification;
                  break;

                case 2:
                  l_attr[index] = record->extended_scan_angle;
                  break;

                case 3:
                  l_attr[index] = record->extended_number_of_returns;
                  break;

                case 4:
                  l_attr[index] = record->extended_return_number;
                  break;

                case 5:
                  l_attr[index] = record->scan_direction_flag;
                  break;

                case 6:
                  l_attr[index] = record->edge_of_flight_line;
                  break;

                case 7:
                  l_attr[index] = record->user_data;
                  break;

                case 8:
                  l_attr[index] = record->point_source_ID;
                  break;

                case 9:
                  l_attr[index] = record->extended_scanner_channel;
                  break;

                case 10:
                  l_attr[index] = record->rgb[0];
                  break;

                case 11:
                  l_attr[index] = record->rgb[1];
                  break;

                case 12:
                  l_attr[index] = record->rgb[2];
                  break;

                case 13:
                  l_attr[index] = record->rgb[3];
                  break;

                case 14:
                  l_attr[index] = record->wavepacket.getIndex ();
                  break;

                case 15:
                  l_attr[index] = record->wavepacket.getLocation ();
                  break;
                }
            }
          else
            {
              switch (i)
                {
                case 0:
                  l_attr[index] = record->intensity;
                  break;

                case 1:
                  l_attr[index] = record->classification;
                  break;

                case 2:
                  l_attr[index] = record->scan_angle_rank;
                  break;

                case 3:
                  l_attr[index] = record->number_of_returns;
                  break;

                case 4:
                  l_attr[index] = record->return_number;
                  break;

                case 5:
                  l_attr[index] = record->scan_direction_flag;
                  break;

                case 6:
                  l_attr[index] = record->edge_of_flight_line;
                  break;

                case 7:
                  l_attr[index] = record->user_data;
                  break;

                case 8:
                  l_attr[index] = record->point_source_ID;
                  break;

                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                  l_attr[index] = 0;
                  break;
                }
            }
        }
    }
}



void getBAGAttributes (float *l_attr, int16_t time_attribute_num, int16_t *bag_attribute_num, BAG_RECORD *bag)
{
  int32_t index = time_attribute_num - 1;
  if (time_attribute_num) l_attr[index] = ABE_NULL_TIME;


  for (int32_t i = 0 ; i < BAG_ATTRIBUTES ; i++)
    {
      int32_t index = bag_attribute_num[i] - 1;

      l_attr[index]= 0.0;

      if (bag_attribute_num[i])
        {
          switch (i)
            {
            case 0:
              l_attr[index] = bag->uncert;
              break;
            }
        }
    }
}
