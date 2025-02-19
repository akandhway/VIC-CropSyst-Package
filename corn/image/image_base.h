#ifndef image_baseH
#define image_baseH

#include "corn/primitive.h"

/* This is the abstract base class for all graphic image

*/
namespace CORN {

typedef nat32 Image_color_index;
//______________________________________________________________________________
class Image_base
{
public:
   inline Image_base() {}
   virtual Image_color_index get_transparent_color() = 0;
   inline void copy_from(const Image_base &to_be_copied)
      {} // Currently nothing to copy
   virtual nat16 get_width() const = 0;  // in pixels
   virtual nat16 get_height() const = 0; // in pixels
public: // drawing primatives
   virtual void point(nat16 x, nat16 y, Image_color_index color) = 0;
   virtual void line(nat16 x1,nat16 y1,nat16 x2,nat16 y2,Image_color_index color);
   void checker_board_transparent();
   // This creates a checkerboard of transparent pixels.
   // When overlaying this image over a solid image, it gives
   // the effect of the underimage blended with this image.
   // Used for quick and dirty color shading.
};
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/
#endif

