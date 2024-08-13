//
// CLI tool for listing page attributes of raster files for CUPS.
//
// Copyright © 2024 by OpenPrinting.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

#include <cups/raster.h>
#include <cups/raster-private.h>
#include <zlib.h>


/*
 * 'raster_cb()' - Callback for reading data from gzFile.
 */

static ssize_t                          /* O - Bytes read or -1 on error */
raster_cb(gzFile        ctx,            /* I - File pointer */
          unsigned char *buffer,        /* I - Buffer */
          size_t        length)         /* I - Bytes to read */
{   
  return ((ssize_t)gzread(ctx, buffer, (unsigned)length));
}


int main(int argc, char *argv[])
{
  int			page_num = -1,		// Number of the selected page - -1 to show all
			show_help = 0,		// Should show help?
			define_page = 0,	// Page number specified?
			i = 1,			// Iterator
			curpage = 0;		// Current page numbe
  cups_page_header2_t	header;			// Raster header
  gzFile		fp;			// Compressed raster file
  cups_raster_t		*ras;			// Raster stream
  unsigned char		*buffer = NULL;		// Line buffer
  unsigned int		bufsize = 0;		// Size of line buffer
  char			*filename = NULL;	// Name of gzipped file
  char			*page = NULL;		// String of page number


  if (argc < 2 || argc > 4)
  {
    help();
    return (0);
  }

  while(argv[i])
  {
    if (!strncmp(argv[i], "--page", 6))
    {
      define_page = 1;

      if (argv[i][6] == '=')
        page = argv[i] + 6;
      else
        page = argv[++i];
    }
    else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
      show_help = 1;
    else
      filename = argv[i];

    i ++;
  }

  if (show_help)
  {
    help();
    return (0);
  }

  if (!filename || (define_page && (!page || !page[0])))
  {
    help();
    return (1);
  }

  if (!(page_num = atoi(page)))
  {
    fprintf(stderr, "Provided string for page number is not a number.\n");
    return (1);
  }

  if (!(fp = gzopen(filename, "r")))
  {
    fprintf(stderr, "File %s cannot be opened with gz.\n", filename);
    return (1);
  }

  if ((ras = cupsRasterOpenIO((cups_raster_iocb_t)raster_cb, fp, CUPS_RASTER_READ)) == NULL)
  {
    fprintf(stderr, "File %s cannot be read as raster stream", filename);
    gzclose(f);
    return (1);
  }

  while (cupsRasterReadHeader2(ras, &header))
  {
    curpage ++;

   /*
    * Ignore the page if it is not the specified page number...
    */

    if (page_num > 0 && curpage != page_num)
      continue;

    printf("PAGE %d:\n", curpage);

    if (!strcmp(header.MediaClass, "PwgRaster"))
    {
      printf("PWG Raster Page Attributes:\n\n");

      printf("MediaColor = \"%s\"\n", header.MediaColor);

      printf("MediaType = \"%s\"\n", header.MediaType);

      printf("PrintContentOptimize = \"%s\"\n", header.OutputType);

      printf("CutMedia = %d\n", header.CutMedia);

      printf("Duplex = %d\n", header.Duplex);

      printf("HWResolution = [ %d %d ]\n", header.HWResolution[0], header.HWResolution[1]);

      printf("InsertSheet = %d\n", header.InsertSheet);

      printf("Jog = %d\n", header.Jog);

      printf("LeadingEdge = %d\n", header.LeadingEdge);

      printf("MediaPosition = %d\n", header.MediaPosition);

      printf("MediaWeightMetric = %d\n", header.MediaWeight);

      printf("NumCopies = %d\n", header.NumCopies);

      printf("Orientation = %d\n", header.Orientation);

      printf("PageSize = [ %d %d ]\n", header.PageSize[0], header.PageSize[1]);

      printf("Tumble = %d\n", header.Tumble);

      printf("Width = %d\n", header.cupsWidth);

      printf("Height = %d\n", header.cupsHeight);

      printf("BitsPerColor = %d\n", header.cupsBitsPerColor);

      printf("BitsPerPixel = %d\n", header.cupsBitsPerPixel);

      printf("BytesPerLine = %d\n", header.cupsBytesPerLine);

      printf("ColorOrder = %s\n", header.cupsColorOrder == CUPS_ORDER_CHUNKED ? "CHUNKED" : header.cupsColorOrder == CUPS_ORDER_BANDED ? "BANDED" : header.cupsColorOrder == CUPS_ORDER_PLANAR ? "PLANAR" : "UNKNOWN");

      printf("ColorSpace = %s\n", _cupsRasterColorSpaceString(header.cupsColorSpace));

      printf("NumColors = %d\n", header.cupsNumColors);

      printf("TotalPageCount = %u\n", header.cupsInteger[CUPS_RASTER_PWG_TotalPageCount]);

      printf("CrossFeedTransform = %d\n", header.cupsInteger[CUPS_RASTER_PWG_CrossFeedTransform]);

      printf("FeedTransform = %d\n", header.cupsInteger[CUPS_RASTER_PWG_FeedTransform]);

      printf("ImageBoxLeft = %u\n", header.cupsInteger[CUPS_RASTER_PWG_ImageBoxLeft]);

      printf("ImageBoxTop = %u\n", header.cupsInteger[CUPS_RASTER_PWG_ImageBoxTop]);

      printf("ImageBoxRight = %u\n", header.cupsInteger[CUPS_RASTER_PWG_ImageBoxRight]);

      printf("ImageBoxBottom = %u\n", header.cupsInteger[CUPS_RASTER_PWG_ImageBoxBottom]);

      printf("AlternatePrimary = %06x (%u, %u, %u)\n", header.cupsInteger[CUPS_RASTER_PWG_AlternatePrimary], (header.cupsInteger[CUPS_RASTER_PWG_AlternatePrimary] >> 16) & 255, (header.cupsInteger[CUPS_RASTER_PWG_AlternatePrimary] >> 8) & 255, header.cupsInteger[CUPS_RASTER_PWG_AlternatePrimary] & 255);

      printf("PrintQuality = %u\n", header.cupsInteger[CUPS_RASTER_PWG_PrintQuality]);

      printf("VendorIdentifier = %u\n", header.cupsInteger[CUPS_RASTER_PWG_VendorIdentifier]);

      printf("VendorLength = %u\n", header.cupsInteger[CUPS_RASTER_PWG_VendorLength]);

      unsigned char	*data = (unsigned char *)header.cupsReal;
      unsigned		dataidx, datalen = header.cupsInteger[15];

      printf("VendorData =\n");

      for (dataidx = 0; dataidx < datalen; dataidx ++)
      {
	if ((dataidx & 7) == 0)
	  printf("\n   ");

	printf(" %02X", *data++);
      }

      printf("\n");

      printf("RenderingIntent = \"%s\"\n", header.cupsRenderingIntent);

      printf("PageSizeName = \"%s\"\n", header.cupsPageSizeName);
    }
    else
    {
      printf("CUPS Raster Page Attributes:\n\n");

      printf("MediaClass = \"%s\"\n", header.MediaClass);

      printf("MediaColor = \"%s\"\n", header.MediaColor);

      printf("MediaType = \"%s\"\n", header.MediaType);

      printf("OutputType = \"%s\"\n", header.OutputType);

      printf("AdvanceDistance = %d\n", header.AdvanceDistance);

      printf("AdvanceMedia = %d\n", header.AdvanceMedia);

      printf("Collate = %d\n", header.Collate);

      printf("CutMedia = %d\n", header.CutMedia);

      printf("Duplex = %d\n", header.Duplex);

      printf("HWResolution = [ %d %d ]\n", header.HWResolution[0], header.HWResolution[1]);

      printf("ImagingBoundingBox = [ %d %d %d %d ]\n", header.ImagingBoundingBox[0], header.ImagingBoundingBox[1], header.ImagingBoundingBox[2], header.ImagingBoundingBox[3]);

      printf("InsertSheet = %d\n", header.InsertSheet);

      printf("Jog = %d\n", header.Jog);

      printf("LeadingEdge = %d\n", header.LeadingEdge);

      printf("Margins = [ %d %d ]\n", header.Margins[0], header.Margins[1]);

      printf("ManualFeed = %d\n", header.ManualFeed);

      printf("MediaPosition = %d\n", header.MediaPosition);

      printf("MediaWeight = %d\n", header.MediaWeight);

      printf("MirrorPrint = %d\n", header.MirrorPrint);

      printf("NegativePrint = %d\n", header.NegativePrint);

      printf("NumCopies = %d\n", header.NumCopies);

      printf("Orientation = %d\n", header.Orientation);

      printf("OutputFaceUp = %d\n", header.OutputFaceUp);

      printf("PageSize = [ %d %d ]\n", header.PageSize[0], header.PageSize[1]);

      printf("Separations = %d\n", header.Separations);

      printf("TraySwitch = %d\n", header.TraySwitch);

      printf("Tumble = %d\n", header.Tumble);

      printf("cupsWidth = %d\n", header.cupsWidth);

      printf("cupsHeight = %d\n", header.cupsHeight);

      printf("cupsMediaType = %d\n", header.cupsMediaType);

      printf("cupsBitsPerColor = %d\n", header.cupsBitsPerColor);

      printf("cupsBitsPerPixel = %d\n", header.cupsBitsPerPixel);

      printf("cupsBytesPerLine = %d\n", header.cupsBytesPerLine);

      printf("cupsColorOrder = %s\n", header.cupsColorOrder == CUPS_ORDER_CHUNKED ? "CHUNKED" : header.cupsColorOrder == CUPS_ORDER_BANDED ? "BANDED" : header.cupsColorOrder == CUPS_ORDER_PLANAR ? "PLANAR" : "UNKNOWN");

      printf("cupsColorSpace = %s\n", _cupsRasterColorSpaceString(header.cupsColorSpace));

      printf("cupsCompression = %d\n", header.cupsCompression);

      printf("cupsRowCount = %d\n", header.cupsRowCount);

      printf("cupsRowFeed = %d\n", header.cupsRowFeed);

      printf("cupsRowStep = %d\n", header.cupsRowStep);

      printf("cupsNumColors = %d\n", header.cupsNumColors);

      printf("cupsBorderlessScalingFactor = %f\n", header.cupsBorderlessScalingFactor);

      printf("cupsPageSize = [ %f %f ]\n", header.cupsPageSize[0], header.cupsPageSize[1]);

      printf("cupsImagingBBox = [ %f %f %f %f ]\n", header.cupsImagingBBox[0], header.cupsImagingBBox[1], header.cupsImagingBBox[2], header.cupsImagingBBox[3]);

      for (i = 0; i < 16; i ++)
      {
	printf("cupsInteger%d = %u\n", i + 1, header.cupsInteger[i]);
      }

      for (i = 0; i < 16; i ++)
      {
	printf("cupsReal%d = %f\n", i + 1, header.cupsReal[i]);
      }

      for (i = 0; i < 16; i ++)
      {
	printf("cupsString%d = \"%s\"\n", i + 1, header.cupsString[i]);
      }

      printf("cupsMarkerType = \"%s\"\n", header.cupsMarkerType);

      printf("cupsRenderingIntent = \"%s\"\n", header.cupsRenderingIntent);

      printf("cupsPageSizeName = \"%s\"\n", header.cupsPageSizeName);
    }

    printf("\n\n");

   /*
    * Go through raster data of the current page, to reach the next raster header of next page...
    */

    if (header.cupsBytesPerLine > bufsize)
    {
      bufsize = header.cupsBytesPerLine;
      buffer  = (unsigned char *)realloc(buffer, bufsize);
    }

    for (int y = header.cupsHeight; y > 0; y --)
      cupsRasterReadPixels(ras, buffer, header.cupsBytesPerLine);
  }

  if (!curpage)
  {
    fprintf(stderr, "Raster header from file %s could not be read.\n", filename);
    return (1);
  }

  if (page_num > 0 && curpage != page_num)
  {
    fprintf(stderr, "Page %d could not be loaded - not enough pages\n", page_num);
    return (1);
  }

  cupsRasterClose(ras);
  gzclose(fp);

  return (0);
}
