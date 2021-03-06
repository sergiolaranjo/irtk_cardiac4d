/*=========================================================================

  Library   : Image Registration Toolkit (IRTK)
  Module    : $Id$
  Copyright : Imperial College, Department of Computing
              Visual Information Processing (VIP), 2008 onwards
  Date      : $Date$
  Version   : $Revision$
  Changes   : $Author$

=========================================================================*/

#ifndef _IRTKFILECVTOIMAGE

#define _IRTKFILECVTOIMAGE

#ifdef HAS_OPENCV
#include <cv.h>
#include <highgui.h>

/**
 * Class for reading images in PGM file format.
 *
 * This is a class which reads images in PGM file format and converts them
 * into images. The PGM (portable graymap) file format is a file format for
 * 2D images and is defined in pgm(1). At the moment only images in PGM raw
 * file format are supported.
 */

class irtkFileOpenCVToImage : public irtkFileToImage
{
protected:

  IplImage* _pimage;

  virtual void ReadHeader();

public:

  /// Return name of class
  virtual const char *NameOfClass();

  /// Return whether file has correct header
  static int CheckHeader(const char *);

  /// Get output
  virtual irtkImage *GetOutput();

  /// Set input
  virtual void SetInput (const char *);
};

#endif

#endif
